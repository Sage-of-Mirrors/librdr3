// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/crypto/secret.cpp

#include "crypto/secret.hpp"
#include "crypto/crc.hpp"
#include "util/bstream.h"
#include "util/callback.hpp"

#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>

// Closest prime to floor(2**32/golden_ratio)
static constexpr uint32_t PolyFactor32 = 0x9E3779B1;

template <typename T>
static inline bool bit_test(const T* bits, size_t index) {
    constexpr usize Radix = sizeof(T) * CHAR_BIT;

#if defined(_MSC_VER) && !defined(__clang__) && (defined(_M_IX86) || defined(_M_X64))
    // `index % Radix` should be a no-op on x86, as it is masked off by shl/shr/bt
    // Yet MSVC still generates a horrible `movzx, and, movzx` sequence
    // Note also, _bittest{64} is not recommended due to its high latency
    return (bits[index / Radix] & (T(1) << (index /*% Radix*/))) != 0;
#else
    return (bits[index / Radix] & (T(1) << (index % Radix))) != 0;
#endif
}

template <typename T>
static inline void bit_set(T* bits, size_t index) {
    constexpr usize Radix = sizeof(T) * CHAR_BIT;

    bits[index / Radix] |= (T(1) << (index % Radix));
}

// b85alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~"
// https://github.com/python/cpython/blob/22df2e0322300d25c1255ceb73cacc0ebd96b20e/Lib/base64.py#L461

static inline char b85encode1(uint8_t value) {
    static const char encoder[85 + 1] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

    return (value < 85) ? encoder[value] : '~';
}

static inline size_t b85enclen(size_t length) {
    return length + ((length + 3) / 4);
}

static inline size_t b85declen(size_t length) {
    return length - ((length + 4) / 5);
}

size_t b85encode(const void* input, size_t input_len, char* output, size_t output_len) {
    const uint8_t* next_in = static_cast<const uint8_t*>(input);
    char* next_out = output;

    size_t avail_in = input_len;
    size_t avail_out = output_len;

    while (avail_in && avail_out) {
        uint32_t acc = 0;
        size_t valid = 1;

        for (size_t i = 0; i < 4; ++i) {
            acc <<= 8;

            if (avail_in) {
                acc |= static_cast<uint32_t>(*next_in++);
                --avail_in;

                ++valid;
            }
        }

        char chunk[5]{};

        for (size_t i = 0; i < 5; ++i) {
            chunk[4 - i] = b85encode1(acc % 85);
            acc /= 85;
        }

        for (size_t i = 0; i < valid; ++i) {
            if (avail_out) {
                *next_out++ = chunk[i];
                --avail_out;
            }
        }
    }

    return output_len - avail_out;
}

static inline uint8_t b85decode1(char value) {
    static const uint8_t decoder[94] = {
        // clang-format off
        62, 85, 63, 64, 65, 66, 85, 67, 68, 69, 70, 85, 71, 85, 85,
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  85, 72, 73, 74, 75,
        76, 77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 85, 85,
        85, 78, 79, 80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
        47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
        81, 82, 83, 84,
        // clang-format on
    };

    return (value >= 33 && value <= 126) ? decoder[value - 33] : 85;
}

size_t b85decode(const char* input, size_t input_len, void* output, size_t output_len) {
    const char* next_in = input;
    uint8_t* next_out = static_cast<uint8_t*>(output);

    size_t avail_in = input_len;
    size_t avail_out = output_len;

    while (avail_in && avail_out) {
        uint32_t acc = 0;
        size_t valid = 0;

        for (size_t i = 0; i < 5; ++i) {
            uint8_t v = 84;

            if (avail_in) {
                v = b85decode1(*next_in++);
                --avail_in;

                if (v >= 85) {
                    return 0;
                }

                ++valid;
            }

            acc = (acc * 85) + v;
        }

        for (size_t i = 1; i < valid; ++i) {
            if (avail_out) {
                *next_out++ = acc >> 24;
                --avail_out;

                acc <<= 8;
            }
        }
    }

    return output_len - avail_out;
}

