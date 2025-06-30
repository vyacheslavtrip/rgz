#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <dlfcn.h>

#include "ICipher.hpp"
#include "generate_key.hpp"

enum class Mode { Unknown, GenerateKey, Encrypt, Decrypt };
enum class Cipher { Unknown, XTEA, ChaCha20, Rabbit };

void print_usage() {
    std::cout << "Usage:\n"
              << "  ./prog -k -xtea key.bin         # Generate XTEA key (16 bytes)\n"
              << "  ./prog -k -cc20 key.bin         # Generate ChaCha20 key (32 bytes)\n"
              << "  ./prog -k -rbbit key.bin        # Generate Rabbit key (16 bytes)\n"
              << "  ./prog -e -xtea input_file key_file  # Encrypt with XTEA\n"
              << "  ./prog -e -cc20 input_file key_file  # Encrypt with ChaCha20\n"
              << "  ./prog -e -rbbit input_file key_file # Encrypt with Rabbit\n"
              << "  ./prog -d -xtea input_file key_file  # Decrypt with XTEA\n"
              << "  ./prog -d -cc20 input_file key_file  # Decrypt with ChaCha20\n"
              << "  ./prog -d -rbbit input_file key_file # Decrypt with Rabbit\n";
}

Mode parse_mode(const std::string& arg) {
    if (arg == "-k") return Mode::GenerateKey;
    if (arg == "-e") return Mode::Encrypt;
    if (arg == "-d") return Mode::Decrypt;
    return Mode::Unknown;
}

Cipher parse_cipher(const std::string& arg) {
    if (arg == "-xtea") return Cipher::XTEA;
    if (arg == "-cc20") return Cipher::ChaCha20;
    if (arg == "-rbbit") return Cipher::Rabbit;
    return Cipher::Unknown;
}

bool read_key_from_file(const std::string& filename, std::string& key_out) {
    std::ifstream key_file(filename, std::ios::binary);
    if (!key_file) {
        std::cerr << "Failed to open key file: " << filename << "\n";
        return false;
    }
    std::vector<char> key_data((std::istreambuf_iterator<char>(key_file)), std::istreambuf_iterator<char>());
    key_out.assign(key_data.begin(), key_data.end());
    return true;
}

bool write_key_to_file(const std::string& filename, const std::vector<uint8_t>& key) {
    std::ofstream key_file(filename, std::ios::binary);
    if (!key_file) {
        std::cerr << "Failed to open file for writing key: " << filename << "\n";
        return false;
    }
    key_file.write(reinterpret_cast<const char*>(key.data()), key.size());
    return true;
}

std::string remove_extension(const std::string& filename) {
    size_t last_dot = filename.find_last_of('.');
    if (last_dot == std::string::npos) return filename;
    return filename.substr(0, last_dot);
}

std::string make_decrypted_filename(const std::string& filename) {
    std::filesystem::path p(filename);

    if (p.extension() == ".enc") {
        std::filesystem::path base = p.stem();
        std::string base_stem = base.stem().string();
        std::string base_ext = base.extension().string();

        return base_stem + "_dec" + base_ext;
    } else {
        std::string stem = p.stem().string();
        std::string ext = p.extension().string();
        return stem + "_dec" + ext;
    }
}

bool generate_key_cmd(const std::string& cipher_arg, const std::string& key_filename) {
    Cipher cipher = parse_cipher(cipher_arg);
    if (cipher == Cipher::Unknown) {
        std::cerr << "Unknown cipher for key generation: " << cipher_arg << "\n";
        print_usage();
        return false;
    }

    size_t key_len = 0;
    switch (cipher) {
        case Cipher::XTEA:
            key_len = 16;
            break;
        case Cipher::ChaCha20:
            key_len = 32;
            break;
        case Cipher::Rabbit:
            key_len = 16;
            break;
        default:
            std::cerr << "Unsupported cipher for key generation\n";
            return false;
    }

    std::string password;
    std::cout << "Enter password to generate key: ";
    std::getline(std::cin, password);

    auto key = generate_key(password, key_len);

    std::filesystem::path output_dir("output");
    if (!std::filesystem::exists(output_dir)) {
        if (!std::filesystem::create_directory(output_dir)) {
            std::cerr << "Failed to create output directory\n";
            return false;
        }
    }

    std::filesystem::path key_path = output_dir / key_filename;

    if (!write_key_to_file(key_path.string(), key)) return false;

    std::cout << "Key generated and saved to " << key_path.string() << "\n";
    return true;
}

