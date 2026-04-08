import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { random } from "./common.mjs";

describe("random", () => {
  test("get_bytes length", () => {
    for (const length of [1, 8, 16, 32, 64, 128, 256]) {
      assert.equal(random.get_bytes(length).length, length);
    }
  });

  test("get_bytes distribution", () => {
    const numSamples = 20000;
    const counts = new Array(256).fill(0);

    for (let i = 0; i < numSamples; i += 1) {
      const bytes = random.get_bytes(1);
      counts[bytes[0]] += 1;
    }

    const expected = numSamples / 256;
    let chiSquare = 0;
    for (let i = 0; i < 256; i += 1) {
      const diff = counts[i] - expected;
      chiSquare += (diff * diff) / expected;
    }

    assert.ok(chiSquare < 365.0, `chi-square too large: ${chiSquare}`);
  });

  test("get_integer range", () => {
    const p = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2fn;
    for (let i = 0; i < 1000; i += 1) {
      const value = random.get_integer(p);
      assert.ok(value > 0n);
      assert.ok(value < p);
    }
  });

  test("get_integer distribution", () => {
    const p = 17n;
    const counts = new Array(16).fill(0);
    const numSamples = 8000;

    for (let i = 0; i < numSamples; i += 1) {
      counts[Number(random.get_integer(p) - 1n)] += 1;
    }

    const expected = numSamples / 16;
    let chiSquare = 0;
    for (let i = 0; i < 16; i += 1) {
      const diff = counts[i] - expected;
      chiSquare += (diff * diff) / expected;
    }

    assert.ok(chiSquare < 45.0, `chi-square too large: ${chiSquare}`);
  });
});
