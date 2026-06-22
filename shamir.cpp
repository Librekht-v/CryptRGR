#include "../algorithm_interface.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;

static string g_resultString;
static vector<unsigned char> g_resultData;

static int g_n = 5;
static int g_k = 3;

static int getPrime(unsigned char key) {
    const int primes[] = {
        257, 263, 269, 271, 277, 281, 283, 293, 307, 311,
        313, 317, 331, 337, 347, 349, 353, 359, 367, 373
    };
    int idx = key % (sizeof(primes) / sizeof(primes[0]));
    return primes[idx];
}

static int modPow(int a, int e, int mod) {
    int res = 1;
    a %= mod;
    while (e > 0) {
        if (e & 1) res = (res * a) % mod;
        a = (a * a) % mod;
        e >>= 1;
    }
    return res;
}

static int modInv(int a, int mod) {
    return modPow(a, mod - 2, mod);
}

// Разбиваем каждый символ на g_n фрагментов через полином степени g_k-1
static string shamirEncryptString(const string& text, unsigned char key) {
    int P = getPrime(key);
    int len = text.length();
    
    // Матрица фрагментов: g_n строк, len столбцов
    vector<vector<int>> shares(g_n, vector<int>(len));
    
    srand(time(nullptr));
    
    for (int pos = 0; pos < len; ++pos) {
        int secret = (unsigned char)text[pos];
        
        // Случайные коэффициенты полинома
        vector<int> coeff(g_k);
        coeff[0] = secret;
        for (int i = 1; i < g_k; ++i) {
            coeff[i] = rand() % (P - 1) + 1;
        }
        
        // Вычисляем значения полинома в точках x = 1..g_n
        for (int i = 0; i < g_n; ++i) {
            int x = i + 1;
            int y = 0, xPow = 1;
            for (int j = 0; j < g_k; ++j) {
                y = (y + coeff[j] * xPow) % P;
                xPow = (xPow * x) % P;
            }
            shares[i][pos] = y;
        }
    }
    
    // Формат: "n,k,P|значения_фрагмента0;значения_фрагмента1;..."
    string result = to_string(g_n) + "," + to_string(g_k) + "," + to_string(P) + "|";
    
    for (int i = 0; i < g_n; ++i) {
        for (int pos = 0; pos < len; ++pos) {
            if (pos != 0) result += ",";
            result += to_string(shares[i][pos]);
        }
        if (i != g_n - 1) result += ";";
    }
    return result;
}

// Восстанавливаем секрет из k фрагментов через интерполяцию Лагранжа
static string shamirDecryptString(const string& data, unsigned char key) {
    size_t firstSep = data.find('|');
    if (firstSep == string::npos) return "";
    
    string header = data.substr(0, firstSep);
    string sharesData = data.substr(firstSep + 1);
    
    // Парсим заголовок
    size_t pos1 = header.find(',');
    size_t pos2 = header.find(',', pos1 + 1);
    int n = stoi(header.substr(0, pos1));
    int k = stoi(header.substr(pos1 + 1, pos2 - pos1 - 1));
    int P = stoi(header.substr(pos2 + 1));
    
    // Парсим фрагменты
    vector<vector<int>> shares;
    size_t start = 0;
    while (start < sharesData.size()) {
        size_t end = sharesData.find(';', start);
        if (end == string::npos) end = sharesData.size();
        
        string shareStr = sharesData.substr(start, end - start);
        vector<int> values;
        size_t vStart = 0;
        while (vStart < shareStr.size()) {
            size_t vEnd = shareStr.find(',', vStart);
            if (vEnd == string::npos) vEnd = shareStr.size();
            values.push_back(stoi(shareStr.substr(vStart, vEnd - vStart)));
            vStart = vEnd + 1;
        }
        shares.push_back(values);
        start = end + 1;
    }
    
    if (shares.empty() || shares[0].empty()) return "";
    
    int len = shares[0].size();
    string result(len, ' ');
    
    // Интерполяция Лагранжа для каждого символа
    for (int pos = 0; pos < len; ++pos) {
        int secret = 0;
        for (int i = 0; i < k; ++i) {
            int xi = i + 1;
            int yi = shares[i][pos];
            int li = 1;
            for (int j = 0; j < k; ++j) {
                if (i == j) continue;
                int xj = j + 1;
                li = (li * ((0 - xj + P) % P) % P * modInv((xi - xj + P) % P, P)) % P;
            }
            secret = (secret + yi * li) % P;
        }
        result[pos] = (char)secret;
    }
    return result;
}

static unsigned char shamirGenerateKey() {
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
    return (rand() % 255) + 1;
}

extern "C" {

const char* encrypt_text(const char* text, unsigned char key) {
    if (!text) return "";
    g_resultString = shamirEncryptString(string(text), key);
    return g_resultString.c_str();
}

const char* decrypt_text(const char* cipher, unsigned char key) {
    if (!cipher) return "";
    g_resultString = shamirDecryptString(string(cipher), key);
    return g_resultString.c_str();
}

unsigned char* encrypt_data(const unsigned char* data, int dataSize, unsigned char key, int* outSize) {
    if (!data || dataSize <= 0) {
        *outSize = 0;
        return nullptr;
    }
    // реализовать шифрование данных 
    *outSize = 0;
    return nullptr;
}

unsigned char* decrypt_data(const unsigned char* data, int dataSize, unsigned char key, int* outSize) {
    if (!data || dataSize <= 0) {
        *outSize = 0;
        return nullptr;
    }
    // реализовать дешифрование данных 
    *outSize = 0;
    return nullptr;
}

unsigned char generate_key() {
    return shamirGenerateKey();
}

const char* get_algorithm_name() {
    g_resultString = "Схема разделения секрета Шамира";
    return g_resultString.c_str();
}

void free_memory(void* ptr) {}

} 
