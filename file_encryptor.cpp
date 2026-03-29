#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

// ─────────────────────────────────────────────
//  Cipher implementations
// ─────────────────────────────────────────────

void xor_cipher(std::vector<char>& data, const std::string& key) {
    if (key.empty()) throw std::invalid_argument("XOR key cannot be empty.");
    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= key[i % key.size()];
}

void caesar_cipher(std::vector<char>& data, int shift, bool decrypt) {
    shift = ((shift % 256) + 256) % 256;
    if (decrypt) shift = 256 - shift;
    for (char& c : data)
        c = static_cast<char>((static_cast<unsigned char>(c) + shift) % 256);
}

// ─────────────────────────────────────────────
//  File I/O helpers
// ─────────────────────────────────────────────

std::vector<char> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open input file: " + path);
    return { std::istreambuf_iterator<char>(file), {} };
}

void write_file(const std::string& path, const std::vector<char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open output file: " + path);
    file.write(data.data(), data.size());
}

// ─────────────────────────────────────────────
//  CLI helpers
// ─────────────────────────────────────────────

void print_usage(const char* prog) {
    std::cout << "\nUsage:\n"
              << "  " << prog << " xor    <encrypt|decrypt> <input> <output> <key>\n"
              << "  " << prog << " caesar <encrypt|decrypt> <input> <output> <shift>\n\n"
              << "Examples:\n"
              << "  " << prog << " xor    encrypt secret.txt secret.enc mypassword\n"
              << "  " << prog << " xor    decrypt secret.enc restored.txt mypassword\n"
              << "  " << prog << " caesar encrypt hello.txt hello.enc 13\n"
              << "  " << prog << " caesar decrypt hello.enc hello.txt 13\n\n";
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────

int main(int argc, char* argv[]) {
    if (argc < 6) {
        print_usage(argv[0]);
        return 1;
    }

    std::string cipher  = argv[1];  // xor | caesar
    std::string mode    = argv[2];  // encrypt | decrypt
    std::string infile  = argv[3];
    std::string outfile = argv[4];
    std::string keyarg  = argv[5];

    if (cipher != "xor" && cipher != "caesar") {
        std::cerr << "Error: cipher must be 'xor' or 'caesar'.\n";
        return 1;
    }
    if (mode != "encrypt" && mode != "decrypt") {
        std::cerr << "Error: mode must be 'encrypt' or 'decrypt'.\n";
        return 1;
    }

    try {
        auto data = read_file(infile);
        auto size = data.size();

        if (cipher == "xor") {
            xor_cipher(data, keyarg);
        } else {
            int shift = std::stoi(keyarg);
            caesar_cipher(data, shift, mode == "decrypt");
        }

        write_file(outfile, data);

        std::cout << "✓ " << (mode == "encrypt" ? "Encrypted" : "Decrypted")
                  << " " << size << " bytes\n"
                  << "  Input : " << infile  << "\n"
                  << "  Output: " << outfile << "\n"
                  << "  Cipher: " << cipher  << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
