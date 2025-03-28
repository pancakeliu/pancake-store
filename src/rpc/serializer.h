#pragma once

#include <seastar/rpc/rpc.hh>

namespace pancake_store::rpc {

struct Serializer {};

template <typename T, typename Output>
inline void WriteArithmeticType(Output &output, T val) {
    static_assert(std::is_arithmetic_v<T>, "must be arithmetic type");
    return output.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

template <typename T, typename Input>
inline T ReadArithmeticType(Input& input) {
    static_assert(std::is_arithmetic_v<T>, "must be arithmetic type");
    T val;
    input.read(reinterpret_cast<char*>(&val), sizeof(T));
    return val;
}

template <typename Output>
inline void Write(Serializer, Output &output, const seastar::sstring &val) {
    WriteArithmeticType(output, static_cast<uint32_t>(val.size()));
    output.write(val.c_str(), val.size());
}

template <typename Input>
inline seastar::sstring Read(Serializer, Input& input, seastar::rpc::type<seastar::sstring>) {
    auto size = ReadArithmeticType<uint32_t>(input);
    seastar::sstring ret = uninitialized_string(size);
    input.read(ret.data(), size);
    return ret;
}

#define GET_RPC_METHOD_INLINE inline seastar::rpc::protocol<Serializer> &GetRpc() { return rpc_; }

} // namespace pancake_store::rpc