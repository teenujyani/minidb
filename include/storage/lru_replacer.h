#ifndef MINIDB_LRU_REPLACER_H
#define MINIDB_LRU_REPLACER_H

#include "storage/types.h"
#include <cstddef>
#include <list>
#include <unordered_map>

namespace minidb
{

    class LRUReplacer
    {
    public:
        /**
         * @brief Construct a new LRUReplacer tracking up to a maximum capacity of frames.
         * @param num_frames The maximum number of frames the replacer handles.
         */
        explicit LRUReplacer(size_t num_frames);

        /**
         * @brief Remove the victim frame according to the LRU policy.
         * @param[out] frame_id The frame identifier that was evicted.
         * @return true if a victim was successfully found and evicted, false otherwise.
         */
        bool Victim(frame_id_t *frame_id);

        /**
         * @brief Pin a frame, removing it from the replacer to shield it from eviction.
         * @param frame_id The ID of the frame being pinned.
         */
        void Pin(frame_id_t frame_id);

        /**
         * @brief Unpin a frame, adding it back to the replacer so it becomes eligible for eviction.
         * @param frame_id The ID of the frame being unpinned.
         */
        void Unpin(frame_id_t frame_id);

        /**
         * @brief Get the current number of frames eligible for eviction inside the replacer.
         * @return The size of the LRU tracking system.
         */
        size_t Size() const;

    private:
        // Maximum number of frames allowed in the replacement tracking pool
        size_t capacity_;

        // Doubly linked list tracking the access order (Least Recently Used at the front)
        std::list<frame_id_t> lru_list_;

        // Hash map pairing frame IDs to list positions for fast O(1) removals and updates
        std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> lru_map_;
    };

} // namespace minidb

#endif // MINIDB_LRU_REPLACER_H
