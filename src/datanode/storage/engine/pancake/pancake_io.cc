
#include "src/datanode/storage/engine/pancake/pancake_io.h"

#include <seastar/core/coroutine.hh>

namespace pancake_store::datanode::storage {

seastar::future<int> test() {
    auto ret = co_await seastar::open_file_dma("", seastar::open_flags::rw);
    co_return 1;
}

seastar::future<ErrorCode> PancakeIO::Init(const seastar::sstring &device_path, DeviceId device_id) {

}

} // namespace pancake_store::datanode::storage