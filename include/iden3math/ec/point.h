#pragma once

#include <iden3math/bigint.h>
#include <iden3math/macro.h>
#include <utility>

namespace iden3math::ec {

class API Point final { // Affine coordinates
public:
    explicit Point(BigInt x = 0, BigInt y = 0) : x(std::move(x)), y(std::move(y)) {}
    ~Point() = default;
    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point& p) const { return x != p.x && y != p.y; }

public:
    BigInt x;
    BigInt y;
};

} // namespace iden3math::ec
