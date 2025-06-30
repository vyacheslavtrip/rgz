#include "chacha20/chacha20.hpp"
#include "chacha20/utils.hpp"

#include <vector>
#include <cstring>
#include <iostream>

static constexpr size_t BLOCK_SIZE = 64;  // 512 bits per block

static const char* constants = "expand 32-byte k";

static void setup_state(uint32_t state[16], const std::vector<uint8_t>& key, uint32_t counter, const std::vector<uint8_t>& nonce) {
    for (int i = 0; i < 4; ++i) {
        state[i] = ((uint32_t)constants[4*i]) |
                   ((uint32_t)constants[4*i + 1] << 8) |
                   ((uint32_t)constants[4*i + 2] << 16) |
                   ((uint32_t)constants[4*i + 3] << 24);
    }

    // Key: 32 bytes (256 bits)
    for (int i = 0; i < 8; ++i) {
        state[4 + i] = key[4*i] | (key[4*i + 1] << 8) | (key[4*i + 2] << 16) | (key[4*i + 3] << 24);
    }

    state[12] = counter;

    // Nonce: 12 bytes
    for (int i = 0; i < 3; ++i) {
        state[13 + i] = nonce[4*i] | (nonce[4*i + 1] << 8) | (nonce[4*i + 2] << 16) | (nonce[4*i + 3] << 24);
    }
}

void ChaCha20::process_stream(std::istream& in, std::ostream& out, const std::string& key_str) {
    if (key_str.size() < 32) {
        throw std::runtime_error("Key too short for ChaCha20 (need 32 bytes)");
    }

    std::vector<uint8_t> key(key_str.begin(), key_str.begin() + 32);

    std::vector<uint8_t> nonce(12, 0);

    uint32_t counter = 0;
    std::vector<uint8_t> buffer(BLOCK_SIZE);

    while (true) {
        in.read(reinterpret_cast<char*>(buffer.data()), BLOCK_SIZE);
        std::streamsize read_bytes = in.gcount();
        if (read_bytes == 0) break;

        uint32_t state[16];
        uint32_t keystream[16];
        setup_state(state, key, counter++, nonce);
        chacha20_utils::chacha20_block(keystream, state);

        std::vector<uint8_t> keystream_bytes(BLOCK_SIZE);
        for (size_t i = 0; i < 16; ++i) {
            keystream_bytes[4*i] = keystream[i] & 0xFF;
            keystream_bytes[4*i + 1] = (keystream[i] >> 8) & 0xFF;
            keystream_bytes[4*i + 2] = (keystream[i] >> 16) & 0xFF;
            keystream_bytes[4*i + 3] = (keystream[i] >> 24) & 0xFF;
        }

        // If last block, resize buffers accordingly
        if (static_cast<size_t>(read_bytes) < BLOCK_SIZE) {
            buffer.resize(read_bytes);
            keystream_bytes.resize(read_bytes);
        }

        chacha20_utils::xor_data(buffer, keystream_bytes);

        out.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        if (!out) throw std::runtime_error("Failed to write output");
    }
}

void ChaCha20::encrypt(std::istream& in, std::ostream& out, const std::string& key) {
    process_stream(in, out, key);
}

void ChaCha20::decrypt(std::istream& in, std::ostream& out, const std::string& key) {
    // Symmetric cipher — same as encryption
    process_stream(in, out, key);
}
