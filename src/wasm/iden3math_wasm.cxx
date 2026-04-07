#ifdef IDEN3MATH_BUILD_WASM

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <iden3math/bitvec.h>
#include <iden3math/ec/babyjub.h>
#include <iden3math/ec/twisted_edwards.h>
#include <iden3math/fp1.h>
#include <iden3math/hash/blake.h>
#include <iden3math/hash/keccak.h>
#include <iden3math/hash/mimc.h>
#include <iden3math/hash/pedersen.h>
#include <iden3math/prime.h>
#include <iden3math/random.h>
#include <optional>
#include <string>

using namespace emscripten;
using namespace iden3math;

namespace {

BigInt parse_bigint(const std::string& value) {
    return {value, 10};
}

std::string to_decimal_string(const BigInt& value) {
    return value.str(10);
}

Endian to_endian(int value) {
    return static_cast<Endian>(value);
}

ByteVec1D from_uint8_array(const val& bytes) {
    const auto length = bytes["length"].as<unsigned>();
    ByteVec1D out(length);
    for (unsigned i = 0; i < length; ++i) {
        out[i] = bytes[i].as<Byte>();
    }
    return out;
}

val to_uint8_array(const ByteVec1D& bytes) {
    auto array = val::global("Uint8Array").new_(bytes.size());
    for (size_t i = 0; i < bytes.size(); ++i) {
        array.set(i, val(bytes[i]));
    }
    return array;
}

ByteVec2D from_uint8_array_list(const val& preimages) {
    const auto length = preimages["length"].as<unsigned>();
    ByteVec2D out;
    out.reserve(length);
    for (unsigned i = 0; i < length; ++i) {
        out.emplace_back(from_uint8_array(preimages[i]));
    }
    return out;
}

val to_uint8_array_list(const ByteVec2D& values) {
    auto array = val::array();
    for (const auto& value : values) {
        array.call<void>("push", to_uint8_array(value));
    }
    return array;
}

ec::Point point_from_js(const val& point) {
    return ec::Point(
        parse_bigint(point["x"].as<std::string>()),
        parse_bigint(point["y"].as<std::string>())
    );
}

val point_to_js(const ec::Point& point) {
    auto obj = val::object();
    obj.set("x", to_decimal_string(point.x));
    obj.set("y", to_decimal_string(point.y));
    return obj;
}

ec::twisted_edwards::ExtPoint ext_point_from_js(const val& point) {
    return ec::twisted_edwards::ExtPoint(
        parse_bigint(point["X"].as<std::string>()),
        parse_bigint(point["Y"].as<std::string>()),
        parse_bigint(point["Z"].as<std::string>()),
        parse_bigint(point["T"].as<std::string>())
    );
}

val ext_point_to_js(const ec::twisted_edwards::ExtPoint& point) {
    auto obj = val::object();
    obj.set("X", to_decimal_string(point.X));
    obj.set("Y", to_decimal_string(point.Y));
    obj.set("Z", to_decimal_string(point.Z));
    obj.set("T", to_decimal_string(point.T));
    return obj;
}

val optional_bigint_to_js(const std::optional<BigInt>& value) {
    if (!value) {
        return val::null();
    }
    return val(to_decimal_string(*value));
}

val optional_point_to_js(const std::optional<ec::Point>& value) {
    if (!value) {
        return val::null();
    }
    return point_to_js(*value);
}

class WasmBitVec1D final {
public:
    WasmBitVec1D() = default;

    [[nodiscard]] size_t size() const {
        return bitvec_.size();
    }

    void push(bool bit) {
        bitvec_.push(bit);
    }

    [[nodiscard]] bool get(size_t index) const {
        return bitvec_[index];
    }

private:
    BitVec1D bitvec_;
};

class WasmFp1 final {
public:
    explicit WasmFp1(const std::string& prime)
        : prime_(parse_bigint(prime)), field_(prime_) {}

    [[nodiscard]] std::string prime() const {
        return to_decimal_string(prime_);
    }

    [[nodiscard]] std::string mod_reduce(const std::string& a) const {
        return to_decimal_string(field_.mod_reduce(parse_bigint(a)));
    }

    [[nodiscard]] std::string add(const std::string& a, const std::string& b) const {
        return to_decimal_string(field_.add(parse_bigint(a), parse_bigint(b)));
    }

