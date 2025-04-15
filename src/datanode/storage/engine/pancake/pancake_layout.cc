
#include "src/datanode/storage/engine/pancake/pancake_layout.h"
#include "src/datanode/storage/engine/pancake/pancake_comm.h"

namespace pancake_store::datanode::storage {

void PancakeLayout::BuildLayout(uint64_t device_size) {
    auto total_device_blocks = device_size / k_block_size;
    auto meta_blocks = k_extent_block_size / k_block_size;

    const auto extent_block_count = extentBlockCount(device_size);

    // start padding
    start_padding_.block_index_ = 0;
    start_padding_.block_count_ = k_start_padding_block_count;

    // super block
    super_block_.block_index_ = start_padding_.block_index_ + start_padding_.block_count_;
    super_block_.block_count_ = 1;

    // super block backup
    super_block_backup_.block_index_ = super_block_.block_index_ + super_block_.block_count_;
    super_block_backup_.block_count_ = 1;

    // extent bitmap blocks
    auto ext_bitmap_block_count = extent_block_count / k_block_data_size / sizeof(uint8_t);
    if (extent_block_count %(k_block_data_size * sizeof(uint8_t)) != 0) {
        ext_bitmap_block_count += 1;
    }
    extent_bit_map_.block_index_ = super_block_backup_.block_index_ + super_block_backup_.block_count_;
    extent_bit_map_.block_count_ = ext_bitmap_block_count;

    // TODO(pancake): trash extent block bitmap
    // TODO(pancake): journal blocks
    // TODO(pancake): bad block bitmap

    // padding blocks
    padding_blocks_.block_index_ = extent_bit_map_.block_index_ + extent_bit_map_.block_count_;
    padding_blocks_.block_count_ = meta_blocks - padding_blocks_.block_index_;

    // extent blocks
    extent_blocks_.block_index_ = padding_blocks_.block_index_ + padding_blocks_.block_count_;
    // NOTICE: per extent block size 64MB
    extent_blocks_.block_count_ = extent_block_count;
}

PancakeFSLayoutItem PancakeLayout::SuperBlockLayout() const {
    return super_block_;
}

PancakeFSLayoutItem PancakeLayout::SuperBlockBackupLayout() const {
    return super_block_backup_;
}

PancakeFSLayoutItem PancakeLayout::ExtentBitmapLayout() const {
    return extent_bit_map_;
}

PancakeFSLayoutItem PancakeLayout::ExtentsBlocksLayout() const {
    return extent_blocks_;
}

uint32_t PancakeLayout::extentBlockCount(const uint64_t device_size) {
    return (device_size / k_block_data_size) - 1;
}

} // namespace pancake_store::datanode::storage