void SearchOverlappingChunks(size_t chunk_size, size_t chunk_overlap, size_t num_chunks, size_t num_workers,
    size_t num_threads, Callback<std::pair<uint64_t, size_t>(void*, size_t)> read_data,
    Callback<bool(size_t, const uint8_t*, const uint8_t*, size_t)> process_data) {
    struct CChunk
    {
        // Start of new Data
        uint8_t* Data{};

        // Length of new data
        size_t Length{};

        // Length of data saved from the previous chunk
        size_t Overlap{};

        // Number of workers that need to start processing this chunk
        size_t Pending{};

        // Number of workers that need to finish processing this chunk
        size_t Running{};

        // Pointer to the start of the raw data
        std::unique_ptr<uint8_t[]> RawData{};
    };

    std::mutex chunk_lock;
    std::vector<CChunk> chunks(num_chunks);
    size_t reader_index = 0;
    size_t writer_index = 0;
    bool finished = false;

    std::condition_variable reader_cond;
    std::condition_variable writer_cond;

    const auto worker_func = [&, process_data] {
        std::unique_lock lock(chunk_lock);

        while (true)
        {
            auto& chunk = chunks[reader_index];

            if (chunk.Pending == 0)
            {
                if (finished)
                    break;

                reader_cond.wait(lock);
                continue;
            }

            size_t work_index = num_workers - chunk.Pending;

            if (--chunk.Pending == 0)
                reader_index = (reader_index + 1) % chunks.size();

            lock.unlock();

            bool stop =
                !process_data(work_index, chunk.Data - chunk.Overlap, chunk.Data + chunk.Length, chunk.Overlap);

            lock.lock();

            if (--chunk.Running == 0)
                writer_cond.notify_one();

            if (stop)
                finished = true;
        }
        };

    std::vector<std::thread> workers;

    for (size_t i = 0; i < num_threads; ++i)
        workers.emplace_back(worker_func);

    uint8_t* saved_data = nullptr;
    size_t saved = 0;
    uint64_t prev_pos = 0;

    {
        std::unique_lock lock(chunk_lock);

        while (!finished)
        {
            auto& chunk = chunks[writer_index];

            if (chunk.Running != 0)
            {
                writer_cond.wait(lock);
                continue;
            }

            writer_index = (writer_index + 1) % num_chunks;

            lock.unlock();

            if (chunk.Data == nullptr)
            {
                chunk.RawData.reset(new uint8_t[chunk_overlap + chunk_size]);
                chunk.Data = &chunk.RawData[chunk_overlap];
            }

            auto [read_pos, read] = read_data(chunk.Data, chunk_size);

            if (read == 0)
                break;

            if (read_pos != prev_pos)
                saved = 0;

            if (saved)
                std::memmove(chunk.Data - saved, saved_data, saved);

            chunk.Length = read;
            chunk.Overlap = saved;

            prev_pos = read_pos + read;
            saved = (std::min)(saved + read, chunk_overlap);
            saved_data = chunk.Data + read - saved;

            lock.lock();

            chunk.Pending = num_workers;
            chunk.Running = num_workers;

            reader_cond.notify_all();
        }

        finished = true;
        reader_cond.notify_all();
    }

    for (auto& worker : workers)
        worker.join();
}

// Using a non-zero seed will break the "rolling" property of the hash.
// It is also the same as just returning `hash + seed * pow(PolyFactor32, length)`
static inline uint32_t PolyHash32(const void* data, size_t length)
{
    const uint8_t* data8 = static_cast<const uint8_t*>(data);

    uint32_t hash = 0;

    for (size_t i = 0; i < length; ++i)
    {
        hash += data8[i];
        hash *= PolyFactor32;
    }

    return hash;
}

