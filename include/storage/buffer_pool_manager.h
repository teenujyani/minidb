#ifndef MINIDB_BUFFER_POOL_MANAGER_H
#define MINIDB_BUFFER_POOL_MANAGER_H

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "storage/disk_manager.h"
#include "storage/frame.h"
#include "storage/lru_replacer.h"

namespace minidb
{

    class BufferPoolManager
    {
    public:
        BufferPoolManager(size_t pool_size,
                          DiskManager *disk_manager);

        Page *FetchPage(page_id_t page_id);

        bool UnpinPage(page_id_t page_id,
                       bool is_dirty);

        bool FlushPage(page_id_t page_id);

        Page *NewPage(page_id_t &page_id);

        bool DeletePage(page_id_t page_id);

        void FlushAllPages();

    private:
        size_t pool_size_;

        DiskManager *disk_manager_;

        std::vector<Frame> frames_;

        std::unordered_map<page_id_t, frame_id_t> page_table_;

        LRUReplacer replacer_;
    };

} // namespace minidb

#endif // MINIDB_BUFFER_POOL_MANAGER_H