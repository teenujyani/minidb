#ifndef MINIDB_TYPES_H
#define MINIDB_TYPES_H

#include <cstdint>

namespace minidb
{

    using page_id_t = int32_t;
    using frame_id_t = int32_t;

    constexpr page_id_t INVALID_PAGE_ID = -1;
    constexpr frame_id_t INVALID_FRAME_ID = -1;

} // namespace minidb

#endif // MINIDB_TYPES_H