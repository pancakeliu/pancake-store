
#pragma once

#include "src/datanode/storage/storage.h"

namespace pancake_store::datanode::device {

using pancake_store::datanode::storage::Storage;

class Device {
public:
    Device() = default;
    ~Device() = default;

private:
    // storage member
    Storage storage_;

    // redundant member
};

} //namespace pancake_store::datanode::device