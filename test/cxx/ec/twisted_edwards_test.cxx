#include <iden3math/ec/babyjub.h>
#include <iden3math/ec/twisted_edwards.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>
#include "../helper.h"

namespace iden3math::ec::twisted_edwards {

static const BigInt SCALAR("7120861356467848435263064379192047478074060781135320967663101236819528304084", 10);

namespace {

const Curve& babyjub_curve() {
    static const Curve CURVE(
        babyjub::finite_field(),
        BigInt("168700", 10),
        BigInt("168696", 10)
    );
    return CURVE;
}

const Point& non_subgroup_point() {
    static const Point POINT(
        BigInt("18058203230422821100148902886692797825595047188176647392575344616831548269714", 10),
        BigInt("9711944701593973077501119179165085016629774523576998623469657748778309798227", 10)
    );
    return POINT;
}

std::vector<Point> sample_points() {
    return {
        identity(),
        babyjub::generator(),
        Point(
            BigInt("3598477115375956014038673435118146273990307570718665994596246292010479888129", 10),
            BigInt("10638520112689679830908703045972665394487620627132873410365672297806577997548", 10)
        ),
        Point(
            BigInt("20068488524343428343945823897150776910502466037019831511105395347911200592077", 10),
            BigInt("8654005564599280198548960974184530398968954683306629931317509486422683019120", 10)
        ),
        non_subgroup_point()
    };
}

Point negate_affine(const Curve& curve, const Point& p) {
    return Point(curve.field().neg(p.x), p.y);
}

ExtPoint scale_ext(const Curve& curve, const ExtPoint& p, const BigInt& lambda) {
    const auto& F = curve.field();
    return ExtPoint(
        F.mul(p.X, lambda),
        F.mul(p.Y, lambda),
        F.mul(p.Z, lambda),
        F.mul(p.T, lambda)
    );
}

std::optional<Point> add_formula(const Curve& curve, const Point& a, const Point& b) {
    const auto& F = curve.field();
    const auto x1x2 = F.mul(a.x, b.x);
    const auto y1y2 = F.mul(a.y, b.y);
    const auto x1y2 = F.mul(a.x, b.y);
    const auto y1x2 = F.mul(a.y, b.x);
    const auto dxxyy = F.mul(curve.d(), F.mul(x1x2, y1y2));

    auto x = F.div(F.add(x1y2, y1x2), F.add(1, dxxyy));
    if (std::nullopt == x) {
        return std::nullopt;
    }

    auto y = F.div(F.sub(y1y2, F.mul(curve.a(), x1x2)), F.sub(1, dxxyy));
    if (std::nullopt == y) {
        return std::nullopt;
    }

    return Point(*x, *y);
}

std::optional<Point> mul_scalar_formula(const Curve& curve, const Point& p, int32_t k) {
    Point acc = identity();
    Point addend = p;
    int32_t count = k;

    if (count < 0) {
        addend = negate_affine(curve, p);
        count = -count;
    }

    for (int32_t i = 0; i < count; ++i) {
        auto next = add_formula(curve, acc, addend);
        if (std::nullopt == next) {
            return std::nullopt;
        }
        acc = *next;
    }
    return acc;
}

} // namespace

TEST(twisted_edwards, identity_roundtrip) {
    const auto& c = babyjub_curve();

    EXPECT_TRUE(in_curve(c, identity()));
    EXPECT_TRUE(in_curve(c, ext_identity()));

    auto affine = to_affine(c, ext_identity());
    ASSERT_NE(affine, std::nullopt);
    EXPECT_EQ(*affine, identity());
}

TEST(twisted_edwards, affine_and_extended_roundtrip_preserve_curve_membership) {
    const auto& c = babyjub_curve();

    for (const auto& p : sample_points()) {
        SCOPED_TRACE_POINT(p, 10)

        EXPECT_TRUE(in_curve(c, p));

        auto canonical_ext = to_ext(c, p);
        auto scaled_ext = scale_ext(c, canonical_ext, 7);

        EXPECT_TRUE(in_curve(c, canonical_ext));
        EXPECT_TRUE(in_curve(c, scaled_ext));
        EXPECT_TRUE(equivalent(c, canonical_ext, scaled_ext));
        EXPECT_TRUE(equivalent(c, scaled_ext, canonical_ext));

        auto affine = to_affine(c, canonical_ext);
        ASSERT_NE(affine, std::nullopt);
        EXPECT_EQ(*affine, p);

        auto scaled_affine = to_affine(c, scaled_ext);
        ASSERT_NE(scaled_affine, std::nullopt);
        EXPECT_EQ(*scaled_affine, p);
    }
}

TEST(twisted_edwards, equivalent_matches_projective_scaling_only) {
    const auto& c = babyjub_curve();
    auto generator_ext = to_ext(c, babyjub::generator());
    auto generator_scaled = scale_ext(c, generator_ext, 7);
    auto generator_scaled_again = scale_ext(c, generator_ext, 11);
    auto other_ext = to_ext(c, non_subgroup_point());

    EXPECT_TRUE(equivalent(c, generator_ext, generator_scaled));
    EXPECT_TRUE(equivalent(c, generator_scaled, generator_scaled_again));
    EXPECT_FALSE(equivalent(c, generator_ext, other_ext));
}

TEST(twisted_edwards, add_matches_affine_formula_and_group_laws) {
    const auto& c = babyjub_curve();
    const auto& F = c.field();
    const auto points = sample_points();

    for (const auto& p : points) {
        SCOPED_TRACE_POINT(p, 10)

        auto neg_p = negate_affine(c, p);
        EXPECT_TRUE(in_curve(c, neg_p));
        EXPECT_EQ(add(c, identity(), p), p);
        EXPECT_EQ(add(c, p, identity()), p);
        EXPECT_EQ(add(c, p, neg_p), identity());
        EXPECT_EQ(add(c, neg_p, p), identity());
        EXPECT_EQ(neg_p.x, F.neg(p.x));
        EXPECT_EQ(neg_p.y, p.y);
    }

    for (const auto& a : points) {
        for (const auto& b : points) {
            SCOPED_TRACE_POINT(a, 10)
            SCOPED_TRACE_POINT(b, 10)

            auto expected = add_formula(c, a, b);
            ASSERT_NE(expected, std::nullopt);

            auto sum_ab = add(c, a, b);
            auto sum_ba = add(c, b, a);

            EXPECT_EQ(sum_ab, *expected);
            EXPECT_EQ(sum_ab, sum_ba);
            EXPECT_TRUE(in_curve(c, sum_ab));
        }
    }

    for (const auto& a : points) {
        for (const auto& b : points) {
            for (const auto& d : points) {
                SCOPED_TRACE_POINT(a, 10)
                SCOPED_TRACE_POINT(b, 10)
                SCOPED_TRACE_POINT(d, 10)

                auto lhs = add(c, add(c, a, b), d);
                auto rhs = add(c, a, add(c, b, d));
                EXPECT_EQ(lhs, rhs);
            }
        }
    }
}

TEST(twisted_edwards, extended_add_and_double_match_affine_formula) {
    const auto& c = babyjub_curve();
    const auto points = sample_points();

    for (const auto& a : points) {
        SCOPED_TRACE_POINT(a, 10)

        auto expected_dbl = add_formula(c, a, a);
        ASSERT_NE(expected_dbl, std::nullopt);

        auto dbl_affine = dbl(c, a);
        EXPECT_EQ(dbl_affine, *expected_dbl);

        auto dbl_ext = dbl(c, scale_ext(c, to_ext(c, a), 7));
        EXPECT_TRUE(in_curve(c, dbl_ext));
        EXPECT_TRUE(equivalent(c, dbl_ext, to_ext(c, *expected_dbl)));

        auto dbl_roundtrip = to_affine(c, dbl_ext);
        ASSERT_NE(dbl_roundtrip, std::nullopt);
        EXPECT_EQ(*dbl_roundtrip, *expected_dbl);
    }

    for (const auto& a : points) {
        for (const auto& b : points) {
            SCOPED_TRACE_POINT(a, 10)
            SCOPED_TRACE_POINT(b, 10)

            auto expected = add_formula(c, a, b);
            ASSERT_NE(expected, std::nullopt);

            auto a_ext = scale_ext(c, to_ext(c, a), 7);
            auto b_ext = scale_ext(c, to_ext(c, b), 11);
            auto sum_ext = add(c, a_ext, b_ext);

            EXPECT_TRUE(in_curve(c, sum_ext));
            EXPECT_TRUE(equivalent(c, sum_ext, to_ext(c, *expected)));

            auto roundtrip = to_affine(c, sum_ext);
            ASSERT_NE(roundtrip, std::nullopt);
            EXPECT_EQ(*roundtrip, *expected);
        }
    }
}

TEST(twisted_edwards, mul_scalar_matches_repeated_affine_formula_for_small_scalars) {
    const auto& c = babyjub_curve();
    std::vector<Point> bases = {
        babyjub::generator(),
        sample_points()[2],
        non_subgroup_point()
    };

    for (const auto& base : bases) {
        SCOPED_TRACE_POINT(base, 10)
        auto base_ext = scale_ext(c, to_ext(c, base), 9);

        for (int32_t k = -32; k <= 32; ++k) {
            SCOPED_TRACE(k);

            auto expected = mul_scalar_formula(c, base, k);
            ASSERT_NE(expected, std::nullopt);

            auto affine = mul_scalar(c, base, k);
            EXPECT_EQ(affine, *expected);

            auto ext = mul_scalar(c, base_ext, k);
            EXPECT_TRUE(in_curve(c, ext));

            auto roundtrip = to_affine(c, ext);
            ASSERT_NE(roundtrip, std::nullopt);
            EXPECT_EQ(*roundtrip, *expected);
        }
    }
}

TEST(twisted_edwards, mul_scalar_respects_group_orders_and_scalar_addition) {
    const auto& c = babyjub_curve();
    const auto g = babyjub::generator();
    const auto g_subgroup = mul_scalar(c, g, 8);
    const auto p = non_subgroup_point();

    EXPECT_EQ(mul_scalar(c, g, 0), identity());
    EXPECT_EQ(mul_scalar(c, g, 1), g);
    EXPECT_EQ(mul_scalar(c, g, babyjub::group_order()), identity());
    EXPECT_EQ(mul_scalar(c, g_subgroup, babyjub::sub_group_order()), identity());
    EXPECT_NE(mul_scalar(c, g, babyjub::sub_group_order()), identity());
    EXPECT_EQ(mul_scalar(c, p, babyjub::group_order()), identity());
    EXPECT_NE(mul_scalar(c, p, babyjub::sub_group_order()), identity());

    auto lhs = mul_scalar(c, g, SCALAR + 19);
    auto rhs_left = mul_scalar(c, g, SCALAR);
    auto rhs_right = mul_scalar(c, g, 19);
    auto rhs = add_formula(c, rhs_left, rhs_right);
    ASSERT_NE(rhs, std::nullopt);
    EXPECT_EQ(lhs, *rhs);
}

TEST(twisted_edwards, invalid_affine_and_extended_points_are_rejected) {
    const auto& c = babyjub_curve();
    std::vector<Point> invalid_points = {
        Point(0, 0),
        Point(1, 1),
        Point(2, 2),
        Point(16, 16),
        Point(-1, -1),
        Point(-2, -2),
        Point(-1, 16),
        Point(babyjub::prime(), babyjub::prime()),
        Point(1, babyjub::prime()),
        Point(babyjub::prime() + 1, babyjub::prime()),
        Point(babyjub::prime() - 1, babyjub::prime())
    };

    for (const auto& p : invalid_points) {
        SCOPED_TRACE_POINT(p, 10)
        EXPECT_FALSE(in_curve(c, p));
    }

    ExtPoint invalid_z(1, 1, 0, 1);
    EXPECT_FALSE(in_curve(c, invalid_z));
    EXPECT_EQ(to_affine(c, invalid_z), std::nullopt);
    EXPECT_FALSE(equivalent(c, invalid_z, ext_identity()));

    auto valid = to_ext(c, babyjub::generator());
    ExtPoint invalid_t(valid.X, valid.Y, valid.Z, valid.T + 1);
    EXPECT_FALSE(in_curve(c, invalid_t));
}

} // namespace iden3math::ec::twisted_edwards
