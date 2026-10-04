#ifndef MINIDB_PAGE_H
#define MINIDB_PAGE_H

#include <cstring>

#include "storage/types.h"

namespace minidb
{
    constexpr std::size_t PAGE_SIZE = 4096;
    class Page
    {
    public:
        Page();

        // Returns pointer to page data
        char *GetData();

        // Returns const pointer to page data
        const char *GetData() const;

        // Returns page ID
        page_id_t GetPageId() const;

        // Sets page ID
        void SetPageId(page_id_t page_id);

        // Clears page contents
        void ResetMemory();

    private:
        page_id_t page_id_;

        char data_[PAGE_SIZE];
    };

} // namespace minidb

#endif