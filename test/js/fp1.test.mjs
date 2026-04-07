import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { Fp1, prime } from "./common.mjs";

const primes = [
  17n,
  (2n ** 31n) - 1n,
  prime.bn254(),
];

describe("Fp1", () => {
  test("mod_reduce", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.mod_reduce(0n), 0n);
      assert.equal(f.mod_reduce(1n), 1n);
      assert.equal(f.mod_reduce(-1n), p - 1n);
      assert.equal(f.mod_reduce(p - 1n), p - 1n);
      assert.equal(f.mod_reduce(p), 0n);
      assert.equal(f.mod_reduce(p + 1n), 1n);
      assert.equal(f.mod_reduce(-p), 0n);
      assert.equal(f.mod_reduce(-p - 1n), p - 1n);
      assert.equal(f.mod_reduce(-p + 1n), 1n);
    }
  });

  test("add", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.add(0n, 0n), 0n);
      assert.equal(f.add(2n, 3n), 5n);
      assert.equal(f.add(-2n, 3n), 1n);
      assert.equal(f.add(2n, -3n), p - 1n);
      assert.equal(f.add(-2n, -3n), p - 5n);
      assert.equal(f.add(p, 0n), 0n);
      assert.equal(f.add(p, 1n), 1n);
      assert.equal(f.add(p - 1n, 1n), 0n);
      assert.equal(f.add(p - 2n, 5n), 3n);
      assert.equal(f.add(p - 2n, -5n), p - 7n);
    }
  });

  test("sub", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.sub(0n, 0n), 0n);
      assert.equal(f.sub(5n, 3n), 2n);
      assert.equal(f.sub(3n, 5n), p - 2n);
      assert.equal(f.sub(-5n, 3n), p - 8n);
      assert.equal(f.sub(-3n, 5n), p - 8n);
      assert.equal(f.sub(p, 0n), 0n);
      assert.equal(f.sub(p, 1n), p - 1n);
      assert.equal(f.sub(p + 1n, 1n), 0n);
      assert.equal(f.sub(p + 2n, 3n), p - 1n);
      assert.equal(f.sub(p - 1n, 1n), p - 2n);
      assert.equal(f.sub(p - 2n, 5n), p - 7n);
      assert.equal(f.sub(p - 2n, -5n), 3n);
    }
  });

  test("mul", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.mul(0n, 0n), 0n);
      assert.equal(f.mul(2n, 3n), 6n);
      assert.equal(f.mul(-2n, 3n), p - 6n);
      assert.equal(f.mul(2n, -3n), p - 6n);
      assert.equal(f.mul(-2n, -3n), 6n);
      assert.equal(f.mul(p, 0n), 0n);
      assert.equal(f.mul(p, 1n), 0n);
      assert.equal(f.mul(p + 1n, 2n), 2n);
      assert.equal(f.mul(p + 2n, 3n), 6n);
      assert.equal(f.mul(p - 1n, 2n), p - 2n);
      assert.equal(f.mul(p - 2n, 3n), p - 6n);
      assert.equal(f.mul(0n, 5n), 0n);
      assert.equal(f.mul(-1n, 5n), p - 5n);
    }
  });

  test("div", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.div(1n, 0n), null);
      assert.equal(f.div(1n, p - 1n), p - 1n);
      assert.equal(f.div(1n, 1n), 1n);
      assert.equal(f.div(4n, 2n), 2n);
      assert.equal(f.div(6n, 2n), 3n);
      assert.equal(f.div(6n, 3n), 2n);
      assert.equal(f.div(8n, 4n), 2n);
      assert.equal(f.div(8n, -4n), p - 2n);
      assert.equal(f.div(-8n, 4n), p - 2n);
      assert.equal(f.div(-8n, -4n), 2n);

      const div53 = f.div(5n, 3n);
      assert.notEqual(div53, null);
      assert.equal(f.mul(div53, 3n), 5n);

      const div24 = f.div(2n, 4n);
      assert.notEqual(div24, null);
      assert.equal(f.mul(div24, 4n), 2n);

      const divPm1 = f.div(p - 1n, p - 2n);
      assert.notEqual(divPm1, null);
      assert.equal(f.mul(divPm1, p - 2n), p - 1n);

      const divPm2 = f.div(p - 2n, p - 1n);
      assert.notEqual(divPm2, null);
      assert.equal(f.mul(divPm2, p - 1n), p - 2n);
    }
  });

  test("pow", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.pow(2n, 3n), 8n);
      assert.equal(f.pow(2n, 0n), 1n);
      assert.equal(f.pow(0n, 5n), 0n);
      assert.equal(f.pow(5n, 1n), 5n);

      const invPow = f.pow(2n, -3n);
      assert.notEqual(invPow, null);
      assert.equal(f.mod_reduce(invPow * f.pow(2n, 3n)), 1n);

      const negPow = f.pow(-2n, 3n);
      assert.notEqual(negPow, null);
      assert.equal(negPow, f.pow(p - 2n, 3n));

      const negInvPow = f.pow(-2n, -3n);
      assert.notEqual(negInvPow, null);
      assert.equal(f.mod_reduce(negInvPow * f.pow(p - 2n, 3n)), 1n);

      assert.equal(f.pow(0n, -1n), null);
      assert.equal(f.pow(1n, 0n), 1n);
      assert.equal(f.pow(1n, -1n), 1n);
    }
  });

  test("square", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.square(0n), 0n);
      assert.equal(f.square(1n), 1n);
      assert.equal(f.square(2n), 4n);
      assert.equal(f.square(3n), 9n);
      assert.equal(f.square(4n), 16n);
      assert.equal(f.square(-4n), 16n);
      assert.equal(f.square(p), 0n);
      assert.equal(f.square(p - 1n), 1n);
      assert.equal(f.square(p - 2n), 4n);
      assert.equal(f.square(p - 3n), 9n);
      assert.equal(f.square(p + 1n), 1n);
      assert.equal(f.square(p + 2n), 4n);
      assert.equal(f.square(p + 3n), 9n);
    }
  });

  test("sqrt", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.sqrt(0n), 0n);

      for (let i = 0n; i < 17n; i += 1n) {
        const root = f.sqrt(i);
        if (root !== null) {
          assert.equal(f.square(root), i);
        }
      }

      const values = [p, p * 2n, p * 2n + 1n, p - 2n, p - 1n, p + 1n, p + 2n];
      const expected = [0n, 0n, 1n, p - 2n, p - 1n, 1n, 2n];
      values.forEach((value, index) => {
        const root = f.sqrt(value);
        if (root !== null) {
          assert.equal(f.square(root), expected[index]);
        }
      });
    }

    const f = new Fp1(17n);
    const root = f.sqrt(-1n);
    assert.notEqual(root, null);
    assert.equal(f.square(root), 16n);
  });

  test("mod_inv", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.mod_inv(0n), null);
      assert.equal(f.mod_inv(p), null);
      assert.equal(f.mod_inv(-p), null);
      assert.equal(f.mul(f.mod_inv(1n), 1n), 1n);
      assert.equal(f.mul(f.mod_inv(-1n), -1n), 1n);
      assert.equal(f.mul(f.mod_inv(-2n), -2n), 1n);
      assert.equal(f.mul(f.mod_inv(2n), 2n), 1n);
      assert.equal(f.mul(f.mod_inv(p - 1n), p - 1n), 1n);
      assert.equal(f.mul(f.mod_inv(p + 1n), p + 1n), 1n);
    }
  });

  test("neg", () => {
    for (const p of primes) {
      const f = new Fp1(p);
      assert.equal(f.neg(0n), 0n);
      assert.equal(f.neg(1n), p - 1n);
      assert.equal(f.neg(2n), p - 2n);
      assert.equal(f.neg(-2n), 2n);
      assert.equal(f.neg(p), 0n);
      assert.equal(f.neg(p - 1n), 1n);
      assert.equal(f.neg(p - 2n), 2n);
      assert.equal(f.neg(p + 1n), p - 1n);
      assert.equal(f.neg(p + 2n), p - 2n);
    }
  });

  test("has_sqrt", () => {
    const f = new Fp1(17n);
    const withSqrt = [1n, 2n, 4n, 8n, 9n, 13n, 15n, 16n];
    const noSqrt = [3n, 5n, 6n, 7n, 10n, 11n, 12n, 14n];

    for (const value of withSqrt) {
      assert.equal(f.has_sqrt(value), true);
    }

    for (const value of noSqrt) {
      assert.equal(f.has_sqrt(value), false);
    }
  });
});
