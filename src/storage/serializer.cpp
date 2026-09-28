#include "storage/serializer.h"
#include <cstring>
#include <stdexcept>

namespace minidb
{
    // --- WRITE ---

    void Serializer::WriteInt32(char *buffer, size_t &offset, int32_t value)
    {
        std::memcpy(buffer + offset, &value, sizeof(int32_t));
        offset += sizeof(int32_t);
    }

    void Serializer::WriteDouble(char *buffer, size_t &offset, double value)
    {
        std::memcpy(buffer + offset, &value, sizeof(double));
        offset += sizeof(double);
    }

    void Serializer::WriteString(char *buffer, size_t &offset, const std::string &value)
    {
        int32_t length = static_cast<int32_t>(value.size());
        WriteInt32(buffer, offset, length); // Write 4-byte string length first

        if (length > 0)
        {
            std::memcpy(buffer + offset, value.data(), length);
            offset += length;
        }
    }

    // --- READ ---

    int32_t Serializer::ReadInt32(const char *buffer, size_t &offset)
    {
        int32_t value;
        std::memcpy(&value, buffer + offset, sizeof(int32_t));
        offset += sizeof(int32_t);
        return value;
    }

    double Serializer::ReadDouble(const char *buffer, size_t &offset)
    {
        double value;
        std::memcpy(&value, buffer + offset, sizeof(double));
        offset += sizeof(double);
        return value;
    }

    std::string Serializer::ReadString(const char *buffer, size_t &offset)
    {
        int32_t length = ReadInt32(buffer, offset);
        if (length < 0)
        {
            throw std::runtime_error("Invalid string length during deserialization");
        }
        if (length == 0)
        {
            return "";
        }

        std::string value(buffer + offset, length);
        offset += length;
        return value;
    }

} // namespace minidb