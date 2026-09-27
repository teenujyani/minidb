#include <iostream>
#include <string>
#include <cstring>

#include "storage/disk_manager.h"

using namespace minidb;

int main() {

    std::cout << "MiniDB Storage Test\n";
    std::cout << "===================\n\n";

    DiskManager disk_manager("minidb.db");

    // Allocate a new page
    page_id_t page_id = disk_manager.AllocatePage();

    std::cout << "Allocated Page ID: "
              << page_id << '\n';

    // Create data
    Page page;
    page.SetPageId(page_id);

    std::string message = "Hello from MiniDB!";

    std::memcpy(
        page.GetData(),
        message.c_str(),
        message.size()
    );

    // Write page to disk
    bool write_success =
        disk_manager.WritePage(page_id, page);

    std::cout << "Write successful: "
              << (write_success ? "YES" : "NO")
              << '\n';

    // Read page back
    Page loaded_page;

    bool read_success =
        disk_manager.ReadPage(
            page_id,
            loaded_page
        );

    std::cout << "Read successful: "
              << (read_success ? "YES" : "NO")
              << '\n';

    if (read_success) {

        std::string result(
            loaded_page.GetData(),
            message.size()
        );

        std::cout << "Data read from disk: "
                  << result
                  << '\n';
    }

    std::cout << "\nNumber of pages: "
              << disk_manager.GetNumPages()
              << '\n';

    return 0;
}