#pragma once

#include <cstddef>

namespace rdb::sql {

struct Location {
    Location(size_t offset, size_t rows, size_t cols)
        : offset_(offset), rows_(rows), cols_(cols) {}
    size_t offset_;
    size_t rows_;
    size_t cols_;
};

} // namespace rdb::sql
