import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { Fp1, assertExtPointEqual, assertPointEqual, ec } from "./common.mjs";

const scalar = 7120861356467848435263064379192047478074060781135320967663101236819528304084n;

function babyjubCurve() {
  return new ec.twisted_edwards.Curve(ec.babyjub.finite_field(), 168700n, 168696n);
}

function nonSubgroupPoint() {
  return new ec.Point(
    18058203230422821100148902886692797825595047188176647392575344616831548269714n,
    9711944701593973077501119179165085016629774523576998623469657748778309798227n,
  );
}

function samplePoints() {
  return [
    ec.twisted_edwards.identity(),
    ec.babyjub.generator(),
    new ec.Point(
      3598477115375956014038673435118146273990307570718665994596246292010479888129n,
      10638520112689679830908703045972665394487620627132873410365672297806577997548n,
    ),
    new ec.Point(
      20068488524343428343945823897150776910502466037019831511105395347911200592077n,
      8654005564599280198548960974184530398968954683306629931317509486422683019120n,
    ),
    nonSubgroupPoint(),
  ];
}

function negateAffine(curve, point) {
  return new ec.Point(curve.field().neg(point.x()), point.y());
}

function scaleExt(curve, point, lambda) {
  const field = curve.field();
  return new ec.twisted_edwards.ExtPoint(
    field.mul(point.X(), lambda),
    field.mul(point.Y(), lambda),
    field.mul(point.Z(), lambda),
    field.mul(point.T(), lambda),
  );
}

function addFormula(curve, a, b) {
  const field = curve.field();
  const x1x2 = field.mul(a.x(), b.x());
  const y1y2 = field.mul(a.y(), b.y());
  const x1y2 = field.mul(a.x(), b.y());
  const y1x2 = field.mul(a.y(), b.x());
  const dxxyy = field.mul(curve.d(), field.mul(x1x2, y1y2));

  const x = field.div(field.add(x1y2, y1x2), field.add(1n, dxxyy));
  if (x === null) {
    return null;
  }

  const y = field.div(field.sub(y1y2, field.mul(curve.a(), x1x2)), field.sub(1n, dxxyy));
  if (y === null) {
    return null;
  }

  return new ec.Point(x, y);
}

function mulScalarFormula(curve, point, scalarValue) {
  let acc = ec.twisted_edwards.identity();
  let addend = point;
  let count = scalarValue;

  if (count < 0n) {
    addend = negateAffine(curve, point);
    count = -count;
  }

  for (let i = 0n; i < count; i += 1n) {
    acc = addFormula(curve, acc, addend);
    if (acc === null) {
      return null;
    }
  }

  return acc;
}