// Returns `-1 * pow(HashFactor, length, 2**32)`
// Used to remove (subtract) the length-1'th value from a hash
static inline uint32_t PolyHashInverse(size_t length)
{
    uint32_t v = 0xFFFFFFFF;

    for (uint32_t x = PolyFactor32; length; x *= x, length >>= 1)
    {
        if (length & 1)
            v *= x;
    }

    return v;
}

// Returns a permutation of the integer to use as a second hash
static inline uint32_t Permute32(uint32_t x)
{
    return ~x * PolyFactor32;
}

static_assert(sizeof(CSecretId) == 0x10);

std::string CSecretId::To85() const
{
    char buffer[20];
    size_t encoded = b85encode(this, sizeof(*this), buffer, std::size(buffer));

    return std::string(buffer, encoded);
}

std::optional<CSecretId> CSecretId::From85(std::string_view text)
{
    CSecretId result;
    size_t decoded = b85decode(text.data(), text.size(), &result, sizeof(result));

    if (decoded != sizeof(result))
        return std::nullopt;

    if (result.Length == 0)
        return std::nullopt;

    return result;
}

CSecretId CSecretId::FromData(const void* data, size_t length)
{
    return { static_cast<uint32_t>(length), PolyHash32(data, length), CRC64(data, length) };
}

struct CSecretSearcher
{
    CSecretSearcher(size_t length);

    void AddSecret(CSecretId secret);
    void Finalize();

    uint32_t Begin(const uint8_t* data) const;
    uint32_t Next(const uint8_t*& start, const uint8_t* end, uint32_t& state) const;

    bool Search(const uint8_t* start, const uint8_t* end, size_t overlap, Callback<bool(CSecretId, const uint8_t*)> add_result) const;

    size_t Length{};
    uint32_t InvFactor{};

    using FilterWord = size_t;

    std::unique_ptr<FilterWord[]> Filter{};
    uint8_t FilterBits{};

    std::unordered_map<uint32_t, std::vector<CSecretId>> Secrets{};
    size_t NumSecrets{};
};

CSecretSearcher::CSecretSearcher(size_t length)
    : Length(length)
    , InvFactor(PolyHashInverse(length))
{}

void CSecretSearcher::AddSecret(CSecretId secret)
{
    auto& secrets = Secrets[secret.Hash];

    if (std::find(secrets.begin(), secrets.end(), secret) != secrets.end())
        return;

    secrets.push_back(secret);
    ++NumSecrets;
}

void CSecretSearcher::Finalize()
{
    //constexpr size_t LoadFactor = 256;

    //FilterBits = std::clamp<uint8_t>(bits::bsr_ceil(Secrets.size() * LoadFactor), 10, 18);

    //constexpr size_t FilterRadix = sizeof(FilterWord) * CHAR_BIT;
    //size_t filter_size = size_t(1) << FilterBits;
    //Filter.reset(new FilterWord[1 + ((filter_size - 1) / FilterRadix)]{});

    //uint8_t filter_shift = 32 - FilterBits;

    //// Setup a bloom filter using 3 hash functions.
    //// Calculating additional indepenent hashes would be slower than just performing a lookup into the Hashes table.
    //for (const auto& [hash, _] : Secrets)
    //{
    //    uint32_t h = hash;
    //    bit_set(Filter.get(), h >> filter_shift);

    //    h = Permute32(h);
    //    bit_set(Filter.get(), h >> filter_shift);

    //    h = Permute32(h);
    //    bit_set(Filter.get(), h >> filter_shift);
    //}
}

inline uint32_t CSecretSearcher::Begin(const uint8_t* data) const
{
    return PolyHash32(data, Length - 1);
}

