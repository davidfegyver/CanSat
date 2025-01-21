#include "mbedtls/sha256.h"
#include <vector>
#include "math.h"

class Crypto
{
public:
    Crypto();
    ~Crypto() = default;

    std::vector<std::vector<uint8_t>> split_vector(std::vector<uint8_t> data, uint8_t n);
    std::vector<uint8_t> sha256(std::vector<uint8_t> data);
    int calculate_n(int input_len, int alpha, int crypto_hash_len);

};