import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { bytesFromHex, hash } from "./common.mjs";

describe("hash.pedersen", () => {
  const cases = [
    { input: new Uint8Array(), expected: "0100000000000000000000000000000000000000000000000000000000000000" },
    { input: Uint8Array.of(0x01), expected: "75c28cc0b8c45fa951bd48ffeb096e3373dac173ee78fc7b58e9ce8dc193b01d" },
    { input: Uint8Array.of(0x01, 0x01), expected: "70db4185b286d4f8cbc8b7bc8b72e312c3b8eb508d3d61e4cf5e8d59d5a81ba6" },
    { input: new Uint8Array(25).fill(0xaa), expected: "52d57e2313839d26950b857c0ee3e9b938cfd928a066ff30c301bf77cc705c06" },
    { input: Uint8Array.from([...new Uint8Array(25).fill(0xaa), 0x01]), expected: "b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3" },
    { input: bytesFromHex("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3"), expected: "5e749cf88b25a21a292a3116c8e068af236ac370c51fe8ab96291c68392392a2" },
    { input: bytesFromHex("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800"), expected: "f3d0d29c8ca8cb52845f2b02e2dd61b494dd40ba19cfde518d8b1161a6260c29" },
    { input: bytesFromHex("b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3"), expected: "de1a2202abf4c240d871b13f547011c7fca48e258d8c0ae82745e5ceb9e1b522" },
    { input: bytesFromHex("00ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f76328808a3b5ad37a1cf080e8eaa796d04c998861b7ee691c46216ee49cbe88f7632880800"), expected: "b634ccf13fdba7b9c2666167869686910e6035f37bb31ba37e7df94e20f8801a" },
    { input: new Uint8Array(50).fill(0xaa), expected: "09552440bd1c1bab3003021b564fddcab835ed6248f157d81c3df7581452531c" },
    { input: Uint8Array.from([...new Uint8Array(50).fill(0xaa), 0x01]), expected: "cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e" },
    { input: Uint8Array.from([...new Uint8Array(1000).fill(0xaa), ...new Uint8Array(1000).fill(0xcd)]), expected: "1bf524b6681272083e9014f73ec4093399270d1ea4aeae96c623ec7e34f2a48d" },
    { input: new Uint8Array(100).fill(0xff), expected: "47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d" },
  ];

  test("known vectors", () => {
    for (const { input, expected } of cases) {
      const digest = hash.pedersen(input);
      const padded = Buffer.from(expected, "hex");
      if (padded.length < 32) {
        const output = Buffer.alloc(32);
        padded.copy(output);
        assert.deepEqual(Buffer.from(digest), output);
      } else {
        assert.deepEqual(Buffer.from(digest), padded);
      }
    }
  });

  test("digest container is not reused across calls", () => {
    assert.deepEqual(Buffer.from(hash.pedersen(new Uint8Array(100).fill(0xff))), Buffer.from("47522d13124d066e751a0254ce3a439db81263c0b543376cd27d0bb37e658a0d", "hex"));
    assert.deepEqual(Buffer.from(hash.pedersen(Uint8Array.from([...new Uint8Array(50).fill(0xaa), 0x01]))), Buffer.from("cbcc6877d2bc7dab56621533f4ce2a5b91be9b842b2ae561fa7546251cfe4e2e", "hex"));
  });
});
