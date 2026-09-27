#include "storage/disk_manager.h"

#include <filesystem>
#include <iostream>

namespace minidb
{
    DiskManager::DiskManager(const std::string &db_file) : db_file_name_(db_file)
    {
        // Open existing database file
        db_file_.open(db_file_name_, std::ios::in | std::ios::out | std::ios::binary);
        // If file doesn't exist, create it
        if (!db_file_.is_open())
        {
            std::ofstream create_file(db_file_name_, std::ios::binary);

            create_file.close();
            db_file_.open(db_file_name_, std::ios::in | std::ios::out | std::ios::binary);
        }
        if (!db_file_.is_open())
        {
            throw std::runtime_error("Failed to open database file");
        }
    }
    DiskManager::~DiskManager()
    {
        if (db_file_.is_open())
        {
            db_file_.close();
        }
    }
    bool DiskManager::ReadPage(page_id_t page_id, Page &page)
    {
        if (page_id < 0)
        {
            return false;
        }
        std::size_t offset =static_cast<std::size_t>(page_id) * PAGE_SIZE;
        // Move read pointer
        db_file_.seekg(offset);
        // Check whether page exists
        if (db_file_.fail())
        {
            db_file_.clear();
            return false;
        }
        db_file_.read(page.GetData(), PAGE_SIZE);
        std::streamsize bytes_read = db_file_.gcount();
        if (bytes_read != PAGE_SIZE)
        {
            db_file_.clear();
            return false;
        }
        page.SetPageId(page_id);
        return true;
    }
    bool DiskManager::WritePage(
        page_id_t page_id,
        const Page &page)
    {
        if (page_id < 0)
        {
            return false;
        }
        std::size_t offset = static_cast<std::size_t>(page_id) * PAGE_SIZE;

        // Move write pointer
        db_file_.seekp(offset);

        if (db_file_.fail())
        {
            db_file_.clear();
            return false;
        }
        db_file_.write(page.GetData(), PAGE_SIZE);
        db_file_.flush();
        return !db_file_.fail();
    }

    page_id_t DiskManager::AllocatePage()
    {
        page_id_t new_page_id =
            static_cast<page_id_t>(GetNumPages());
        Page empty_page;
        empty_page.SetPageId(new_page_id);
        if (!WritePage(new_page_id, empty_page))
        {
            return INVALID_PAGE_ID;
        }
        return new_page_id;
    }
    std::size_t DiskManager::GetNumPages()
    {
        db_file_.seekg(0, std::ios::end);
        std::streampos file_size = db_file_.tellg();
        db_file_.clear();
        if (file_size <= 0)
        {
            return 0;
        }
        return static_cast<std::size_t>(
            file_size / PAGE_SIZE);
    }
} // namespace minidb