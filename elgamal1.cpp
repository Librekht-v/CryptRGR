#ifndef ELGAMAL_STRING_H
#define ELGAMAL_STRING_H

#include "elgamal_core.h"

static string elgamalEncryptString(const string& text, unsigned char key) {
    long long p, g, y, x;
    generateParams(key, p, g, y, x);
    
    string result;
    srand(time(nullptr));
    
    for (size_t i = 0; i < text.size(); ++i) {
        long long m = (unsigned char)text[i];
        long long k = (rand() % (p - 2)) + 1;
        long long a = modPow(g, k, p);
        long long yk = modPow(y, k, p);
        long long b = (yk * m) % p;
        
        result += to_string(a) + "|" + to_string(b);
        if (i != text.size() - 1) result += ",";
    }
    return result;
}

static string elgamalDecryptString(const string& cipherText, unsigned char key) {
    long long p, g, y, x;
    generateParams(key, p, g, y, x);
    
    string result;
    size_t pos = 0;
    size_t blockStart = 0;
    
    while (pos < cipherText.size()) {
        if (cipherText[pos] == ',' || pos == cipherText.size() - 1) {
            string block;
            if (pos == cipherText.size() - 1) {
                block = cipherText.substr(blockStart, pos - blockStart + 1);
            } else {
                block = cipherText.substr(blockStart, pos - blockStart);
            }
            
            size_t delim = block.find('|');
            if (delim != string::npos) {
                long long a = stoll(block.substr(0, delim));
                long long b = stoll(block.substr(delim + 1));
                long long ax = modPow(a, x, p);
                long long axInv = modInverse(ax, p);
                long long m = (b * axInv) % p;
                result += (char)m;
            }
            blockStart = pos + 1;
        }
        ++pos;
    }
    return result;
}

#endif