bool process_encrypt_decrypt(Mode mode, const std::string& cipher_arg,
                             const std::string& input_filename, const std::string& key_filename,
                             ICipher*& cipher_obj, void*& lib_handle) {
    Cipher cipher = parse_cipher(cipher_arg);
    if (cipher == Cipher::Unknown) {
        std::cerr << "Unknown cipher: " << cipher_arg << "\n";
        print_usage();
        return false;
    }

    std::string cipher_lib;
    switch (cipher) {
        case Cipher::XTEA:
            cipher_lib = "./lib/libxtea.so";
            break;
        case Cipher::ChaCha20:
            cipher_lib = "./lib/libchacha20.so";
            break;
        case Cipher::Rabbit:
            cipher_lib = "./lib/librabbit.so";
            break;
        default:
            std::cerr << "Unsupported cipher\n";
            return false;
    }

    std::filesystem::path output_dir("output");
    if (!std::filesystem::exists(output_dir)) {
        if (!std::filesystem::create_directory(output_dir)) {
            std::cerr << "Failed to create output directory\n";
            return false;
        }
    }

    lib_handle = dlopen(cipher_lib.c_str(), RTLD_NOW);
    if (!lib_handle) {
        std::cerr << "Failed to load cipher library: " << dlerror() << "\n";
        return false;
    }

    auto create_cipher = (ICipher*(*)()) dlsym(lib_handle, "create_cipher");
    auto destroy_cipher = (void(*)(ICipher*)) dlsym(lib_handle, "destroy_cipher");
    if (!create_cipher || !destroy_cipher) {
        std::cerr << "Failed to load symbols from library\n";
        dlclose(lib_handle);
        return false;
    }

    cipher_obj = create_cipher();

    std::string key_str;
    if (!read_key_from_file(key_filename, key_str)) {
        destroy_cipher(cipher_obj);
        dlclose(lib_handle);
        return false;
    }

    std::ifstream input_file(input_filename, std::ios::binary);
    if (!input_file) {
        std::cerr << "Failed to open input file: " << input_filename << "\n";
        destroy_cipher(cipher_obj);
        dlclose(lib_handle);
        return false;
    }

    std::string output_filename;
    std::filesystem::path in_path(input_filename);
    if (mode == Mode::Encrypt) {
        output_filename = (output_dir / (in_path.filename().string() + ".enc")).string();
    } else {
        output_filename = (output_dir / make_decrypted_filename(input_filename)).string();
    }

    std::ofstream output_file(output_filename, std::ios::binary);
    if (!output_file) {
        std::cerr << "Failed to create output file: " << output_filename << "\n";
        destroy_cipher(cipher_obj);
        dlclose(lib_handle);
        return false;
    }

    if (mode == Mode::Encrypt) {
        cipher_obj->encrypt(input_file, output_file, key_str);
        std::cout << "Encrypted to " << output_filename << "\n";
    } else {
        cipher_obj->decrypt(input_file, output_file, key_str);
        std::cout << "Decrypted to " << output_filename << "\n";
    }

    return true;
}

int run(int argc, char* argv[], ICipher*& cipher_obj, void*& lib_handle) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    Mode mode = parse_mode(argv[1]);
    if (mode == Mode::Unknown) {
        std::cerr << "Unknown mode: " << argv[1] << "\n";
        print_usage();
        return 1;
    }

    switch (mode) {
        case Mode::GenerateKey:
            if (argc != 4) {
                std::cerr << "Invalid arguments for key generation\n";
                print_usage();
                return 1;
            }
            if (!generate_key_cmd(argv[2], argv[3]))
                return 1;
            break;

        case Mode::Encrypt:
        case Mode::Decrypt:
            if (argc != 5) {
                std::cerr << "Invalid number of arguments for encrypt/decrypt\n";
                print_usage();
                return 1;
            }
            if (!process_encrypt_decrypt(mode, argv[2], argv[3], argv[4], cipher_obj, lib_handle))
                return 1;
            break;

        default:
            std::cerr << "Invalid mode\n";
            print_usage();
            return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    ICipher* cipher_obj = nullptr;
    void* lib_handle = nullptr;

    int ret = run(argc, argv, cipher_obj, lib_handle);

    if (cipher_obj) {
        auto destroy_cipher = (void(*)(ICipher*)) dlsym(lib_handle, "destroy_cipher");
        if (destroy_cipher) {
            destroy_cipher(cipher_obj);
        }
    }
    if (lib_handle) {
        dlclose(lib_handle);
    }

    return ret;
}