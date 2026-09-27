#include "storage/page.h"

namespace minidb
{

    Page::Page() : page_id_(INVALID_PAGE_ID)
    {
        ResetMemory();
    }

    char *Page::GetData()
    {
        return data_;
    }

    const char *Page::GetData() const
    {
        return data_;
    }

    page_id_t Page::GetPageId() const
    {
        return page_id_;
    }

    void Page::SetPageId(page_id_t page_id)
    {
        page_id_ = page_id;
    }

    void Page::ResetMemory()
    {
        std::memset(data_, 0, PAGE_SIZE);
    }

} // namespace minidb