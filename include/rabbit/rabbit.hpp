#pragma once

#include "ICipher.hpp"
#include <string>
#include <istream>
#include <ostream>

class Rabbit : public ICipher {
public:
    Rabbit() = default;
    virtual ~Rabbit() = default;

    void encrypt(std::istream& in, std::ostream& out, const std::string& key) override;
    void decrypt(std::istream& in, std::ostream& out, const std::string& key) override;

private:
    void process_stream(std::istream& in, std::ostream& out, const std::string& key);
};