
#pragma once

#include <cstdint>
#include "src/comm/error_code.h"

namespace pancake_store::datanode::storage {

using pancake_store::comm::ErrorCode;

enum class LayoutType : uint8_t {
    UNKNOWN = 0,
    PADDING = 1,
    SUPER_BLOCK = 2,
    SUPER_BLOCK_BACKUP = 3,
    EXTENT_BIT_MAP = 4,
    TRASH_EXTENT_BIT_MAP = 5,
    JOURNAL_BLOCKS = 6, // journal use COW
    BAD_BLOCK_BIT_MAP = 7,
    EXTENTS = 8,
};

struct PancakeFSLayoutItem {
    LayoutType layout_type_{LayoutType::UNKNOWN};
    uint64_t block_index_{0};
    uint64_t block_count_{0};
};

// | PADDING | SUPER_BLOCK | SUPER_BLOCK_BACKUP | JOURNAL_BIT_MAP | EXTENT_BIT_MAP | JOURNAL_BLOCKS | EXTENTS

constexpr uint64_t k_start_padding_block_count = 4;

class PancakeLayout {
public:
    PancakeLayout() = default;
    ~PancakeLayout() = default;

    void BuildLayout(uint64_t device_size);

    PancakeFSLayoutItem SuperBlockLayout() const;
    PancakeFSLayoutItem SuperBlockBackupLayout() const;
    PancakeFSLayoutItem ExtentBitmapLayout() const;
    PancakeFSLayoutItem ExtentsBlocksLayout() const;

private:
    static uint32_t extentBlockCount(uint64_t device_size) ;

private:
    PancakeFSLayoutItem start_padding_{.layout_type_ = LayoutType::PADDING};
    PancakeFSLayoutItem super_block_{.layout_type_ = LayoutType::SUPER_BLOCK};
    PancakeFSLayoutItem super_block_backup_{.layout_type_ = LayoutType::SUPER_BLOCK_BACKUP};
    PancakeFSLayoutItem extent_bit_map_{.layout_type_ = LayoutType::EXTENT_BIT_MAP};
    // PancakeFSLayoutItem trash_extent_bit_map_{.layout_type_ = LayoutType::TRASH_EXTENT_BIT_MAP};
    // PancakeFSLayoutItem journal_blocks_{.layout_type_ = LayoutType::JOURNAL_BLOCKS};
    // PancakeFSLayoutItem bad_block_bit_map_{.layout_type_ = LayoutType::BAD_BLOCK_BIT_MAP};
    PancakeFSLayoutItem padding_blocks_{.layout_type_ = LayoutType::PADDING};
    PancakeFSLayoutItem extent_blocks_{.layout_type_ = LayoutType::EXTENTS};
};

} // namespace pancake_store::datanode::storage