#if defined(_M_IX86) // Inlining increases register spills on x86
SW_NOINLINE
#endif
uint32_t CSecretSearcher::Next(const uint8_t*& inout_start, const uint8_t* end, uint32_t& inout_state) const
{
    //const size_t last = Length - 1;
    //const uint32_t inv_factor = InvFactor;

    //const FilterWord* filter = Filter.get();
    //const uint8_t filter_shift = 32 - FilterBits;

    //const uint8_t* here = inout_start;
    //uint32_t state = inout_state;
    uint32_t hash = 0;

    //while (SW_LIKELY(here < end))
    //{
    //    [[SW_ATTR_LIKELY]];

    //    uint32_t h = (state + here[last]) * PolyFactor32;
    //    state = h + (here[0] * inv_factor);
    //    ++here;

    //    if (SW_LIKELY(!bit_test(filter, h >> filter_shift)))
    //    {
    //        [[SW_ATTR_LIKELY]];
    //        continue;
    //    }

    //    h = Permute32(h);
    //    if (SW_LIKELY(!bit_test(filter, h >> filter_shift)))
    //    {
    //        [[SW_ATTR_LIKELY]];
    //        continue;
    //    }

    //    h = Permute32(h);
    //    if (SW_LIKELY(!bit_test(filter, h >> filter_shift)))
    //    {
    //        [[SW_ATTR_LIKELY]];
    //        continue;
    //    }

    //    --here;
    //    hash = state - (here[0] * inv_factor);
    //    break;
    //}

    //inout_start = here;
    //inout_state = state;

    return hash;
}

bool CSecretSearcher::Search(
    const uint8_t* start, const uint8_t* end, size_t overlap, Callback<bool(CSecretId, const uint8_t*)> add_result) const
{
    const size_t last = Length - 1;

    start += overlap - (std::min)(overlap, last);

    if (static_cast<size_t>(end - start) <= last)
        return true;

    end -= last;

    uint32_t state = Begin(start);

    // TODO: Add fast path for NumSecrets == 1

    while (start < end)
    {
        const uint8_t* here = start;
        uint32_t hash = Next(here, end, state);

        if (here < end)
        {
            if (auto iter = Secrets.find(hash); iter != Secrets.end())
            {
                for (const auto& secret : iter->second)
                {
                    if (secret.Crc == CRC64(here, secret.Length))
                    {
                        if (!add_result(secret, here))
                            return false;
                    }
                }
            }

            ++here;
        }

        start = here;
    }

    return true;
}

static std::vector<CSecretSearcher> MakeSearchers(std::unordered_set<CSecretId> secrets)
{
    std::vector<CSecretSearcher> searchers;

    for (const auto& secret : secrets)
    {
        auto iter = std::find_if(searchers.begin(), searchers.end(),
            [&](const auto& searcher) { return searcher.Length == secret.Length; });

        if (iter == searchers.end())
        {
            iter = searchers.emplace(iter, secret.Length);
        }

        iter->AddSecret(secret);
    }

    for (auto& searcher : searchers)
        searcher.Finalize();

    return searchers;
}

std::unordered_map<CSecretId, std::vector<uint8_t>> FindSecrets(
    std::unordered_set<CSecretId> secrets, Callback<std::pair<uint64_t, size_t>(void*, size_t)> read_data)
{
    const auto searchers = MakeSearchers(std::move(secrets));

    size_t num_secrets = 0;
    size_t prefix_length = 0;
    std::unordered_map<size_t, std::atomic<size_t>> num_pending;

    for (const auto& searcher : searchers)
    {
        num_pending.emplace(searcher.Length, searcher.NumSecrets);
        num_secrets += searcher.NumSecrets;
        prefix_length = std::max<size_t>(prefix_length, searcher.Length);
    }

    prefix_length = (prefix_length + 0x3F) & ~size_t(0x3F);

    std::mutex results_lock;
    std::unordered_map<CSecretId, std::vector<uint8_t>> results;

    const auto add_result = [&results_lock, &results, &num_pending, num_secrets](CSecretId secret, const uint8_t* data) {
        std::lock_guard<std::mutex> lock(results_lock);

        if (auto [iter, added] = results.try_emplace(secret, data, data + secret.Length); added)
        {
            --num_pending.at(secret.Length);
        }
        else if (std::memcmp(data, iter->second.data(), iter->second.size()))
        {
            // TODO: How should this be handled?
        }

        return results.size() < num_secrets;
        };

    const auto process_data = [&searchers, &num_pending, &add_result](
        size_t index, const uint8_t* start, const uint8_t* end, size_t overlap) {
            const auto& searcher = searchers.at(index);

            if (num_pending.at(searcher.Length) == 0)
                return true;

            return searcher.Search(start, end, overlap, add_result);
        };

    size_t const num_workers = searchers.size();
    size_t const num_threads = std::clamp<size_t>(std::thread::hardware_concurrency(), 4, 16) - 1;

    size_t const num_chunks = 16;
    size_t const chunk_size = size_t(2) << 20;

    SearchOverlappingChunks(
        chunk_size, prefix_length, num_chunks, num_workers, num_threads, read_data, process_data);

    return results;
}

