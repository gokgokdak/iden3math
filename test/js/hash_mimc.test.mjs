import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { Worker } from "node:worker_threads";
import { Endian, hash, hex, leftPadHex, prime } from "./common.mjs";

describe("hash.mimc_sponge", () => {
  function runWorker(job, loopCount) {
    return new Promise((resolve, reject) => {
      const worker = new Worker(new URL("./hash_mimc.worker.mjs", import.meta.url), {
        type: "module",
        workerData: { job, loopCount },
      });

      worker.once("message", () => resolve());
      worker.once("error", reject);
      worker.once("exit", (code) => {
        if (code !== 0) {
          reject(new Error(`MiMC worker exited with code ${code}`));
        }
      });
    });
  }

  test("preimage count 2 digest count 3", () => {
    const expected = [
      "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f",
      "2f7d340a3c24b8ef9899ab5f019b85b87354c7f6c965a19ca090321f7e5425e9",
      "0cf71423c39e70b9858eaa8e1dc3ac40a09c3927dc31d12014af16066f2bdcb6",
    ];
    const digests = hash.mimc_sponge([Uint8Array.of(0x01), Uint8Array.of(0x02)], expected.length, new Uint8Array());
    assert.equal(digests.length, expected.length);
    expected.forEach((value, index) => assert.equal(hex(digests[index]), value));
  });

  test("preimage count 2 digest count 3 key 0x09", () => {
    const expected = [
      "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77",
      "10c7e728e6219df6431c1e55c788e777d339815553bd8c2560a628288229286f",
      "1208620a5c351a5a88dfdb6d7386d1069081d21f4cccce630edb0cb7196009c1",
    ];
    const digests = hash.mimc_sponge([Uint8Array.of(0x01), Uint8Array.of(0x02)], expected.length, Uint8Array.of(0x09));
    expected.forEach((value, index) => assert.equal(hex(digests[index]), value));
  });

  test("preimage and key prime minus 1", () => {
    const expected = [
      "12af600bcba99cb94f3720cdbf071d86b090bc3571bc9a91d58fc7213e444741",
      "25e469d12f79c571a2ba5c3aac42b32c1b83ff49afab33298abc2184148cac0e",
      "01dab6a98cca214537bf2ca607f3475ce2a5a64b9540dec4b14279b706060cdf",
    ];
    const primeMinusOne = Uint8Array.from(Buffer.from(leftPadHex(prime.bn254() - 1n), "hex"));
    const digests = hash.mimc_sponge([primeMinusOne, primeMinusOne], expected.length, primeMinusOne);
    expected.forEach((value, index) => assert.equal(hex(digests[index]), value));
  });

  test("preimage prime and prime plus 1 key prime plus 2", () => {
    const expected = [
      "068fac99307e94389058d13efa9b1ff21aaa66fc6bf2fed373f5f1e32bffe2e5",
      "18aeef5dc576db758b5d2acac9a75ddcf0671862f6709f7f0eab822d02031166",
      "151770cf9b1209c4a3fe5fd5ef8f81dd4ab555787d7e5ae7c7e7d976a15763a3",
    ];
    const p = prime.bn254();
    const preimages = [
      Uint8Array.from(Buffer.from(leftPadHex(p), "hex")),
      Uint8Array.from(Buffer.from(leftPadHex(p + 1n), "hex")),
    ];
    const key = Uint8Array.from(Buffer.from(leftPadHex(p + 2n), "hex"));
    const digests = hash.mimc_sponge(preimages, expected.length, key);
    expected.forEach((value, index) => assert.equal(hex(digests[index]), value));
  });

  test("preimage and key prime with output count 220", () => {
    const p = prime.bn254();
    const encoded = Uint8Array.from(Buffer.from(leftPadHex(p), "hex"));
    const digests = hash.mimc_sponge([encoded, encoded], 220, encoded);
    assert.equal(digests.length, 220);
    assert.equal(hex(digests[0]), "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581");
    assert.equal(hex(digests[219]), "0b437ab068775e930f36c52caea8491e1aa1fc5260ce51c03aa261d4d51ff81c");
  });

  test("digest padding under big and little endian", () => {
    const preimages = [
      Uint8Array.from(Buffer.from("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "hex")),
      Uint8Array.from(Buffer.from("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "hex")),
    ];
    const key = Uint8Array.from(Buffer.from("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "hex"));

    const expectedBe = [
      "04030c4eec2927d034bb5a92e413b7fa6f67d9a05d542f96983a860d57cb085d",
      "00b38029c8451ff2f4a9c5298aed364d2a978bdb26c7ddd3573b739e9ce68000",
      "28b6ea95e68dd403f66be53f8af9a4a7185bcbf0675f077ff132056214a5cc8e",
    ];
    const beDigests = hash.mimc_sponge(preimages, expectedBe.length, key, Endian.BE, Endian.BE, Endian.BE);
    expectedBe.forEach((value, index) => assert.equal(hex(beDigests[index]), value));

    const expectedLe = expectedBe.map((value) => Buffer.from(value, "hex").reverse().toString("hex"));
    const leDigests = hash.mimc_sponge(preimages, expectedLe.length, key, Endian.BE, Endian.BE, Endian.LE);
    expectedLe.forEach((value, index) => assert.equal(hex(leDigests[index]), value));
  });

  test("worker thread calls remain stable", async () => {
    // Keep the loop count moderate so the test still exercises repeated cross-thread use without bloating runtime.
    await Promise.all([runWorker("job0", 200), runWorker("job1", 200)]);
  });

  test("preimage 0xff 2x32 bytes", () => {
    const expected = [
      "19f45c7a923c585cbc59505939367acd7960685fc90d661752b1fccd32ae6fd0",
      "0fa61f33087dfcc04b2d920826e744a85f42dee1f9e019d961cc2bf5342e9b0c",
      "17bfe4bdadbe239c72b96e97fd37f122169137fb1591242060ecac771f10a679",
    ];
    const allFF = Uint8Array.from(Buffer.from("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", "hex"));
    const digests = hash.mimc_sponge([allFF, allFF], expected.length, allFF);
    expected.forEach((value, index) => assert.equal(hex(digests[index]), value));
  });

  test("digest container is not reused across calls", () => {
    const primeBn254 = Uint8Array.from(Buffer.from(leftPadHex(prime.bn254()), "hex"));
    let digests = hash.mimc_sponge([primeBn254, primeBn254], 1, new Uint8Array());
    assert.equal(hex(digests[0]), "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581");

    digests = hash.mimc_sponge([Uint8Array.of(0x01), Uint8Array.of(0x02)], 1, Uint8Array.of(0x09));
    assert.equal(digests.length, 1);
    assert.equal(hex(digests[0]), "1a18b5636f54e0268de295548e1ddc4ce94f930ccc7ca67422d21cfb43f70a77");
  });
});
