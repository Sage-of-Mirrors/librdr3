#pragma once

#include "types.h"
#include "util/streamutil.hpp"

#include <bstream.h>

#include <vector>
#include <memory>
#include <functional>

namespace UNavmesh {
    // Implementation of Rockstar's split array type.
    // Elements are stored in buckets of an arbitrary size, but the array still acts like it's contiguous.
    template<typename T, uint32_t BucketSize>
    class USplitArray {
        using shared_vector = std::vector<std::shared_ptr<T>>;
        using bucket = std::vector<shared_vector>;

        uint32_t mTotalSize;
        bucket mBuckets;

    public:
        USplitArray() : mTotalSize(0) { mBuckets.push_back(shared_vector()); }
        ~USplitArray() { }

        uint32_t GetBucketCount() const { return mBuckets.size(); }
        uint32_t GetBucketSize(uint32_t idx) const { return mBuckets[idx].size(); }
        uint32_t GetTotalSize() { return mTotalSize; }

        uint32_t GetMaxBucketSize() const { return BucketSize; }
        shared_vector* GetHead() { return &mBuckets.back(); }

        void Append(std::shared_ptr<T> item) {
            shared_vector* head = GetHead();

            if (head->size() == BucketSize) {
                mBuckets.push_back(shared_vector());
                head = GetHead();
            }

            head->push_back(item);
            mTotalSize++;
        }

        std::shared_ptr<T> Get(const uint32_t i) {
            uint32_t bucketIdx = i / BucketSize;
            uint32_t elementIdx = i % BucketSize;

            return mBuckets[bucketIdx][elementIdx];
        }

        std::shared_ptr<T> operator[](const uint32_t i) {
            return Get(i);
        }

        void Deserialize(bStream::CStream* stream, std::function<void(bStream::CStream*, std::shared_ptr<T>)> elementDeserializeFunc) {
            uint64_t offset = UStreamUtil::DeserializePtr64(stream);

            size_t returnPos = stream->tell();
            stream->seek(offset);

            uint32_t unk00 = stream->readUInt32();
            uint32_t unk04 = stream->readUInt32();
            uint32_t totalElementCount = stream->readUInt32();
            uint32_t unk0C = stream->readUInt32();
            uint64_t dataTableOffset = UStreamUtil::DeserializePtr64(stream);
            uint64_t indexTableOffset = UStreamUtil::DeserializePtr64(stream);
            uint32_t dataTableCount = stream->readUInt32();

            assert(stream->readUInt32() == 0);
            assert(stream->readUInt32() == 0);
            assert(stream->readUInt32() == 0);

            stream->seek(dataTableOffset);

            for (uint32_t tableIdx = 0; tableIdx < dataTableCount; tableIdx++) {
                uint64_t dataOffset = UStreamUtil::DeserializePtr64(stream);
                uint64_t dataCount = stream->readUInt64();

                size_t tableStreamPos = stream->tell();
                stream->seek(dataOffset);

                for (uint32_t elemIdx = 0; elemIdx < dataCount; elemIdx++) {
                    std::shared_ptr<T> newElem = std::make_shared<T>();
                    elementDeserializeFunc(stream, newElem);

                    Append(newElem);
                }

                stream->seek(tableStreamPos);
            }

            stream->seek(returnPos);
        }
    };
}
