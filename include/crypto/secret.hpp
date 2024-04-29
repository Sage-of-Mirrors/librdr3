// Copied and adapted from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/secret.h

#pragma once

#include "types.h"

#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <optional>
#include <string>

namespace rdr3 {
    namespace crypto {
        struct CSecretId
        {
            uint32_t Length{ };
            uint32_t Hash{ };
            uint64_t Crc{ };

            std::string To85() const;

            static std::optional<CSecretId> From85(std::string_view text);
            static CSecretId FromData(const void* data, size_t length);

            inline bool operator==(const CSecretId& rhs) const
            {
                return (Length == rhs.Length) && (Hash == rhs.Hash) && (Crc == rhs.Crc);
            }
        };

        template <typename T>
        inline void HashCombine(std::size_t& seed, const T& value)
        {
            seed ^= std::hash<T> {}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    }
}

template <>
struct std::hash<rdr3::crypto::CSecretId>
{
    std::size_t operator()(const rdr3::crypto::CSecretId& s) const noexcept
    {
        std::size_t seed = 0;
        rdr3::crypto::HashCombine(seed, s.Length);
        rdr3::crypto::HashCombine(seed, s.Hash);
        rdr3::crypto::HashCombine(seed, s.Crc);
        return seed;
    }
};

namespace rdr3 {
    namespace crypto {
        class CSecretFinder
        {
        public:
            void Add(CSecretId id);
            void Add(const char* id);

            std::unordered_map<CSecretId, std::vector<uint8_t>> Search(bStream::CStream* stream);

        private:
            std::unordered_set<CSecretId> secrets_;
        };

        class CSecretStore
        {
        public:
            void Add(std::vector<uint8_t> data);
            void Add(const std::unordered_map<CSecretId, std::vector<uint8_t>>& secrets);

            bool Get(CSecretId id, void* output, size_t length);
            bool Get(const char* id, void* output, size_t length);

            void Load(bStream::CFileStream* input);
            void Save(bStream::CStream* output);

            bool TryLoad();
            void Save();

        private:
            std::unordered_map<CSecretId, std::vector<uint8_t>> secrets_;
        };
    }
}
