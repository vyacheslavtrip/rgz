#pragma once
#include "utils.hpp"
#include "ICipher.hpp"
#include <string>

class XTEA : public ICipher {
public:
    void encrypt(std::istream& in, std::ostream& out, const std::string& key) override;
    void decrypt(std::istream& in, std::ostream& out, const std::string& key) override;
};