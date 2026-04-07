import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { hash, hex } from "./common.mjs";

describe("hash.blake256", () => {
  const cases = [
    { name: "single byte 0x00", input: Uint8Array.of(0x00), expected: "0ce8d4ef4dd7cd8d62dfded9d4edb0a774ae6a41929a74da23109e8f11139c87" },
    { name: "single byte 0xaa", input: Uint8Array.of(0xaa), expected: "2057dc48805e7e0a1140f2a80e72d58dd9607dfeffcb0159b98658fc15322c1e" },
    { name: "single byte 0xff", input: Uint8Array.of(0xff), expected: "63bf0367c7edf0a5285db26daaeb90629434ce05e7f385de1db83ea3c70bc4cf" },
    { name: "55 bytes 0x00", input: new Uint8Array(55), expected: "dc980544f4181cc43505318e317cdfd4334dab81ae035a28818308867ce23060" },
    { name: "55 bytes 0xaa", input: new Uint8Array(55).fill(0xaa), expected: "2cd6a7d2bc557e771cc7b482f987cb18ffd5d2d19cf444ff03f1fcc736da44a4" },
    { name: "55 bytes 0xff", input: new Uint8Array(55).fill(0xff), expected: "d806c129c0a95654d746419667a9f0878da9cc5d55d77e3e22df7c1b12176010" },
    { name: "64 bytes 0x00", input: new Uint8Array(64), expected: "6d994042954f8dc5633626cd50b2bc66d733a313d67fd9702c5a8149a8028c98" },
    { name: "64 bytes 0xaa", input: new Uint8Array(64).fill(0xaa), expected: "00bb5fa2cd91c47bb8bfaf8f8489a2603d7b7d7291894d476d4858961436ae9a" },
    { name: "64 bytes 0xff", input: new Uint8Array(64).fill(0xff), expected: "80a0ace8b131870da8de11bca85a811f44ece342c57cb8cd5567d2a33685b5be" },
    { name: "100 bytes 0x00", input: new Uint8Array(100), expected: "db10fa7d8a13c4bb74729474485366132da7e221ec651f57a3b7fc3258af9696" },
    { name: "100 bytes 0xaa", input: new Uint8Array(100).fill(0xaa), expected: "cf274ed07d6e716f979a238151bb9d6c6a67376e5b736046880a30819ee3d3fe" },
    { name: "100 bytes 0xff", input: new Uint8Array(100).fill(0xff), expected: "c9dda01aa7cd6e0c5401ac0d3c4a30abab4c229d7039714be52db6257088c7ae" },
    { name: "128 bytes 0x00", input: new Uint8Array(128), expected: "4c8ed99ae2cfdd5bdaba9f19848fcd98b4c60e122096a47ea565c410a1d567ce" },
    { name: "128 bytes 0xaa", input: new Uint8Array(128).fill(0xaa), expected: "5320a6e09347384b05bbe61b5d43bb2055584a56c1f753a9ac93cecff0a271c8" },
    { name: "128 bytes 0xff", input: new Uint8Array(128).fill(0xff), expected: "9f0edf629f1045114aedcbe094093e4f4e7a4b24ca9794219a35499a4969f1d9" },
    { name: "1000 bytes 0x00", input: new Uint8Array(1000), expected: "e63f5343fc28b480dd8e9586f5cc9b11827e9b317d7a4086326e671b68a5efee" },
    { name: "1000 bytes 0xaa", input: new Uint8Array(1000).fill(0xaa), expected: "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513" },
    { name: "1000 bytes 0xff", input: new Uint8Array(1000).fill(0xff), expected: "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384" },
  ];

  test("empty input returns empty digest", () => {
    assert.equal(hash.blake256(new Uint8Array()).length, 0);
    assert.equal(hash.blake256("").length, 0);
  });

  test("known vectors", () => {
    for (const { input, expected } of cases) {
      assert.equal(hex(hash.blake256(input)), expected);
    }
  });

  test("text input matches utf-8 bytes", () => {
    const bytes = Uint8Array.from([0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39]);
    assert.equal(hex(hash.blake256(bytes)), hex(hash.blake256("0123456789")));
  });

  test("digest container is not reused across calls", () => {
    assert.equal(hex(hash.blake256(new Uint8Array(1000).fill(0xaa))), "a821641b802939d182455e571294313674e36877ab362c589728ae182458c513");
    assert.equal(hex(hash.blake256(new Uint8Array(1000).fill(0xff))), "13252f0a129e6c30d474dcbde5290ece336a82233ca7fd95e2c3b664fbf68384");
  });
});
