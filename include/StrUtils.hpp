#pragma once

#include <iomanip>
#include <boost/algorithm/string.hpp>

namespace gdb_stub {

namespace str {

inline std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    boost::split(tokens, str, boost::is_any_of(std::string(1, delimiter)));
    return tokens;
}

inline std::string toHexStr(const std::vector<uint8_t> &bytes) {
    std::stringstream hexStr;
    for (auto b : bytes) {
        hexStr << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(b);
    }
    return hexStr.str();
}

template<typename IntT>
inline IntT hexToInt(const std::string &hexStr) {
    int val;
    std::stringstream ss;
    ss << std::hex << hexStr;
    ss >> val;
    return static_cast<IntT>(val);
}

template<typename IntT>
std::string intToHex(IntT i) {
    std::stringstream ss;
    ss << std::setfill ('0') << std::setw(sizeof(IntT)*2) << std::hex << i;
    return ss.str();
}



}

}