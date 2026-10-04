#include "storage/buffer_pool_manager.h"

namespace minidb
{

    BufferPoolManager::BufferPoolManager(size_t pool_size,DiskManager *disk_manager): 
        pool_size_(pool_size),
        disk_manager_(disk_manager),
        frames_(pool_size),
        replacer_(pool_size)
    {
    }

} // namespace minidb