    [[nodiscard]] std::string sub(const std::string& a, const std::string& b) const {
        return to_decimal_string(field_.sub(parse_bigint(a), parse_bigint(b)));
    }

    [[nodiscard]] std::string mul(const std::string& a, const std::string& b) const {
        return to_decimal_string(field_.mul(parse_bigint(a), parse_bigint(b)));
    }

    [[nodiscard]] val div(const std::string& a, const std::string& b) const {
        return optional_bigint_to_js(field_.div(parse_bigint(a), parse_bigint(b)));
    }

    [[nodiscard]] val pow(const std::string& a, const std::string& b) const {
        return optional_bigint_to_js(field_.pow(parse_bigint(a), parse_bigint(b)));
    }

    [[nodiscard]] std::string square(const std::string& a) const {
        return to_decimal_string(field_.square(parse_bigint(a)));
    }

    [[nodiscard]] val sqrt(const std::string& a) const {
        return optional_bigint_to_js(field_.sqrt(parse_bigint(a)));
    }

    [[nodiscard]] val mod_inv(const std::string& a) const {
        return optional_bigint_to_js(field_.mod_inv(parse_bigint(a)));
    }

    [[nodiscard]] std::string neg(const std::string& a) const {
        return to_decimal_string(field_.neg(parse_bigint(a)));
    }

    [[nodiscard]] bool has_sqrt(const std::string& a) const {
        return field_.has_sqrt(parse_bigint(a));
    }

    [[nodiscard]] const Fp1& field() const {
        return field_;
    }

private:
    BigInt prime_;
    Fp1 field_;
};

class WasmCurve final {
public:
    WasmCurve(const std::string& field_prime, const std::string& a, const std::string& d)
        : field_(field_prime),
          a_(parse_bigint(a)),
          d_(parse_bigint(d)),
          curve_(field_.field(), a_, d_) {}

    [[nodiscard]] std::string field_prime() const {
        return field_.prime();
    }

    [[nodiscard]] std::string a() const {
        return to_decimal_string(a_);
    }

    [[nodiscard]] std::string d() const {
        return to_decimal_string(d_);
    }

    [[nodiscard]] const ec::twisted_edwards::Curve& curve() const {
        return curve_;
    }

private:
    friend val twisted_edwards_to_ext(const WasmCurve&, const val&);
    friend val twisted_edwards_to_affine(const WasmCurve&, const val&);
    friend bool twisted_edwards_in_curve_affine(const WasmCurve&, const val&);
    friend bool twisted_edwards_in_curve_ext(const WasmCurve&, const val&);
    friend bool twisted_edwards_equivalent(const WasmCurve&, const val&, const val&);
    friend val twisted_edwards_add_affine(const WasmCurve&, const val&, const val&);
    friend val twisted_edwards_add_ext(const WasmCurve&, const val&, const val&);
    friend val twisted_edwards_dbl_affine(const WasmCurve&, const val&);
    friend val twisted_edwards_dbl_ext(const WasmCurve&, const val&);
    friend val twisted_edwards_mul_scalar_affine(const WasmCurve&, const val&, const std::string&);
    friend val twisted_edwards_mul_scalar_ext(const WasmCurve&, const val&, const std::string&);

