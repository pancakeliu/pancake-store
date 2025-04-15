
#include "src/datanode/storage/engine/pancake/pancake_comm.h"

namespace pancake_store::datanode::storage {

inline bool PancakeComm::IsBlockAlign(const DeviceOffset val) {
    return val % k_block_size == 0;
}

} // namespace pancake_store::datanode::storage