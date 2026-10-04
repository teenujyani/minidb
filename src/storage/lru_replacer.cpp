#include "storage/lru_replacer.h"

namespace minidb
{

    LRUReplacer::LRUReplacer(size_t num_frames)
        : capacity_(num_frames)
    {
        // Constructor: Initialize the maximum frame capacity
    }

    bool LRUReplacer::Victim(frame_id_t *frame_id)
    {
        // If there are no frames tracked for eviction, return false
        if (lru_list_.empty())
        {
            return false;
        }

        // The front of the list holds the oldest (least recently used) frame
        frame_id_t victim = lru_list_.front();

        // Remove the victim frame from both the tracking list and the hash map
        lru_list_.pop_front();
        lru_map_.erase(victim);

        // Store the evicted frame ID in the output parameter
        *frame_id = victim;

        return true;
    }

    void LRUReplacer::Pin(frame_id_t frame_id)
    {
        auto it = lru_map_.find(frame_id);

        // If the frame is not in the replacer, it is already pinned; do nothing
        if (it == lru_map_.end())
        {
            return;
        }

        // Remove the frame from the list using its iterator, then clear its map entry
        lru_list_.erase(it->second);
        lru_map_.erase(it);
    }

    void LRUReplacer::Unpin(frame_id_t frame_id)
    {
        // If the frame is already in the replacer, do not add it again
        if (lru_map_.find(frame_id) != lru_map_.end())
        {
            return;
        }

        // Add the frame to the back of the list (making it the most recently unpinned)
        lru_list_.push_back(frame_id);

        // Get the iterator pointing to this newly added frame
        auto it = lru_list_.end();
        --it;

        // Save the frame's position inside the hash map for O(1) lookups
        lru_map_[frame_id] = it;
    }

    size_t LRUReplacer::Size() const
    {
        // Return the number of frames currently eligible for replacement
        return lru_list_.size();
    }

} // namespace minidb