    WasmFp1 field_;
    BigInt a_;
    BigInt d_;
    ec::twisted_edwards::Curve curve_;
};

std::string prime_bn254() {
    return to_decimal_string(prime::bn254());
}

val random_get_bytes(size_t len) {
    return to_uint8_array(random::get_bytes(len));
}

std::string random_get_integer(const std::string& p) {
    return to_decimal_string(random::get_integer(parse_bigint(p)));
}

val hash_blake256_bytes(const val& bytes) {
    return to_uint8_array(hash::blake256(from_uint8_array(bytes)));
}

val hash_blake256_text(const std::string& text) {
    return to_uint8_array(hash::blake256(text));
}

val hash_keccak256_bytes(const val& bytes) {
    ByteVec1D digest;
    hash::keccak256(from_uint8_array(bytes), digest);
    return to_uint8_array(digest);
}

val hash_keccak256_text(const std::string& text) {
    ByteVec1D digest;
    hash::keccak256(text, digest);
    return to_uint8_array(digest);
}

val hash_keccak256_bigint(const std::string& number) {
    ByteVec1D digest;
    hash::keccak256(parse_bigint(number), digest);
    return to_uint8_array(digest);
}

val hash_pedersen(const val& preimage) {
    ByteVec1D digest;
    hash::pedersen(from_uint8_array(preimage), digest);
    return to_uint8_array(digest);
}

val hash_mimc_sponge(const val& preimages, size_t outputs, const val& key, int preimage_endian, int key_endian, int digest_endian) {
    ByteVec2D digests;
    hash::mimc_sponge(
        from_uint8_array_list(preimages),
        outputs,
        from_uint8_array(key),
        digests,
        to_endian(preimage_endian),
        to_endian(key_endian),
        to_endian(digest_endian)
    );
    return to_uint8_array_list(digests);
}

std::string babyjub_prime() {
    return to_decimal_string(ec::babyjub::prime());
}

std::string babyjub_group_order() {
    return to_decimal_string(ec::babyjub::group_order());
}

std::string babyjub_sub_group_order() {
    return to_decimal_string(ec::babyjub::sub_group_order());
}

val babyjub_zero() {
    return point_to_js(ec::babyjub::zero());
}

val babyjub_generator() {
    return point_to_js(ec::babyjub::generator());
}

val babyjub_add(const val& a, const val& b) {
    return point_to_js(ec::babyjub::add(point_from_js(a), point_from_js(b)));
}

val babyjub_mul_scalar(const val& point, const std::string& scalar) {
    return point_to_js(ec::babyjub::mul_scalar(point_from_js(point), parse_bigint(scalar)));
}

bool babyjub_in_sub_group(const val& point) {
    return ec::babyjub::in_sub_group(point_from_js(point));
}

bool babyjub_in_curve(const val& point) {
    return ec::babyjub::in_curve(point_from_js(point));
}

val babyjub_compress(const val& point, int endian) {
    return to_uint8_array(ec::babyjub::compress(point_from_js(point), to_endian(endian)));
}

val babyjub_decompress(const val& packed, int endian) {
    return optional_point_to_js(ec::babyjub::decompress(from_uint8_array(packed), to_endian(endian)));
}

val twisted_edwards_identity() {
    return point_to_js(ec::twisted_edwards::identity());
}

val twisted_edwards_ext_identity() {
    return ext_point_to_js(ec::twisted_edwards::ext_identity());
}

val twisted_edwards_to_ext(const WasmCurve& curve, const val& point) {
    return ext_point_to_js(ec::twisted_edwards::to_ext(curve.curve_, point_from_js(point)));
}

val twisted_edwards_to_affine(const WasmCurve& curve, const val& point) {
    return optional_point_to_js(ec::twisted_edwards::to_affine(curve.curve_, ext_point_from_js(point)));
}

bool twisted_edwards_in_curve_affine(const WasmCurve& curve, const val& point) {
    return ec::twisted_edwards::in_curve(curve.curve_, point_from_js(point));
}

bool twisted_edwards_in_curve_ext(const WasmCurve& curve, const val& point) {
    return ec::twisted_edwards::in_curve(curve.curve_, ext_point_from_js(point));
}

bool twisted_edwards_equivalent(const WasmCurve& curve, const val& a, const val& b) {
    return ec::twisted_edwards::equivalent(curve.curve_, ext_point_from_js(a), ext_point_from_js(b));
}

val twisted_edwards_add_affine(const WasmCurve& curve, const val& a, const val& b) {
    return point_to_js(ec::twisted_edwards::add(curve.curve_, point_from_js(a), point_from_js(b)));
}

val twisted_edwards_add_ext(const WasmCurve& curve, const val& a, const val& b) {
    return ext_point_to_js(ec::twisted_edwards::add(curve.curve_, ext_point_from_js(a), ext_point_from_js(b)));
}

val twisted_edwards_dbl_affine(const WasmCurve& curve, const val& point) {
    return point_to_js(ec::twisted_edwards::dbl(curve.curve_, point_from_js(point)));
}

val twisted_edwards_dbl_ext(const WasmCurve& curve, const val& point) {
    return ext_point_to_js(ec::twisted_edwards::dbl(curve.curve_, ext_point_from_js(point)));
}

val twisted_edwards_mul_scalar_affine(const WasmCurve& curve, const val& point, const std::string& scalar) {
    return point_to_js(ec::twisted_edwards::mul_scalar(curve.curve_, point_from_js(point), parse_bigint(scalar)));
}

val twisted_edwards_mul_scalar_ext(const WasmCurve& curve, const val& point, const std::string& scalar) {
    return ext_point_to_js(ec::twisted_edwards::mul_scalar(curve.curve_, ext_point_from_js(point), parse_bigint(scalar)));
}

} // namespace

