#include "crypto.h"

Crypto::Crypto() {}

std::vector<std::vector<uint8_t>> Crypto::split_vector(std::vector<uint8_t> data, uint8_t n)
{
    std::vector<std::vector<uint8_t>> result(n);

    for (int i = 0; i < data.size(); i++)
    {
        result[i % n].push_back(data[i]);
    }
    int max_len = result[0].size();

    for (int i = 0; i < n; i++)
    {
        while (result[i].size() % max_len != 0)
        {
            result[i].push_back(0);
        }
    }

    return result;
}

std::vector<uint8_t> Crypto::sha256(std::vector<uint8_t> data)
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);

    mbedtls_sha256_update(&ctx, data.data(), data.size());

    std::vector<uint8_t> hash(32);
    mbedtls_sha256_finish(&ctx, hash.data());

    mbedtls_sha256_free(&ctx);

    return hash;
}

std::vector<uint8_t> Crypto::sha256(uint8_t *data, size_t data_len)
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);

    mbedtls_sha256_update(&ctx, data, data_len);

    std::vector<uint8_t> hash(32);
    mbedtls_sha256_finish(&ctx, hash.data());

    mbedtls_sha256_free(&ctx);

    return hash;
}

int Crypto::calculate_n(int input_len, int alpha, int crypto_hash_len)
{
    int n = ceil(sqrt(input_len * alpha / crypto_hash_len));

    int remainder = n % 6;

    if (remainder == 0)
    {
        return n + 1;
    }

    if (remainder == 1 || remainder == 5)
    {
        return n;
    }

    if (remainder == 2 || remainder == 3 || remainder == 4)
    {
        return n + (5 - remainder);
    }

    return n;
}

String Crypto::uint8_vector_to_string(std::vector<uint8_t> data)
{
    String result = "";
    for (int i = 0; i < data.size(); i++)
    {
        if (data[i] < 0x10)
        {
            result += "0"; 
        }
        result += String(data[i], HEX);
    }
    return result;
}