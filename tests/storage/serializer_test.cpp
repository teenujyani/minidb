#include "storage/serializer.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace minidb;

int main()
{
    // Buffer used for serialization
    char buffer[1024];

    size_t offset = 0;

    // Original data
    int32_t original_id = 101;
    double original_price = 25.5;
    std::string original_name = "Teenu";

    // SERIALIZATION

    Serializer::WriteInt32(buffer, offset, original_id);

    Serializer::WriteDouble(buffer, offset, original_price);

    Serializer::WriteString(buffer, offset, original_name);

    // Save the total serialized size
    size_t serialized_size = offset;

    std::cout << "Serialized size: " << serialized_size << " bytes\n";

    // DESERIALIZATION

    offset = 0;

    int32_t recovered_id = Serializer::ReadInt32(buffer, offset);

    double recovered_price = Serializer::ReadDouble(buffer, offset);

    std::string recovered_name = Serializer::ReadString(buffer, offset);

    // VERIFICATION

    assert(recovered_id == original_id);

    assert(recovered_price == original_price);

    assert(recovered_name == original_name);

    std::cout << "Basic serialization test PASSED!\n";

    return 0;
}