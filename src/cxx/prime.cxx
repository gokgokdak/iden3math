#include <iden3math/prime.h>

namespace iden3math::prime {

const BigInt& bn254() {
    static const BigInt BN254("21888242871839275222246405745257275088548364400416034343698204186575808495617", 10);
    return BN254;
}

} // namespace iden3math::prime
