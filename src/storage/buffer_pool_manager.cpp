#include "storage/buffer_pool_manager.h"

namespace minidb
{

    BufferPoolManager::BufferPoolManager(
        size_t pool_size,
        DiskManager *disk_manager)
        : pool_size_(pool_size),
          disk_manager_(disk_manager),
          frames_(pool_size),
          replacer_(pool_size)
    {
    }

    // --------------------------------------------------
    // AcquireFrame()
    // --------------------------------------------------

    bool BufferPoolManager::AcquireFrame(frame_id_t &frame_id)
    {
        // Step 1: Look for an unused frame.
        for (size_t i = 0; i < pool_size_; ++i)
        {
            if (frames_[i].page.GetPageId() == INVALID_PAGE_ID)
            {
                frame_id = static_cast<frame_id_t>(i);
                return true;
            }
        }

        // Step 2: No free frame exists. Ask LRU for a victim.
        if (!replacer_.Victim(&frame_id))
        {
            // All frames are pinned, or no frame is eligible.
            return false;
        }

        Frame &victim = frames_[frame_id];

        // Defensive check: LRU should never return a pinned frame.
        if (victim.pin_count != 0)
        {
            return false;
        }

        page_id_t old_page_id = victim.page.GetPageId();

        // Step 3: A dirty page must be written before eviction.
        if (victim.is_dirty)
        {
            if (!disk_manager_->WritePage(old_page_id, victim.page))
            {
                // The write failed. Keep the page and its mapping.
                // Restore the frame as an eviction candidate.
                replacer_.Unpin(frame_id);
                return false;
            }
        }

        // Step 4: Remove the old page-to-frame mapping.
        page_table_.erase(old_page_id);

        // Step 5: Reset the frame so it can hold another page.
        victim.page.ResetMemory();
        victim.page.SetPageId(INVALID_PAGE_ID);
        victim.pin_count = 0;
        victim.is_dirty = false;

        return true;
    }

    // --------------------------------------------------
    // FetchPage()
    // --------------------------------------------------

    Page *BufferPoolManager::FetchPage(page_id_t page_id)
    {
        if (disk_manager_ == nullptr || page_id == INVALID_PAGE_ID)
        {
            return nullptr;
        }

        // Step 1: Check whether the page is already in RAM.
        auto it = page_table_.find(page_id);

        if (it != page_table_.end())
        {
            frame_id_t frame_id = it->second;
            Frame &frame = frames_[frame_id];

            // The caller is using this page.
            ++frame.pin_count;

            // A pinned frame must not be evicted.
            replacer_.Pin(frame_id);

            return &frame.page;
        }

        // Step 2: Find a free frame or evict an existing page.
        frame_id_t frame_id;

        if (!AcquireFrame(frame_id))
        {
            return nullptr;
        }

        Frame &frame = frames_[frame_id];

        // Step 3: Read the requested page from disk.
        if (!disk_manager_->ReadPage(page_id, frame.page))
        {
            // The requested page could not be read.
            frame.page.ResetMemory();
            frame.page.SetPageId(INVALID_PAGE_ID);
            frame.pin_count = 0;
            frame.is_dirty = false;

            return nullptr;
        }

        // Step 4: Register the page in the page table.
        page_table_[page_id] = frame_id;

        // Step 5: The caller is using the page.
        frame.pin_count = 1;
        frame.is_dirty = false;

        // The frame is pinned, so it must not be an LRU candidate.
        replacer_.Pin(frame_id);

        return &frame.page;
    }

    // --------------------------------------------------
    // UnpinPage()
    // --------------------------------------------------

