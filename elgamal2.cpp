#include "../algorithm_interface.h"
#include "elgamal_string.h"

static vector<unsigned char> elgamalEncryptData(const vector<unsigned char>& data, unsigned char key) {
    long long p, g, y, x;
    generateParams(key, p, g, y, x);
    
    vector<unsigned char> result;
    srand(time(nullptr));
    
    for (size_t i = 0; i < data.size(); ++i) {
        long long m = data[i];
        long long k = (rand() % (p - 2)) + 1;
        long long a = modPow(g, k, p);
        long long yk = modPow(y, k, p);
        long long b = (yk * m) % p;
        
        for (int j = 0; j < 4; ++j) {
            result.push_back((a >> (j * 8)) & 0xFF);
        }
        for (int j = 0; j < 4; ++j) {
            result.push_back((b >> (j * 8)) & 0xFF);
        }
    }
    return result;
}

static vector<unsigned char> elgamalDecryptData(const vector<unsigned char>& data, unsigned char key) {
    long long p, g, y, x;
    generateParams(key, p, g, y, x);
    
    vector<unsigned char> result;
    
    for (size_t i = 0; i < data.size(); i += 8) {
        if (i + 7 >= data.size()) break;
        
        long long a = 0, b = 0;
        for (int j = 0; j < 4; ++j) {
            a |= (long long)data[i + j] << (j * 8);
        }
        for (int j = 0; j < 4; ++j) {
            b |= (long long)data[i + 4 + j] << (j * 8);
        }
        
        long long ax = modPow(a, x, p);
        long long axInv = modInverse(ax, p);
        long long m = (b * axInv) % p;
        
        result.push_back((unsigned char)m);
    }
    return result;
}

extern "C" {

const char* encrypt_text(const char* text, unsigned char key) {
    if (!text) return "";
    g_resultString = elgamalEncryptString(string(text), key);
    return g_resultString.c_str();
}

const char* decrypt_text(const char* cipher, unsigned char key) {
    if (!cipher) return "";
    g_resultString = elgamalDecryptString(string(cipher), key);
    return g_resultString.c_str();
}

unsigned char* encrypt_data(const unsigned char* data, int dataSize, unsigned char key, int* outSize) {
    if (!data || dataSize <= 0) {
        *outSize = 0;
        return nullptr;
    }
    vector<unsigned char> input(data, data + dataSize);
    g_resultData = elgamalEncryptData(input, key);
    *outSize = g_resultData.size();
    return g_resultData.data();
}

unsigned char* decrypt_data(const unsigned char* data, int dataSize, unsigned char key, int* outSize) {
    if (!data || dataSize <= 0) {
        *outSize = 0;
        return nullptr;
    }
    vector<unsigned char> input(data, data + dataSize);
    g_resultData = elgamalDecryptData(input, key);
    *outSize = g_resultData.size();
    return g_resultData.data();
}

unsigned char generate_key() {
    return elgamalGenerateKey();
}

const char* get_algorithm_name() {
    g_resultString = "Эль-Гамаль (упрощённый)";
    return g_resultString.c_str();
}

void free_memory(void* ptr) {}

}
