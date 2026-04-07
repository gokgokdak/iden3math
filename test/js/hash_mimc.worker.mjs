import { parentPort, workerData } from "node:worker_threads";
import { hash, hex, leftPadHex, prime } from "./common.mjs";

function runJob0(loopCount) {
  let digests = [];
  for (let index = 0; index < loopCount; index += 1) {
    digests = hash.mimc_sponge([Uint8Array.of(0x01), Uint8Array.of(0x02)], 1, new Uint8Array());
  }
  if (digests.length !== 1 || hex(digests[0]) !== "2bcea035a1251603f1ceaf73cd4ae89427c47075bb8e3a944039ff1e3d6d2a6f") {
    throw new Error("worker job0 returned an unexpected MiMC digest");
  }
}

function runJob1(loopCount) {
  const primeBn254 = Uint8Array.from(Buffer.from(leftPadHex(prime.bn254()), "hex"));
  let digests = [];
  for (let index = 0; index < loopCount; index += 1) {
    digests = hash.mimc_sponge([primeBn254, primeBn254], 1, primeBn254);
  }
  if (digests.length !== 1 || hex(digests[0]) !== "2d9fea8398a61ea1997e7d748364c0fdb49412c4dbabc1578375ade642e85581") {
    throw new Error("worker job1 returned an unexpected MiMC digest");
  }
}

if (workerData.job === "job0") {
  runJob0(workerData.loopCount);
} else if (workerData.job === "job1") {
  runJob1(workerData.loopCount);
} else {
  throw new Error(`unknown worker job: ${workerData.job}`);
}

parentPort.postMessage({ ok: true });