EMSCRIPTEN_BINDINGS(iden3math_wasm) {
    enum_<Endian>("Endian")
        .value("LE", LE)
        .value("BE", BE);

    class_<WasmBitVec1D>("RawBitVec1D")
        .constructor<>()
        .function("size", &WasmBitVec1D::size)
        .function("push", &WasmBitVec1D::push)
        .function("get", &WasmBitVec1D::get);

    class_<WasmFp1>("RawFp1")
        .constructor<const std::string&>()
        .function("prime", &WasmFp1::prime)
        .function("mod_reduce", &WasmFp1::mod_reduce)
        .function("add", &WasmFp1::add)
        .function("sub", &WasmFp1::sub)
        .function("mul", &WasmFp1::mul)
        .function("div", &WasmFp1::div)
        .function("pow", &WasmFp1::pow)
        .function("square", &WasmFp1::square)
        .function("sqrt", &WasmFp1::sqrt)
        .function("mod_inv", &WasmFp1::mod_inv)
        .function("neg", &WasmFp1::neg)
        .function("has_sqrt", &WasmFp1::has_sqrt);

    class_<WasmCurve>("RawCurve")
        .constructor<const std::string&, const std::string&, const std::string&>()
        .function("field_prime", &WasmCurve::field_prime)
        .function("a", &WasmCurve::a)
        .function("d", &WasmCurve::d);

    function("prime_bn254", &prime_bn254);
    function("random_get_bytes", &random_get_bytes);
    function("random_get_integer", &random_get_integer);
    function("hash_blake256_bytes", &hash_blake256_bytes);
    function("hash_blake256_text", &hash_blake256_text);
    function("hash_keccak256_bytes", &hash_keccak256_bytes);
    function("hash_keccak256_text", &hash_keccak256_text);
    function("hash_keccak256_bigint", &hash_keccak256_bigint);
    function("hash_pedersen", &hash_pedersen);
    function("hash_mimc_sponge", &hash_mimc_sponge);
    function("babyjub_prime", &babyjub_prime);
    function("babyjub_group_order", &babyjub_group_order);
    function("babyjub_sub_group_order", &babyjub_sub_group_order);
    function("babyjub_zero", &babyjub_zero);
    function("babyjub_generator", &babyjub_generator);
    function("babyjub_add", &babyjub_add);
    function("babyjub_mul_scalar", &babyjub_mul_scalar);
    function("babyjub_in_sub_group", &babyjub_in_sub_group);
    function("babyjub_in_curve", &babyjub_in_curve);
    function("babyjub_compress", &babyjub_compress);
    function("babyjub_decompress", &babyjub_decompress);
    function("twisted_edwards_identity", &twisted_edwards_identity);
    function("twisted_edwards_ext_identity", &twisted_edwards_ext_identity);
    function("twisted_edwards_to_ext", &twisted_edwards_to_ext);
    function("twisted_edwards_to_affine", &twisted_edwards_to_affine);
    function("twisted_edwards_in_curve_affine", &twisted_edwards_in_curve_affine);
    function("twisted_edwards_in_curve_ext", &twisted_edwards_in_curve_ext);
    function("twisted_edwards_equivalent", &twisted_edwards_equivalent);
    function("twisted_edwards_add_affine", &twisted_edwards_add_affine);
    function("twisted_edwards_add_ext", &twisted_edwards_add_ext);
    function("twisted_edwards_dbl_affine", &twisted_edwards_dbl_affine);
    function("twisted_edwards_dbl_ext", &twisted_edwards_dbl_ext);
    function("twisted_edwards_mul_scalar_affine", &twisted_edwards_mul_scalar_affine);
    function("twisted_edwards_mul_scalar_ext", &twisted_edwards_mul_scalar_ext);
}

#endif
