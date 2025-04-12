
#pragma once

#include <cstdint>

namespace pancake_store::datanode::storage {

enum class LayoutType : uint8_t {
    UNKNOWN = 0,
    PADDING = 1,
    SUPER_BLOCK = 2,
    SUPER_BLOCK_BACKUP = 3,
    EXTENT_BIT_MAP = 4,
    JOURNAL_BLOCKS = 5, // journal use COW
    EXTENTS = 6,
};

struct PancakeFSLayoutItem {
    LayoutType layout_type_{LayoutType::UNKNOWN};
    uint64_t block_index_{0};
    uint64_t block_count_{0};
};

// | PADDING | SUPER_BLOCK | SUPER_BLOCK_BACKUP | JOURNAL_BIT_MAP | EXTENT_BIT_MAP | JOURNAL_BLOCKS | EXTENTS

struct PancakeFSLayout {
    PancakeFSLayoutItem padding_{.layout_type_ = LayoutType::PADDING};
    PancakeFSLayoutItem super_block_{.layout_type_ = LayoutType::SUPER_BLOCK};
    PancakeFSLayoutItem extent_bit_map_{.layout_type_ = LayoutType::EXTENT_BIT_MAP};
    PancakeFSLayoutItem journal_blocks_{.layout_type_ = LayoutType::JOURNAL_BLOCKS};
    PancakeFSLayoutItem extent_blocks_{.layout_type_ = LayoutType::EXTENTS};
};

} // namespace pancake_store::datanode::storage