void CSecretFinder::Add(CSecretId id)
{
    secrets_.emplace(id);
}

void CSecretFinder::Add(const char* id)
{
    Add(CSecretId::From85(id).value());
}

std::unordered_map<CSecretId, std::vector<uint8_t>> CSecretFinder::Search(bStream::CStream* stream)
{
    std::unordered_map<CSecretId, std::vector<uint8_t>> results;
    std::unordered_set<CSecretId> needles;

    std::vector<uint8_t> data;

    for (const auto& secret : secrets_)
    {
        data.resize(secret.Length);

        if (Secrets.Get(secret, data.data(), data.size()))
        {
            results.emplace(secret, data);
        }
        else
        {
            needles.emplace(secret);
        }
    }

    const auto read_data = [&stream](void* buffer, size_t length) -> std::pair<uint64_t, size_t> {
        stream->readBytesTo((uint8_t*)buffer, length);
        length = stream->tell();

        // Calculate offset after reading, to support sparse streams (Win32ProcStream)
        uint64_t here = stream->tell() - length;

        return { here, length };
        };

    if (!needles.empty())
    {
        std::unordered_map<CSecretId, std::vector<uint8_t>> found = FindSecrets(needles, read_data);
        results.insert(found.begin(), found.end());
    }

    return results;
}

CSecretStore Secrets;

void CSecretStore::Add(std::vector<uint8_t> data)
{
    CSecretId id = CSecretId::FromData(data.data(), data.size());

    secrets_.emplace(id, std::move(data));
}

void CSecretStore::Add(const std::unordered_map<CSecretId, std::vector<uint8_t>>& secrets)
{
    for (const auto& [_, data] : secrets)
        Add(data);
}

bool CSecretStore::Get(CSecretId id, void* output, size_t length)
{
    auto iter = secrets_.find(id);

    if (iter == secrets_.end())
        return false;

    if (iter->second.size() != length)
        return false;

    std::memcpy(output, iter->second.data(), iter->second.size());

    return true;
}

bool CSecretStore::Get(const char* id, void* output, size_t length)
{
    return Get(CSecretId::From85(id).value(), output, length);
}

void CSecretStore::Load(bStream::CStream* input)
{
    //uint32_t length = 0;

    //while (input.TryRead(&length, sizeof(length)))
    //{
    //    std::vector<uint8_t> data(length);

    //    if (!input.TryRead(data.data(), ByteSize(data)))
    //        break;

    //    Add(std::move(data));
    //}
}

void CSecretStore::Save(bStream::CStream* output)
{
    //for (const auto& [_, data] : secrets_)
    //{
    //    uint32_t length = static_cast<uint32_t>(data.size());
    //    output->writeUInt32(length);
    //    output->writeBytesTo(data.data(), data.size());
    //}
}

void CSecretStore::Load()
{
    //if (Rc<Stream> s = AssetManager::Open("user:/secrets.bin"))
        //Secrets.Load(*s);
}

void CSecretStore::Save()
{
    //if (Rc<Stream> s = AssetManager::Create("user:/secrets.bin"))
        //Secrets.Save(*s);
}
