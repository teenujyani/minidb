#ifndef MINIDB_BUFFER_POOL_MANAGER_H
#define MINIDB_BUFFER_POOL_MANAGER_H

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "storage/disk_manager.h"
#include "storage/frame.h"
#include "storage/lru_replacer.h"
#include "storage/types.h"

namespace minidb
{

    class BufferPoolManager
    {
    public:
        BufferPoolManager(size_t pool_size, DiskManager *disk_manager);

        // Fetch an existing page from the buffer pool or disk.
        Page *FetchPage(page_id_t page_id);

        // Unpin a page after use and optionally mark it dirty.
        bool UnpinPage(page_id_t page_id, bool is_dirty);

        // Write a resident page to disk.
        bool FlushPage(page_id_t page_id);

        // Allocate a new page and place it in the buffer pool.
        Page *NewPage(page_id_t &page_id);

        // Remove a page from the buffer pool if it is not pinned.
        bool DeletePage(page_id_t page_id);

        // Flush all resident pages to disk.
        void FlushAllPages();

    private:
        // Find an unused frame or evict an eligible frame.
        bool AcquireFrame(frame_id_t &frame_id);

        size_t pool_size_;

        DiskManager *disk_manager_;

        std::vector<Frame> frames_;

        // Maps database page IDs to RAM frame IDs.
        std::unordered_map<page_id_t, frame_id_t> page_table_;

        LRUReplacer replacer_;
    };

} // namespace minidb

#endif // MINIDB_BUFFER_POOL_MANAGER_H