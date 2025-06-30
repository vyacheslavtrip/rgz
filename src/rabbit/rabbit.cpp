#include "rabbit.hpp"
#include "rabbit/utils.hpp" // вспомогательные функции из utils.cpp
#include <vector>
#include <stdexcept>

static constexpr size_t BLOCK_SIZE = 16;

void Rabbit::process_stream(std::istream& in, std::ostream& out, const std::string& key_str) {
    if (key_str.size() < 16)
        throw std::runtime_error("Key too short for Rabbit (need 16 bytes)");

    RabbitState st;
    key_setup(st, reinterpret_cast<const uint8_t*>(key_str.data()));

    std::vector<uint8_t> buf(BLOCK_SIZE);
    std::vector<uint8_t> ks(BLOCK_SIZE);

    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), BLOCK_SIZE);
        std::streamsize n = in.gcount();
        if (n <= 0) break;

        generate_block(st, ks.data());

        for (int i = 0; i < n; ++i) {
            buf[i] ^= ks[i];
        }

        out.write(reinterpret_cast<const char*>(buf.data()), n);
        if (!out) throw std::runtime_error("Write error in Rabbit");
    }
}

void Rabbit::encrypt(std::istream& in, std::ostream& out, const std::string& key) {
    process_stream(in, out, key);
}

void Rabbit::decrypt(std::istream& in, std::ostream& out, const std::string& key) {
    process_stream(in, out, key);
}
