#ifndef MINIDB_SERIALIZER_H
#define MINIDB_SERIALIZER_H

#include <cstdint>
#include <cstddef>
#include <string>

namespace minidb
{

    class Serializer
    {
    public:
        // Write operations
        static void WriteInt32(char *buffer, size_t &offset, int32_t value);
        static void WriteDouble(char *buffer, size_t &offset, double value);
        static void WriteString(char *buffer, size_t &offset, const std::string &value);

        // Read operations
        static int32_t ReadInt32(const char *buffer, size_t &offset);
        static double ReadDouble(const char *buffer, size_t &offset);
        static std::string ReadString(const char *buffer, size_t &offset);
    };

} // namespace minidb

#endif // MINIDB_SERIALIZER_H