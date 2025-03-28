
#pragma once

namespace pancake_store::comm {

class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

} // namestore pancake_store::comm