describe("ec.twisted_edwards", () => {
  test("identity roundtrip", () => {
    const curve = babyjubCurve();

    assert.equal(ec.twisted_edwards.in_curve(curve, ec.twisted_edwards.identity()), true);
    assert.equal(ec.twisted_edwards.in_curve(curve, ec.twisted_edwards.ext_identity()), true);

    const affine = ec.twisted_edwards.to_affine(curve, ec.twisted_edwards.ext_identity());
    assert.notEqual(affine, null);
    assertPointEqual(affine, ec.twisted_edwards.identity());
  });

  test("affine and extended roundtrip preserve curve membership", () => {
    const curve = babyjubCurve();

    for (const point of samplePoints()) {
      assert.equal(ec.twisted_edwards.in_curve(curve, point), true);

      const canonicalExt = ec.twisted_edwards.to_ext(curve, point);
      const scaledExt = scaleExt(curve, canonicalExt, 7n);

      assert.equal(ec.twisted_edwards.in_curve(curve, canonicalExt), true);
      assert.equal(ec.twisted_edwards.in_curve(curve, scaledExt), true);
      assert.equal(ec.twisted_edwards.equivalent(curve, canonicalExt, scaledExt), true);
      assert.equal(ec.twisted_edwards.equivalent(curve, scaledExt, canonicalExt), true);

      assertPointEqual(ec.twisted_edwards.to_affine(curve, canonicalExt), point);
      assertPointEqual(ec.twisted_edwards.to_affine(curve, scaledExt), point);
    }
  });

  test("equivalent matches projective scaling only", () => {
    const curve = babyjubCurve();
    const generatorExt = ec.twisted_edwards.to_ext(curve, ec.babyjub.generator());
    const generatorScaled = scaleExt(curve, generatorExt, 7n);
    const generatorScaledAgain = scaleExt(curve, generatorExt, 11n);
    const otherExt = ec.twisted_edwards.to_ext(curve, nonSubgroupPoint());

    assert.equal(ec.twisted_edwards.equivalent(curve, generatorExt, generatorScaled), true);
    assert.equal(ec.twisted_edwards.equivalent(curve, generatorScaled, generatorScaledAgain), true);
    assert.equal(ec.twisted_edwards.equivalent(curve, generatorExt, otherExt), false);
  });

  test("add matches affine formula and group laws", () => {
    const curve = babyjubCurve();
    const field = curve.field();
    const points = samplePoints();

    for (const point of points) {
      const negPoint = negateAffine(curve, point);
      assert.equal(ec.twisted_edwards.in_curve(curve, negPoint), true);
      assertPointEqual(ec.twisted_edwards.add(curve, ec.twisted_edwards.identity(), point), point);
      assertPointEqual(ec.twisted_edwards.add(curve, point, ec.twisted_edwards.identity()), point);
      assertPointEqual(ec.twisted_edwards.add(curve, point, negPoint), ec.twisted_edwards.identity());
      assertPointEqual(ec.twisted_edwards.add(curve, negPoint, point), ec.twisted_edwards.identity());
      assert.equal(negPoint.x(), field.neg(point.x()));
      assert.equal(negPoint.y(), point.y());
    }

    for (const a of points) {
      for (const b of points) {
        const expected = addFormula(curve, a, b);
        assert.notEqual(expected, null);

        const sumAB = ec.twisted_edwards.add(curve, a, b);
        const sumBA = ec.twisted_edwards.add(curve, b, a);

        assertPointEqual(sumAB, expected);
        assertPointEqual(sumAB, sumBA);
        assert.equal(ec.twisted_edwards.in_curve(curve, sumAB), true);
      }
    }

    for (const a of points) {
      for (const b of points) {
        for (const d of points) {
          const lhs = ec.twisted_edwards.add(curve, ec.twisted_edwards.add(curve, a, b), d);
          const rhs = ec.twisted_edwards.add(curve, a, ec.twisted_edwards.add(curve, b, d));
          assertPointEqual(lhs, rhs);
        }
      }
    }
  });

  test("extended add and double match affine formula", () => {
    const curve = babyjubCurve();
    const points = samplePoints();

    for (const a of points) {
      const expectedDbl = addFormula(curve, a, a);
      assert.notEqual(expectedDbl, null);

      assertPointEqual(ec.twisted_edwards.dbl(curve, a), expectedDbl);

      const dblExt = ec.twisted_edwards.dbl(curve, scaleExt(curve, ec.twisted_edwards.to_ext(curve, a), 7n));
      assert.equal(ec.twisted_edwards.in_curve(curve, dblExt), true);
      assert.equal(ec.twisted_edwards.equivalent(curve, dblExt, ec.twisted_edwards.to_ext(curve, expectedDbl)), true);
      assertPointEqual(ec.twisted_edwards.to_affine(curve, dblExt), expectedDbl);
    }

    for (const a of points) {
      for (const b of points) {
        const expected = addFormula(curve, a, b);
        assert.notEqual(expected, null);

        const aExt = scaleExt(curve, ec.twisted_edwards.to_ext(curve, a), 7n);
        const bExt = scaleExt(curve, ec.twisted_edwards.to_ext(curve, b), 11n);
        const sumExt = ec.twisted_edwards.add(curve, aExt, bExt);

        assert.equal(ec.twisted_edwards.in_curve(curve, sumExt), true);
        assert.equal(ec.twisted_edwards.equivalent(curve, sumExt, ec.twisted_edwards.to_ext(curve, expected)), true);
        assertPointEqual(ec.twisted_edwards.to_affine(curve, sumExt), expected);
      }
    }
  });

  test("mul_scalar matches repeated affine formula for small scalars", () => {
    const curve = babyjubCurve();
    const bases = [ec.babyjub.generator(), samplePoints()[2], nonSubgroupPoint()];

    for (const base of bases) {
      const baseExt = scaleExt(curve, ec.twisted_edwards.to_ext(curve, base), 9n);
      for (let k = -32n; k <= 32n; k += 1n) {
        const expected = mulScalarFormula(curve, base, k);
        assert.notEqual(expected, null);

        assertPointEqual(ec.twisted_edwards.mul_scalar(curve, base, k), expected);

        const ext = ec.twisted_edwards.mul_scalar(curve, baseExt, k);
        assert.equal(ec.twisted_edwards.in_curve(curve, ext), true);
        assertPointEqual(ec.twisted_edwards.to_affine(curve, ext), expected);
      }
    }
  });

  test("mul_scalar respects group orders and scalar addition", () => {
    const curve = babyjubCurve();
    const generator = ec.babyjub.generator();
    const subgroupGenerator = ec.twisted_edwards.mul_scalar(curve, generator, 8n);
    const point = nonSubgroupPoint();

    assertPointEqual(ec.twisted_edwards.mul_scalar(curve, generator, 0n), ec.twisted_edwards.identity());
    assertPointEqual(ec.twisted_edwards.mul_scalar(curve, generator, 1n), generator);
    assertPointEqual(ec.twisted_edwards.mul_scalar(curve, generator, ec.babyjub.group_order()), ec.twisted_edwards.identity());
    assertPointEqual(ec.twisted_edwards.mul_scalar(curve, subgroupGenerator, ec.babyjub.sub_group_order()), ec.twisted_edwards.identity());
    assert.equal(ec.twisted_edwards.mul_scalar(curve, generator, ec.babyjub.sub_group_order()).equals(ec.twisted_edwards.identity()), false);
    assertPointEqual(ec.twisted_edwards.mul_scalar(curve, point, ec.babyjub.group_order()), ec.twisted_edwards.identity());
    assert.equal(ec.twisted_edwards.mul_scalar(curve, point, ec.babyjub.sub_group_order()).equals(ec.twisted_edwards.identity()), false);

    const lhs = ec.twisted_edwards.mul_scalar(curve, generator, scalar + 19n);
    const rhsLeft = ec.twisted_edwards.mul_scalar(curve, generator, scalar);
    const rhsRight = ec.twisted_edwards.mul_scalar(curve, generator, 19n);
    const rhs = addFormula(curve, rhsLeft, rhsRight);
    assert.notEqual(rhs, null);
    assertPointEqual(lhs, rhs);
  });

  test("invalid affine and extended points are rejected", () => {
    const curve = babyjubCurve();
    const invalidPoints = [
      new ec.Point(0n, 0n),
      new ec.Point(1n, 1n),
      new ec.Point(2n, 2n),
      new ec.Point(16n, 16n),
      new ec.Point(-1n, -1n),
      new ec.Point(-2n, -2n),
      new ec.Point(-1n, 16n),
      new ec.Point(ec.babyjub.prime(), ec.babyjub.prime()),
      new ec.Point(1n, ec.babyjub.prime()),
      new ec.Point(ec.babyjub.prime() + 1n, ec.babyjub.prime()),
      new ec.Point(ec.babyjub.prime() - 1n, ec.babyjub.prime()),
    ];

    for (const point of invalidPoints) {
      assert.equal(ec.twisted_edwards.in_curve(curve, point), false);
    }

    const invalidZ = new ec.twisted_edwards.ExtPoint(1n, 1n, 0n, 1n);
    assert.equal(ec.twisted_edwards.in_curve(curve, invalidZ), false);
    assert.equal(ec.twisted_edwards.to_affine(curve, invalidZ), null);
    assert.equal(ec.twisted_edwards.equivalent(curve, invalidZ, ec.twisted_edwards.ext_identity()), false);

    const valid = ec.twisted_edwards.to_ext(curve, ec.babyjub.generator());
    const invalidT = new ec.twisted_edwards.ExtPoint(valid.X(), valid.Y(), valid.Z(), valid.T() + 1n);
    assert.equal(ec.twisted_edwards.in_curve(curve, invalidT), false);
  });
});
