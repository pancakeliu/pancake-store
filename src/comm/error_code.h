#pragma once

#include "proto/error_code.pb.h"

namespace pancake_store::comm {

using ErrorCode = proto::common::ErrorCode;

using proto::common::ErrorCode_Name;

inline std::string ErrorCodeName(const ErrorCode code) {
    return ErrorCode_Name(code);
}

inline bool IsOk(const ErrorCode code) {
    return code == ErrorCode::PANCAKE_STORE_OK;
}

inline bool IsNotOk(const ErrorCode code) {
    return code != ErrorCode::PANCAKE_STORE_OK;
}

} // namespace pancake_store::comm