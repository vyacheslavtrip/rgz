#pragma once

#include "ICipher.hpp"
#include <string>

class ChaCha20 : public ICipher {
public:
    ChaCha20() = default;
    virtual ~ChaCha20() = default;

    void encrypt(std::istream& in, std::ostream& out, const std::string& key) override;
    void decrypt(std::istream& in, std::ostream& out, const std::string& key) override;

private:
    void process_stream(std::istream& in, std::ostream& out, const std::string& key);
};