#include "xtea.hpp"
#include "utils.hpp"
#include <cstring>
#include <vector>

namespace {

constexpr int NUM_ROUNDS = 32;
constexpr uint32_t DELTA = 0x9E3779B9;

void encryptBlock(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;
    for (int i = 0; i < NUM_ROUNDS; ++i) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        sum += DELTA;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

void decryptBlock(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = DELTA * NUM_ROUNDS;
    for (int i = 0; i < NUM_ROUNDS; ++i) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
        sum -= DELTA;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

void prepareKey(const std::string& keyStr, uint32_t key[4]) {
    std::memcpy(key, keyStr.data(), 16);
}

} // namespace anonymous

void XTEA::encrypt(std::istream& in, std::ostream& out, const std::string& keyStr) {
    uint32_t key[4];
    prepareKey(keyStr, key);

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(in)), {});
    xtea_utils::pad(buffer, 8);

    for (size_t i = 0; i < buffer.size(); i += 8) {
        uint32_t v[2];
        std::memcpy(v, &buffer[i], 8);
        encryptBlock(v, key);
        std::memcpy(&buffer[i], v, 8);
    }

    out.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}

void XTEA::decrypt(std::istream& in, std::ostream& out, const std::string& keyStr) {
    uint32_t key[4];
    prepareKey(keyStr, key);

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(in)), {});

    for (size_t i = 0; i < buffer.size(); i += 8) {
        uint32_t v[2];
        std::memcpy(v, &buffer[i], 8);
        decryptBlock(v, key);
        std::memcpy(&buffer[i], v, 8);
    }

    xtea_utils::unpad(buffer);

    out.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}