// utils.cpp Паддинг — PKCS#7-like

#include "utils.hpp"
#include <stdexcept>

namespace xtea_utils {

void pad(std::vector<uint8_t>& data, size_t blockSize) {
    size_t padLen = blockSize - (data.size() % blockSize);
    data.insert(data.end(), padLen, static_cast<uint8_t>(padLen));
}

void unpad(std::vector<uint8_t>& data) {
    if (data.empty()) throw std::runtime_error("Пустой буфер для удаления паддинга");
    uint8_t padLen = data.back();
    if (padLen == 0 || padLen > data.size())
        throw std::runtime_error("Некорректный паддинг");
    data.resize(data.size() - padLen);
}

}