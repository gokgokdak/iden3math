import unittest

from iden3math import ec


class TestTwistedEdwards(unittest.TestCase):

    def setUp(self):
        self.scalar = 7120861356467848435263064379192047478074060781135320967663101236819528304084

    def babyjub_curve(self):
        return ec.twisted_edwards.Curve(ec.babyjub.finite_field(), 168700, 168696)

    def non_subgroup_point(self):
        return ec.Point(
            18058203230422821100148902886692797825595047188176647392575344616831548269714,
            9711944701593973077501119179165085016629774523576998623469657748778309798227
        )

    def sample_points(self):
        return [
            ec.twisted_edwards.identity(),
            ec.babyjub.generator(),
            ec.Point(
                3598477115375956014038673435118146273990307570718665994596246292010479888129,
                10638520112689679830908703045972665394487620627132873410365672297806577997548
            ),
            ec.Point(
                20068488524343428343945823897150776910502466037019831511105395347911200592077,
                8654005564599280198548960974184530398968954683306629931317509486422683019120
            ),
            self.non_subgroup_point(),
        ]

    def negate_affine(self, curve, p):
        return ec.Point(curve.field().neg(p.x()), p.y())

    def scale_ext(self, curve, p, lam):
        field = curve.field()
        return ec.twisted_edwards.ExtPoint(
            field.mul(p.X(), lam),
            field.mul(p.Y(), lam),
            field.mul(p.Z(), lam),
            field.mul(p.T(), lam),
        )

    def add_formula(self, curve, a, b):
        field = curve.field()
        x1x2 = field.mul(a.x(), b.x())
        y1y2 = field.mul(a.y(), b.y())
        x1y2 = field.mul(a.x(), b.y())
        y1x2 = field.mul(a.y(), b.x())
        dxxyy = field.mul(curve.d(), field.mul(x1x2, y1y2))

        x = field.div(field.add(x1y2, y1x2), field.add(1, dxxyy))
        if x is None:
            return None

        y = field.div(field.sub(y1y2, field.mul(curve.a(), x1x2)), field.sub(1, dxxyy))
        if y is None:
            return None

        return ec.Point(x, y)

    def mul_scalar_formula(self, curve, p, k):
        acc = ec.twisted_edwards.identity()
        addend = p
        count = k

        if count < 0:
            addend = self.negate_affine(curve, p)
            count = -count

        for _ in range(count):
            acc = self.add_formula(curve, acc, addend)
            if acc is None:
                return None
        return acc

    def test_identity_roundtrip(self):
        curve = self.babyjub_curve()

        self.assertTrue(ec.twisted_edwards.in_curve(curve, ec.twisted_edwards.identity()))
        self.assertTrue(ec.twisted_edwards.in_curve(curve, ec.twisted_edwards.ext_identity()))

        affine = ec.twisted_edwards.to_affine(curve, ec.twisted_edwards.ext_identity())
        self.assertIsNotNone(affine)
        self.assertEqual(affine, ec.twisted_edwards.identity())

    def test_affine_and_extended_roundtrip_preserve_curve_membership(self):
        curve = self.babyjub_curve()

        for p in self.sample_points():
            with self.subTest(point=(p.x(), p.y())):
                self.assertTrue(ec.twisted_edwards.in_curve(curve, p))

                canonical_ext = ec.twisted_edwards.to_ext(curve, p)
                scaled_ext = self.scale_ext(curve, canonical_ext, 7)

                self.assertTrue(ec.twisted_edwards.in_curve(curve, canonical_ext))
                self.assertTrue(ec.twisted_edwards.in_curve(curve, scaled_ext))
                self.assertTrue(ec.twisted_edwards.equivalent(curve, canonical_ext, scaled_ext))
                self.assertTrue(ec.twisted_edwards.equivalent(curve, scaled_ext, canonical_ext))

                affine = ec.twisted_edwards.to_affine(curve, canonical_ext)
                self.assertIsNotNone(affine)
                self.assertEqual(affine, p)

                scaled_affine = ec.twisted_edwards.to_affine(curve, scaled_ext)
                self.assertIsNotNone(scaled_affine)
                self.assertEqual(scaled_affine, p)

    def test_equivalent_matches_projective_scaling_only(self):
        curve = self.babyjub_curve()
        generator_ext = ec.twisted_edwards.to_ext(curve, ec.babyjub.generator())
        generator_scaled = self.scale_ext(curve, generator_ext, 7)
        generator_scaled_again = self.scale_ext(curve, generator_ext, 11)
        other_ext = ec.twisted_edwards.to_ext(curve, self.non_subgroup_point())

        self.assertTrue(ec.twisted_edwards.equivalent(curve, generator_ext, generator_scaled))
        self.assertTrue(ec.twisted_edwards.equivalent(curve, generator_scaled, generator_scaled_again))
        self.assertFalse(ec.twisted_edwards.equivalent(curve, generator_ext, other_ext))

    def test_add_matches_affine_formula_and_group_laws(self):
        curve = self.babyjub_curve()
        field = curve.field()
        points = self.sample_points()

        for p in points:
            with self.subTest(identity_and_inverse=(p.x(), p.y())):
                neg_p = self.negate_affine(curve, p)
                self.assertTrue(ec.twisted_edwards.in_curve(curve, neg_p))
                self.assertEqual(ec.twisted_edwards.add(curve, ec.twisted_edwards.identity(), p), p)
                self.assertEqual(ec.twisted_edwards.add(curve, p, ec.twisted_edwards.identity()), p)
                self.assertEqual(ec.twisted_edwards.add(curve, p, neg_p), ec.twisted_edwards.identity())
                self.assertEqual(ec.twisted_edwards.add(curve, neg_p, p), ec.twisted_edwards.identity())
                self.assertEqual(neg_p.x(), field.neg(p.x()))
                self.assertEqual(neg_p.y(), p.y())

        for a in points:
            for b in points:
                with self.subTest(add_formula=((a.x(), a.y()), (b.x(), b.y()))):
                    expected = self.add_formula(curve, a, b)
                    self.assertIsNotNone(expected)

                    sum_ab = ec.twisted_edwards.add(curve, a, b)
                    sum_ba = ec.twisted_edwards.add(curve, b, a)

                    self.assertEqual(sum_ab, expected)
                    self.assertEqual(sum_ab, sum_ba)
                    self.assertTrue(ec.twisted_edwards.in_curve(curve, sum_ab))

        for a in points:
            for b in points:
                for d in points:
                    with self.subTest(associativity=((a.x(), a.y()), (b.x(), b.y()), (d.x(), d.y()))):
                        lhs = ec.twisted_edwards.add(curve, ec.twisted_edwards.add(curve, a, b), d)
                        rhs = ec.twisted_edwards.add(curve, a, ec.twisted_edwards.add(curve, b, d))
                        self.assertEqual(lhs, rhs)

    def test_extended_add_and_double_match_affine_formula(self):
        curve = self.babyjub_curve()
        points = self.sample_points()

        for a in points:
            with self.subTest(double=(a.x(), a.y())):
                expected_dbl = self.add_formula(curve, a, a)
                self.assertIsNotNone(expected_dbl)

                dbl_affine = ec.twisted_edwards.dbl(curve, a)
                self.assertEqual(dbl_affine, expected_dbl)

                dbl_ext = ec.twisted_edwards.dbl(curve, self.scale_ext(curve, ec.twisted_edwards.to_ext(curve, a), 7))
                self.assertTrue(ec.twisted_edwards.in_curve(curve, dbl_ext))
                self.assertTrue(ec.twisted_edwards.equivalent(curve, dbl_ext, ec.twisted_edwards.to_ext(curve, expected_dbl)))

                dbl_roundtrip = ec.twisted_edwards.to_affine(curve, dbl_ext)
                self.assertIsNotNone(dbl_roundtrip)
                self.assertEqual(dbl_roundtrip, expected_dbl)

        for a in points:
            for b in points:
                with self.subTest(ext_add=((a.x(), a.y()), (b.x(), b.y()))):
                    expected = self.add_formula(curve, a, b)
                    self.assertIsNotNone(expected)

                    a_ext = self.scale_ext(curve, ec.twisted_edwards.to_ext(curve, a), 7)
                    b_ext = self.scale_ext(curve, ec.twisted_edwards.to_ext(curve, b), 11)
                    sum_ext = ec.twisted_edwards.add(curve, a_ext, b_ext)

                    self.assertTrue(ec.twisted_edwards.in_curve(curve, sum_ext))
                    self.assertTrue(ec.twisted_edwards.equivalent(curve, sum_ext, ec.twisted_edwards.to_ext(curve, expected)))

                    roundtrip = ec.twisted_edwards.to_affine(curve, sum_ext)
                    self.assertIsNotNone(roundtrip)
                    self.assertEqual(roundtrip, expected)

    def test_mul_scalar_matches_repeated_affine_formula_for_small_scalars(self):
        curve = self.babyjub_curve()
        bases = [
            ec.babyjub.generator(),
            self.sample_points()[2],
            self.non_subgroup_point(),
        ]

        for base in bases:
            base_ext = self.scale_ext(curve, ec.twisted_edwards.to_ext(curve, base), 9)
            for k in range(-32, 33):
                with self.subTest(base=(base.x(), base.y()), scalar=k):
                    expected = self.mul_scalar_formula(curve, base, k)
                    self.assertIsNotNone(expected)

                    affine = ec.twisted_edwards.mul_scalar(curve, base, k)
                    self.assertEqual(affine, expected)

                    ext = ec.twisted_edwards.mul_scalar(curve, base_ext, k)
                    self.assertTrue(ec.twisted_edwards.in_curve(curve, ext))

                    roundtrip = ec.twisted_edwards.to_affine(curve, ext)
                    self.assertIsNotNone(roundtrip)
                    self.assertEqual(roundtrip, expected)

    def test_mul_scalar_respects_group_orders_and_scalar_addition(self):
        curve = self.babyjub_curve()
        g = ec.babyjub.generator()
        g_subgroup = ec.twisted_edwards.mul_scalar(curve, g, 8)
        p = self.non_subgroup_point()

        self.assertEqual(ec.twisted_edwards.mul_scalar(curve, g, 0), ec.twisted_edwards.identity())
        self.assertEqual(ec.twisted_edwards.mul_scalar(curve, g, 1), g)
        self.assertEqual(ec.twisted_edwards.mul_scalar(curve, g, ec.babyjub.group_order()), ec.twisted_edwards.identity())
        self.assertEqual(
            ec.twisted_edwards.mul_scalar(curve, g_subgroup, ec.babyjub.sub_group_order()),
            ec.twisted_edwards.identity()
        )
        self.assertNotEqual(
            ec.twisted_edwards.mul_scalar(curve, g, ec.babyjub.sub_group_order()),
            ec.twisted_edwards.identity()
        )
        self.assertEqual(ec.twisted_edwards.mul_scalar(curve, p, ec.babyjub.group_order()), ec.twisted_edwards.identity())
        self.assertNotEqual(
            ec.twisted_edwards.mul_scalar(curve, p, ec.babyjub.sub_group_order()),
            ec.twisted_edwards.identity()
        )

        lhs = ec.twisted_edwards.mul_scalar(curve, g, self.scalar + 19)
        rhs_left = ec.twisted_edwards.mul_scalar(curve, g, self.scalar)
        rhs_right = ec.twisted_edwards.mul_scalar(curve, g, 19)
        rhs = self.add_formula(curve, rhs_left, rhs_right)
        self.assertIsNotNone(rhs)
        self.assertEqual(lhs, rhs)

    def test_invalid_affine_and_extended_points_are_rejected(self):
        curve = self.babyjub_curve()
        invalid_points = [
            ec.Point(0, 0),
            ec.Point(1, 1),
            ec.Point(2, 2),
            ec.Point(16, 16),
            ec.Point(-1, -1),
            ec.Point(-2, -2),
            ec.Point(-1, 16),
            ec.Point(ec.babyjub.prime(), ec.babyjub.prime()),
            ec.Point(1, ec.babyjub.prime()),
            ec.Point(ec.babyjub.prime() + 1, ec.babyjub.prime()),
            ec.Point(ec.babyjub.prime() - 1, ec.babyjub.prime()),
        ]

        for p in invalid_points:
            with self.subTest(invalid_affine=(p.x(), p.y())):
                self.assertFalse(ec.twisted_edwards.in_curve(curve, p))

        invalid_z = ec.twisted_edwards.ExtPoint(1, 1, 0, 1)
        self.assertFalse(ec.twisted_edwards.in_curve(curve, invalid_z))
        self.assertIsNone(ec.twisted_edwards.to_affine(curve, invalid_z))
        self.assertFalse(ec.twisted_edwards.equivalent(curve, invalid_z, ec.twisted_edwards.ext_identity()))

        valid = ec.twisted_edwards.to_ext(curve, ec.babyjub.generator())
        invalid_t = ec.twisted_edwards.ExtPoint(valid.X(), valid.Y(), valid.Z(), valid.T() + 1)
        self.assertFalse(ec.twisted_edwards.in_curve(curve, invalid_t))

