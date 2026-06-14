// algorithms/xor.cpp
#include "algorithm_interface.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

static string g_resultString;
static vector<unsigned char> g_resultData;

static string xorEncryptDecrypt(const string& text, unsigned char key) {
    string result = text;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = (char)((unsigned char)result[i] ^ key);
    }
    return result;
}

static vector<unsigned char> xorEncryptDecryptData(const vector<unsigned char>& data, unsigned char key) {
    vector<unsigned char> result = data;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = result[i] ^ key;
    }
    return result;
}

static unsigned char xorGenerateKey() {
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    return (rand() % 255) + 1;
}
