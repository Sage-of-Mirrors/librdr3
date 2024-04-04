#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

namespace bStream {
    class CStream;
    class CFileStream;
    class CMemoryStream;
}

template<typename T>
using shared_vector = std::vector<std::shared_ptr<T>>;
