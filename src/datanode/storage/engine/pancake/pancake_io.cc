
#include "src/datanode/storage/engine/pancake/pancake_io.h"
#include "src/datanode/storage/engine/pancake/pancake_fs_block.h"

#include <seastar/core/file.hh>

namespace pancake_store::datanode::storage {

seastar::future<ErrorCode> PancakeIO::Init(const seastar::sstring &device_path, DeviceId device_id) {
    return seastar::open_file_dma(device_path, seastar::open_flags::rw | seastar::open_flags::dsync)
        .then([this, device_path, device_id](seastar::file device_fd) {
            device_fd_ = std::make_shared<seastar::file>(std::move(device_fd));
            device_path_ = device_path;
            device_id_ = device_id;

            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
        })
        .handle_exception([this, device_path](std::exception_ptr ex) {
            logger_.error("open device:{}, err:{}", device_path, ex);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_DEVICE_OPEN_FAILED);
        });
}

seastar::future<ErrorCode> PancakeIO::ReadAt(uint64_t offset, uint64_t read_len, seastar::sstring *read_buf) {
    if (!PancakeComm::IsBlockAlign(offset) || !PancakeComm::IsBlockAlign(read_len)) {
        logger_.error("code error!! offset:{} or read_len:{} not block aligned", offset, read_len);
        return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_NOT_BLOCK_ALIGNED);
    }

    return device_fd_->dma_read_exactly<char>(offset, read_len)
        .then([this, read_buf](seastar::temporary_buffer<char> buf) {
            // TODO: zero copy
            read_buf->append(buf.get(), buf.size());
            return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
        })
        .handle_exception([this, read_buf](std::exception_ptr ex) {
            logger_.error("read_at got exception:{}, device:{}", ex, device_path_);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_IO_READ_AT_FAILED);
        });
}

seastar::future<ErrorCode> PancakeIO::WriteAt(uint64_t offset, const seastar::sstring &write_buf) {
    if (!IsBlockAlign(offset) || !IsBlockAlign(write_buf.size())) {
        logger_.error("code error!! offset:{} or write_buf:{} not block aligned.",
            offset, write_buf.size());
        return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_NOT_BLOCK_ALIGNED);
    }

    return device_fd_->dma_write(offset, write_buf.data(), write_buf.size())
        .then([this, write_buf](size_t len) {
            if (len == write_buf.size()) {
                return seastar::make_ready_future<ErrorCode>(ErrorCode::PANCAKE_STORE_OK);
            }

            logger_.error("write_at failed. written:{}, buf_size:{}", len, write_buf.size());
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_IO_WRITE_AT_FAILED);
        })
        .handle_exception([this](std::exception_ptr ex) {
            logger_.error("write_at got exception:{}, device:{}", ex, device_path_);
            return seastar::make_ready_future<ErrorCode>(ErrorCode::DATANODE_STORAGE_IO_WRITE_AT_FAILED);
        });
}

} // namespace pancake_store::datanode::storage