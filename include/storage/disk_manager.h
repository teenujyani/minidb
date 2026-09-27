#ifndef MINIDB_DISK_MANAGER_H
#define MINIDB_DISK_MANAGER_H

#include <fstream>
#include <string>

#include "storage/page.h"

namespace minidb
{
    class DiskManager
    {
    public:
        explicit DiskManager(const std::string &db_file);
        ~DiskManager();

        // Read a page from disk
        bool ReadPage(page_id_t page_id, Page &page);

        // Write a page to disk
        bool WritePage(page_id_t page_id, const Page &page);

        // Allocate a new page
        page_id_t AllocatePage();

        // Number of pages currently stored
        std::size_t GetNumPages();

    private:
        std::fstream db_file_;
        std::string db_file_name_;
    };

} // namespace minidb

#endif