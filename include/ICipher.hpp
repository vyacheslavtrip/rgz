#pragma once
#include <string>
#include <iostream>

class ICipher {
public:
    virtual ~ICipher() = default;

    virtual void encrypt(std::istream& in, std::ostream& out, const std::string& key) = 0;
    virtual void decrypt(std::istream& in, std::ostream& out, const std::string& key) = 0;
};