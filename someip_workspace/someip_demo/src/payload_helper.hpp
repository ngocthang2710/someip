#ifndef PAYLOAD_HELPER_HPP
#define PAYLOAD_HELPER_HPP

#include <vsomeip/vsomeip.hpp>
#include <vector>

// Hàm đóng gói số nguyên 32-bit thành mảng byte (Big Endian)
void serialize_uint32(std::vector<vsomeip::byte_t>& data, uint32_t value) {
    data.push_back((value >> 24) & 0xFF);
    data.push_back((value >> 16) & 0xFF);
    data.push_back((value >> 8) & 0xFF);
    data.push_back(value & 0xFF);
}

// Hàm giải nén mảng byte thành số nguyên 32-bit
uint32_t deserialize_uint32(const vsomeip::byte_t* data, size_t offset) {
    return ((uint32_t)data[offset] << 24) |
           ((uint32_t)data[offset + 1] << 16) |
           ((uint32_t)data[offset + 2] << 8) |
           ((uint32_t)data[offset + 3]);
}

#endif