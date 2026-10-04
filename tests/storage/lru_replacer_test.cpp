#include "storage/lru_replacer.h"

#include <cassert>
#include <iostream>

using namespace minidb;

int main()
{
    LRUReplacer replacer(3);

    // Initially, no frames are available for replacement.
    assert(replacer.Size() == 0);

    std::cout << "Test 1 passed: empty replacer\n";

    // Add frames to the replacer.
    replacer.Unpin(0);
    replacer.Unpin(1);
    replacer.Unpin(2);

    assert(replacer.Size() == 3);

    std::cout << "Test 2 passed: Unpin adds frames\n";

    // Frame 0 was added first, so it is the least recently used.
    frame_id_t victim;

    bool found = replacer.Victim(&victim);

    assert(found);
    assert(victim == 0);
    assert(replacer.Size() == 2);

    std::cout << "Test 3 passed: Victim selects LRU frame\n";

    // Pin frame 1.
    replacer.Pin(1);

    assert(replacer.Size() == 1);

    std::cout << "Test 4 passed: Pin removes frame\n";

    // Only frame 2 should now be available.
    found = replacer.Victim(&victim);

    assert(found);
    assert(victim == 2);

    std::cout << "Test 5 passed: pinned frame is not selected\n";

    // No replacement candidates remain.
    found = replacer.Victim(&victim);

    assert(!found);

    std::cout << "Test 6 passed: empty replacer returns no victim\n";

    // Unpin frame 1 again.
    replacer.Unpin(1);

    assert(replacer.Size() == 1);

    found = replacer.Victim(&victim);

    assert(found);
    assert(victim == 1);

    std::cout << "Test 7 passed: Unpin makes frame replaceable again\n";

    std::cout << "\nAll LRU tests PASSED!\n";

    return 0;
}