#include "picosha2.h"

#include <fstream>
#include <random>
#include <string>

class Seed {
   public:
    Seed(const std::string& filePath) { path_ = filePath; }
    bool exists();
    void initSeed();
    void getSeed();
    std::string getHash() { return hash_; }

   private:
    std::string path_, hash_;

    void calcHash(const std::string&);
    void calcHash(std::ifstream&);
};

bool Seed::exists() {
    std::ifstream inFile(path_);
    return (inFile.good());
}

/* Define Command member functions */

// Use <random> header for portability
std::string genSeedData(int rngNumIter) {
    std::string seedData;
    std::random_device randomDev;
    std::mt19937 randNum(randomDev());
    for (int i = 0; i < rngNumIter; ++i) {
        seedData.push_back((char)randNum());
    }

    return seedData;
}

void Seed::initSeed() {
    std::string seedData;
    std::ofstream seedFile(path_, std::ios::binary | std::ios::trunc);
    if (seedFile.is_open()) {
        seedData = genSeedData(SEED_RNG_ITERATIONS);
        seedFile << seedData;
    }

    calcHash(seedData);
}

void Seed::getSeed() {
    if (exists()) {
        std::ifstream seedFile(path_, std::ios::binary);
        if (seedFile.is_open()) calcHash(seedFile);
    }
}

void Seed::calcHash(const std::string& str) {
    hash_ = picosha2::hash256_hex_string(str);
}

// This method may use less memory than getting hex_str from string
void Seed::calcHash(std::ifstream& seedFile) {
    std::vector<unsigned char> hash(32);
    picosha2::hash256(std::istreambuf_iterator<char>(seedFile),
                      std::istreambuf_iterator<char>(), hash.begin(),
                      hash.end());
    hash_ = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
}