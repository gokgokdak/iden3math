#pragma once

#include <iden3math/macro.h>
#include <iden3math/typedef.h>

namespace iden3math {

class API BitVec1D final {
public:
    BitVec1D();
    ~BitVec1D() = default;

public:
    [[nodiscard]] size_t size() const;
    void push(Bit bit);
    Bit operator[](size_t index) const;

private:
    /** Storage Map
     *
     *  (Low Addr)                 (High Addr)
     * {0x[first_bit], 0xff, 0xff, 0x[last_bit]}
     *
     * 0xff = (last_bit) 1 1 1 1 1 1 1 1 (first_bit)
     * first_bit = 0xff & 0x01
     * last_bit = 0xff & 0x80
     */
    ByteVec1D bits_;
    size_t size_; // How many bits
};

} // namespace iden3math