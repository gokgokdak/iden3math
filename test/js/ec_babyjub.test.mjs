import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { babyjubMulScalarExpectedCoordinates } from "./babyjub.fixtures.mjs";
import { Endian, assertPointEqual, ec } from "./common.mjs";

const scalar = 7120861356467848435263064379192047478074060781135320967663101236819528304084n;

describe("ec.babyjub", () => {
  test("mul_scalar known vector sequence", () => {
    const expected = babyjubMulScalarExpectedCoordinates.map(([x, y]) => new ec.Point(x, y));

    expected.forEach((point, index) => {
      const actual = ec.babyjub.mul_scalar(ec.babyjub.generator(), scalar + BigInt(index));
      assertPointEqual(actual, point);
    });
  });

  test("repeated add matches mul_scalar", () => {
    const repeatTimes = 2000n;
    let sum = new ec.Point(0n, 1n);
    for (let i = 0n; i < repeatTimes; i += 1n) {
      sum = ec.babyjub.add(sum, ec.babyjub.generator());
    }
    assertPointEqual(sum, ec.babyjub.mul_scalar(ec.babyjub.generator(), repeatTimes));
  });

  test("in_curve", () => {
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
    invalidPoints.forEach((point) => assert.equal(ec.babyjub.in_curve(point), false));

    assert.equal(ec.babyjub.in_curve(ec.babyjub.zero()), true);
    assert.equal(ec.babyjub.in_curve(ec.babyjub.generator()), true);

    for (let i = 0n; i < 200n; i += 1n) {
      const point = ec.babyjub.mul_scalar(ec.babyjub.generator(), scalar + i);
      assert.equal(ec.babyjub.in_curve(point), true);
    }
  });

  test("in_subgroup", () => {
    const identity = new ec.Point(0n, 1n);
    assert.equal(ec.babyjub.in_sub_group(identity), true);

    const point = new ec.Point(
      18058203230422821100148902886692797825595047188176647392575344616831548269714n,
      9711944701593973077501119179165085016629774523576998623469657748778309798227n,
    );
    assert.equal(ec.babyjub.in_curve(point), true);
    assert.equal(ec.babyjub.in_sub_group(point), false);

    const subgroupGenerator = ec.babyjub.mul_scalar(ec.babyjub.generator(), 8n);
    assert.equal(ec.babyjub.in_sub_group(subgroupGenerator), true);

    for (let i = 0n; i < 200n; i += 1n) {
      assert.equal(ec.babyjub.in_sub_group(ec.babyjub.mul_scalar(subgroupGenerator, i)), true);
    }
  });

  test("add obeys commutativity and associativity on representative cases", () => {
    const check = (a, b, c) => {
      const ab = ec.babyjub.add(a, b);
      const ba = ec.babyjub.add(b, a);
      assertPointEqual(ab, ba);
      assertPointEqual(ec.babyjub.add(ab, c), ec.babyjub.add(a, ec.babyjub.add(b, c)));
    };

    const p = ec.babyjub.prime();
    const points0 = [
      [new ec.Point(0n, 0n), new ec.Point(0n, 0n), new ec.Point(p, p)],
      [new ec.Point(0n, 1n), new ec.Point(0n, 1n), new ec.Point(p, p)],
      [new ec.Point(1n, 0n), new ec.Point(1n, 0n), new ec.Point(p, p)],
      [new ec.Point(p, 1n), new ec.Point(p, 1n), new ec.Point(16n, 16n)],
      [new ec.Point(1n, p), new ec.Point(1n, p), new ec.Point(16n, 16n)],
      [new ec.Point(p, p), new ec.Point(p, p), new ec.Point(16n, 16n)],
      [new ec.Point(p + 1n, p), new ec.Point(0n, 0n), new ec.Point(16n, 16n)],
      [new ec.Point(p - 1n, p), new ec.Point(0n, 0n), new ec.Point(16n, 16n)],
    ];
    points0.forEach(([a, b, c]) => check(a, b, c));

    let k = scalar;
    for (let i = 0; i < 200; i += 1) {
      k += 1n;
      check(
        ec.babyjub.mul_scalar(ec.babyjub.generator(), k),
        ec.babyjub.mul_scalar(ec.babyjub.generator(), k + 1n),
        ec.babyjub.mul_scalar(ec.babyjub.generator(), k + 2n),
      );
    }
  });

  test("compress/decompress little endian", () => {
    for (let i = 0n; i < 200n; i += 1n) {
      const point = ec.babyjub.mul_scalar(ec.babyjub.generator(), scalar + i);
      const packed = ec.babyjub.compress(point, Endian.LE);
      const unpacked = ec.babyjub.decompress(packed, Endian.LE);
      assert.equal(ec.babyjub.in_curve(point), true);
      assert.notEqual(unpacked, null);
      assertPointEqual(unpacked, point);
    }
  });

  test("compress/decompress big endian", () => {
    for (let i = 0n; i < 200n; i += 1n) {
      const point = ec.babyjub.mul_scalar(ec.babyjub.generator(), scalar + i);
      const packed = ec.babyjub.compress(point, Endian.BE);
      const unpacked = ec.babyjub.decompress(packed, Endian.BE);
      assert.equal(ec.babyjub.in_curve(point), true);
      assert.notEqual(unpacked, null);
      assertPointEqual(unpacked, point);
    }
  });
});