    bool BufferPoolManager::UnpinPage(
        page_id_t page_id,
        bool is_dirty)
    {
        // Step 1: Find the page in the buffer pool.
        auto it = page_table_.find(page_id);

        if (it == page_table_.end())
        {
            return false;
        }

        frame_id_t frame_id = it->second;
        Frame &frame = frames_[frame_id];

        // Step 2: Reject an extra unpin.
        if (frame.pin_count <= 0)
        {
            return false;
        }

        // Step 3: Remember whether the page has been modified.
        // Once dirty, it remains dirty until successfully flushed.
        frame.is_dirty = frame.is_dirty || is_dirty;

        // Step 4: Release one pin.
        --frame.pin_count;

        // Step 5: Only an unpinned frame becomes an LRU candidate.
        if (frame.pin_count == 0)
        {
            replacer_.Unpin(frame_id);
        }

        return true;
    }

    // --------------------------------------------------
    // FlushPage()
    // --------------------------------------------------

    bool BufferPoolManager::FlushPage(page_id_t page_id)
    {
        if (disk_manager_ == nullptr)
        {
            return false;
        }

        // Step 1: Find the resident page.
        auto it = page_table_.find(page_id);

        if (it == page_table_.end())
        {
            return false;
        }

        frame_id_t frame_id = it->second;
        Frame &frame = frames_[frame_id];

        // Step 2: Write the page to disk.
        if (!disk_manager_->WritePage(page_id, frame.page))
        {
            return false;
        }

        // Step 3: Disk now has the latest contents.
        frame.is_dirty = false;

        return true;
    }

    // --------------------------------------------------
    // NewPage()
    // --------------------------------------------------

    Page *BufferPoolManager::NewPage(page_id_t &page_id)
    {
        page_id = INVALID_PAGE_ID;

        if (disk_manager_ == nullptr)
        {
            return nullptr;
        }

        // Step 1: Acquire a frame before allocating a page ID.
        // This avoids allocating a page if the pool has no usable frame.
        frame_id_t frame_id;

        if (!AcquireFrame(frame_id))
        {
            return nullptr;
        }

        Frame &frame = frames_[frame_id];

        // Step 2: Ask DiskManager to allocate a new page.
        page_id_t new_page_id = disk_manager_->AllocatePage();

        if (new_page_id == INVALID_PAGE_ID)
        {
            // Allocation failed. The frame remains available.
            frame.page.ResetMemory();
            frame.page.SetPageId(INVALID_PAGE_ID);
            frame.pin_count = 0;
            frame.is_dirty = false;

            return nullptr;
        }

        // Step 3: Initialize the new page in memory.
        frame.page.ResetMemory();
        frame.page.SetPageId(new_page_id);
        frame.pin_count = 1;
        frame.is_dirty = false;

        // Step 4: Register the new page.
        page_table_[new_page_id] = frame_id;

        // Step 5: It is pinned because the caller receives it.
        replacer_.Pin(frame_id);

        page_id = new_page_id;

        return &frame.page;
    }

    // --------------------------------------------------
    // DeletePage()
    // --------------------------------------------------

    bool BufferPoolManager::DeletePage(page_id_t page_id)
    {
        // An invalid page ID is not resident, so nothing needs removing.
        if (page_id == INVALID_PAGE_ID)
        {
            return true;
        }

        // Step 1: Check whether the page is resident.
        auto it = page_table_.find(page_id);

        if (it == page_table_.end())
        {
            return true;
        }

        frame_id_t frame_id = it->second;
        Frame &frame = frames_[frame_id];

        // Step 2: A page in use cannot be removed.
        if (frame.pin_count > 0)
        {
            return false;
        }

        // Step 3: Remove the frame from LRU tracking.
        replacer_.Pin(frame_id);

        // Step 4: Remove the page-table entry.
        page_table_.erase(it);

        // Step 5: Reset the frame.
        frame.page.ResetMemory();
        frame.page.SetPageId(INVALID_PAGE_ID);
        frame.pin_count = 0;
        frame.is_dirty = false;

        return true;
    }

    // --------------------------------------------------
    // FlushAllPages()
    // --------------------------------------------------

    void BufferPoolManager::FlushAllPages()
    {
        // FlushPage() leaves a page dirty if its write fails.
        for (const auto &entry : page_table_)
        {
            FlushPage(entry.first);
        }
    }

} // namespace minidb