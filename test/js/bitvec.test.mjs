import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { BitVec1D } from "./common.mjs";

describe("BitVec1D", () => {
  test("constructor", () => {
    const bv = new BitVec1D();
    assert.equal(bv.length, 0);
    assert.equal(bv.size(), 0);
  });

  test("access out of range", () => {
    const bv = new BitVec1D();
    assert.throws(() => bv.get(1));

    for (let i = 0; i < 8; i += 1) {
      bv.push(true);
    }

    assert.equal(bv.get(7), true);
    assert.throws(() => bv.get(8));

    bv.push(true);
    assert.equal(bv.get(8), true);
    assert.throws(() => bv.get(9));
  });

  test("push and access", () => {
    const bv = new BitVec1D();
    bv.push(true);
    bv.push(false);
    bv.push(true);

    assert.equal(bv.get(0), true);
    assert.equal(bv.get(1), false);
    assert.equal(bv.get(2), true);

    for (let i = 0; i < 10; i += 1) {
      bv.push(i % 2 === 0);
    }

    assert.equal(bv.get(3), true);
    assert.equal(bv.get(4), false);
    assert.equal(bv.get(5), true);
    assert.equal(bv.get(6), false);
    assert.equal(bv.get(7), true);
    assert.equal(bv.get(8), false);
    assert.equal(bv.get(9), true);
    assert.equal(bv.get(10), false);
    assert.equal(bv.get(11), true);
    assert.equal(bv.get(12), false);
  });

  test("push and access across byte boundary", () => {
    const bv = new BitVec1D();

    for (let i = 0; i < 8; i += 1) {
      bv.push(true);
    }

    for (let i = 0; i < 8; i += 1) {
      assert.equal(bv.get(i), true);
    }

    bv.push(false);
    bv.push(true);

    assert.equal(bv.get(8), false);
    assert.equal(bv.get(9), true);
  });

  test("alternating pattern", () => {
    const bv = new BitVec1D();

    for (let i = 0; i < 16; i += 1) {
      bv.push(i % 2 === 0);
    }

    for (let i = 0; i < 16; i += 1) {
      assert.equal(bv.get(i), i % 2 === 0);
    }
  });
});
