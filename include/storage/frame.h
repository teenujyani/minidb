#ifndef MINIDB_FRAME_H
#define MINIDB_FRAME_H

#include "storage/page.h"

namespace minidb
{

    struct Frame
    {
        Page page;
        int32_t pin_count = 0;
        bool is_dirty = false;
    };

} // namespace minidb

#endif // MINIDB_FRAME_H