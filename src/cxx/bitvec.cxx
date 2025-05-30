#include <iden3math/bitvec.h>
#include <stdexcept>

namespace iden3math {

BitVec1D::BitVec1D() : size_(0) {
    bits_.push_back(0x00);
}

size_t BitVec1D::size() const {
    return size_;
}

void BitVec1D::push(Bit bit) {
    auto idx_in_vec = size_ / 8;
    auto idx_in_byte = size_ % 8;
    bits_[idx_in_vec] |= (static_cast<Byte>(bit ? 0x01 : 0x00) << idx_in_byte);
    ++size_;
    if (size_ % 8 == 0) {
        bits_.push_back(0x00);
    }
}

Bit BitVec1D::operator[](size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    auto idx_in_vec = index / 8;
    auto idx_in_byte = index % 8;
    return (bits_[idx_in_vec] >> idx_in_byte) & 0x01;
}

} // namespace iden3math
