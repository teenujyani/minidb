#include "storage/buffer_pool_manager.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace minidb;

int main()
{
    const char *db_file = "buffer_pool_test.db";
    std::remove(db_file);

    {
        // A pool with only one frame makes eviction easy to test.
        DiskManager disk_manager(db_file);
        BufferPoolManager bpm(1, &disk_manager);

        page_id_t page1_id = INVALID_PAGE_ID;
        page_id_t page2_id = INVALID_PAGE_ID;
        page_id_t page3_id = INVALID_PAGE_ID;

        // ------------------------------------------
        // Test 1: Allocate a new page.
        // ------------------------------------------

        Page *page1 = bpm.NewPage(page1_id);

        assert(page1 != nullptr);
        assert(page1_id != INVALID_PAGE_ID);
        assert(page1->GetPageId() == page1_id);

        std::cout << "Test 1 passed: NewPage\n";

        // ------------------------------------------
        // Test 2: Modify and unpin a dirty page.
        // ------------------------------------------

        const char *message = "MiniDB dirty page test";

        std::memcpy(
            page1->GetData(),
            message,
            std::strlen(message) + 1
        );

        assert(bpm.UnpinPage(page1_id, true));

        std::cout << "Test 2 passed: UnpinPage and dirty marking\n";

        // ------------------------------------------
        // Test 3: Allocate another page.
        // The first page must be evicted.
        // Because it is dirty, it must be written
        // to disk before eviction.
        // ------------------------------------------

        Page *page2 = bpm.NewPage(page2_id);

        assert(page2 != nullptr);
        assert(page2_id != INVALID_PAGE_ID);
        assert(page2_id != page1_id);

        std::cout << "Test 3 passed: page allocation and eviction\n";

        // ------------------------------------------
        // Test 4: All frames are pinned.
        // Allocation should fail rather than
        // evicting page2.
        // ------------------------------------------

        Page *page3 = bpm.NewPage(page3_id);

        assert(page3 == nullptr);
        assert(page3_id == INVALID_PAGE_ID);

        std::cout << "Test 4 passed: pinned frame is not evicted\n";

        // Release page2 so the frame can be reused.
        assert(bpm.UnpinPage(page2_id, false));

        // ------------------------------------------
        // Test 5: Fetch page1 again.
        // This forces a disk read after eviction.
        // ------------------------------------------

        Page *recovered_page = bpm.FetchPage(page1_id);

        assert(recovered_page != nullptr);
        assert(recovered_page->GetPageId() == page1_id);

        assert(
            std::strcmp(
                recovered_page->GetData(),
                message
            ) == 0
        );

        std::cout << "Test 5 passed: dirty page survives eviction\n";

        // ------------------------------------------
        // Test 6: Flushing a resident page.
        // ------------------------------------------

        assert(bpm.FlushPage(page1_id));

        std::cout << "Test 6 passed: FlushPage\n";

        // ------------------------------------------
        // Test 7: A pinned page cannot be deleted.
        // ------------------------------------------

        assert(!bpm.DeletePage(page1_id));

        std::cout << "Test 7 passed: pinned page cannot be deleted\n";

        // Release the page and delete it from the buffer pool.
        assert(bpm.UnpinPage(page1_id, false));
        assert(bpm.DeletePage(page1_id));

        std::cout << "Test 8 passed: DeletePage\n";

        // ------------------------------------------
        // Test 9: A new page can reuse the freed frame.
        // ------------------------------------------

        Page *page3 = bpm.NewPage(page3_id);

        assert(page3 != nullptr);
        assert(page3_id != INVALID_PAGE_ID);

        std::cout << "Test 9 passed: deleted frame can be reused\n";

        // ------------------------------------------
        // Test 10: FlushAllPages.
        // ------------------------------------------

        assert(bpm.UnpinPage(page3_id, true));

        bpm.FlushAllPages();

        // A final fetch confirms that the page remains accessible.
        Page *final_page = bpm.FetchPage(page3_id);

        assert(final_page != nullptr);

        assert(bpm.UnpinPage(page3_id, false));

        std::cout << "Test 10 passed: FlushAllPages\n";

        std::cout << "\nAll BufferPoolManager tests PASSED!\n";
    }

    // DiskManager has been destroyed, so the file can be removed.
    std::remove(db_file);

    return 0;
}