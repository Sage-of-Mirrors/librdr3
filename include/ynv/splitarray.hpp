#pragma once

#include "types.h"
#include "util/streamutil.hpp"

#include <bstream.h>

#include <vector>
#include <memory>
#include <functional>

constexpr uint64_t SPLIT_ARRAY_HEADER_SIZE = 0x30;
constexpr uint64_t BUCKET_INFO_SIZE        = 0x08;

namespace UNavmesh {
    // Implementation of Rockstar's split array type.
    // Elements are stored in buckets of an arbitrary size, but the array still acts like it's contiguous.
    template<typename T, uint32_t BucketSize>
    class USplitArray {
        using shared_vector = std::vector<std::shared_ptr<T>>;
        using bucket = std::vector<shared_vector>;

        uint32_t mTotalSize;
        bucket mBuckets;

        // These members are used to store data during serialization.

        // Flag to indicate this array has serialized its data. Allows SerializeMetadata() to run.
        bool bSerializedData;
        // File offsets of each bucket's data.
        std::vector<uint32_t> mDataOffsets;
        // Starting indices of each bucket's data in the global list.
        std::vector<uint32_t> mBucketIndices;

    public:
        USplitArray() : mTotalSize(0), bSerializedData(false) { mBuckets.push_back(shared_vector()); }
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

        uint32_t SerializeData(bStream::CStream* stream, std::function<void(bStream::CStream*, std::shared_ptr<T>)> elementSerializeFunc) {
            stream->seek(0, true);

            uint32_t index = 0;
            size_t startPos = stream->tell();

            for (size_t i = 0; i < mBuckets.size(); i++) {
                mDataOffsets.push_back(uint32_t(stream->tell()));
                mBucketIndices.push_back(index);

                for (std::shared_ptr<T> elem : mBuckets[i]) {
                    elementSerializeFunc(stream, elem);
                }

                index += uint32_t(mBuckets[i].size());
            }

            UStreamUtil::PadStream(stream, 16);

            bSerializedData = true;
            return uint32_t(stream->tell() - startPos);
        }

        uint64_t SerializeMetadata(bStream::CStream* stream, uint32_t id) {
            if (!bSerializedData) {
                return 0;
            }

            uint64_t startPos = uint64_t(stream->tell());
            uint64_t bucketInfoOffset = startPos + SPLIT_ARRAY_HEADER_SIZE;
            uint64_t indexInfoOffset = bucketInfoOffset + (BUCKET_INFO_SIZE * mBuckets.size());

            stream->writeUInt32(id);                               // Unknown value
            stream->writeUInt32(1);                                // Unknown value, always 1?
            stream->writeUInt32(mTotalSize);                       // Total number of elements in the array
            stream->writeUInt32(0);                                // Unknown value, might just be the upper half of the total element count?
            UStreamUtil::SerializePtr64(stream, bucketInfoOffset); // Bucket info pointer
            UStreamUtil::SerializePtr64(stream, indexInfoOffset);  // Index info pointer
            stream->writeUInt32(uint32_t(mBuckets.size()));        // Number of buckets

            stream->writeUInt32(0);                                // Padding
            stream->writeUInt32(0);                                // Padding
            stream->writeUInt32(0);                                // Padding

            // Write bucket info
            for (size_t i = 0; i < mBuckets.size(); i++) {
                UStreamUtil::SerializePtr64(stream, mDataOffsets[i]); // Offset to the bucket's data

                stream->writeUInt32(uint32_t(mBuckets[i].size()));    // Number of elements in this bucket
                stream->writeUInt32(0);                               // Unknown value, might just be the upper half of element count?
            }

            // Write bucket indices
            for (size_t i = 0; i < mBuckets.size(); i++) {
                stream->writeUInt32(mBucketIndices[i]);
            }

            UStreamUtil::PadStream(stream, 16);
            bSerializedData = false;

            return startPos;
        }
    };
}
