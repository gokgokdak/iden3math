import assert from "node:assert/strict";
import fs from "node:fs/promises";
import path from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const thisDir = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.resolve(thisDir, "..", "..");
const distCandidates = [
  path.join(repoRoot, "product", "javascript", "npm", "dist"),
  path.join(repoRoot, "build-wasm", "npm", "dist"),
  path.join(repoRoot, "build", "npm", "dist")
];

async function resolveDistDir() {
  for (const candidate of distCandidates) {
    try {
      await fs.access(path.join(candidate, "index.mjs"));
      return candidate;
    } catch {
      continue;
    }
  }
  throw new Error("iden3math wasm artifacts were not found. Build the wasm target before running JS tests.");
}

const distDir = await resolveDistDir();
const moduleUrl = pathToFileURL(path.join(distDir, "index.mjs")).href;
const { default: createIden3Math } = await import(moduleUrl);

export const iden3math = await createIden3Math();
export const { BitVec1D, Endian, Fp1, hash, prime, random, ec } = iden3math;

export function bytesFromHex(hex) {
  return Uint8Array.from(Buffer.from(hex, "hex"));
}

export function hex(bytes) {
  return Buffer.from(bytes).toString("hex");
}

export function leftPadHex(value) {
  const hexString = value.toString(16);
  return hexString.length % 2 === 0 ? hexString : `0${hexString}`;
}

export function assertPointEqual(actual, expected) {
  assert.equal(actual.x(), expected.x());
  assert.equal(actual.y(), expected.y());
}

export function assertExtPointEqual(actual, expected) {
  assert.equal(actual.X(), expected.X());
  assert.equal(actual.Y(), expected.Y());
  assert.equal(actual.Z(), expected.Z());
  assert.equal(actual.T(), expected.T());
}
