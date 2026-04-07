import assert from "node:assert/strict";
import { describe, test } from "node:test";
import { hash, hex } from "./common.mjs";

function repeatedPatternBytes(byteLength) {
  const hexPattern = "0123456789abcdef";
  const requiredHexChars = byteLength * 2;
  return Buffer.from(hexPattern.repeat(Math.ceil(requiredHexChars / hexPattern.length)).slice(0, requiredHexChars), "hex");
}

describe("hash.keccak256", () => {
  const boundaryCases = [
    { name: "preimage_len_32_bytes", byteLength: 32, expected: "f5e0be5ec5f0820f83decedf083468519a4e3c1c7568ab0a5f4ed56a2d5037a4" },
    { name: "preimage_len_136_minus_9_bytes", byteLength: 136 - 9, expected: "e6ceb7d956ae7ea58ee995db9daed5a801adb6580b6b8f8d137cec1d81d7f012" },
    { name: "preimage_len_136_minus_8_bytes", byteLength: 136 - 8, expected: "2acdfc5b5522eb5fddb177479fa66c965d44470e83c55928b5332966acef9bde" },
    { name: "preimage_len_136_minus_7_bytes", byteLength: 136 - 7, expected: "05dbf308c902e6cde7effc6fa52940b1bc60754500830d59f1c5b998bc3c2d96" },
    { name: "preimage_len_136_minus_6_bytes", byteLength: 136 - 6, expected: "749d0dae56d7d4fdb0578780510c7347c2a4c04acd40d7e320d99dc792f162a5" },
    { name: "preimage_len_136_minus_5_bytes", byteLength: 136 - 5, expected: "b3072dc3f354380c15083d809387292cbf750810e038fc3065b37d6a4df14612" },
    { name: "preimage_len_136_minus_4_bytes", byteLength: 136 - 4, expected: "984165fb33e1f839a80dcea669789604563a4fea225e7442af7c6701e191a22b" },
    { name: "preimage_len_136_minus_3_bytes", byteLength: 136 - 3, expected: "14d637d537f2d071574f792efc60cf0f4e7dc40cdb6af6b721371e703b5e7049" },
    { name: "preimage_len_136_minus_2_bytes", byteLength: 136 - 2, expected: "96dae491dced22bc4c9871ff144fc2c37c446797754a5b9e645559d65474812c" },
    { name: "preimage_len_136_minus_1_bytes", byteLength: 136 - 1, expected: "3e4916729e2522af4937548f5848a5b49067eec910a0a6a890b0c71dde08854e" },
    { name: "preimage_len_136_bytes", byteLength: 136, expected: "3f7424fa94a2f8c5a733b86dac312d85685f9af3dea919694cc6a8abfc075460" },
    { name: "preimage_len_136_plus_1_bytes", byteLength: 136 + 1, expected: "bd039942db13a90eb8d5ee6e98c2af187e77bc1884d622e3d47fb573e80cefc2" },
    { name: "preimage_len_136_plus_2_bytes", byteLength: 136 + 2, expected: "4292ca8cca198d9e7c7b9faaeda70025d1cd422ac8fae5f52ec03a21b6199e6d" },
    { name: "preimage_len_136_plus_3_bytes", byteLength: 136 + 3, expected: "a8073f8bc83c1fc173ae747a1b217a894203678ee22f9e3b13199eb5df3b76fc" },
    { name: "preimage_len_136_plus_4_bytes", byteLength: 136 + 4, expected: "08f8d877b7332fc4e87c0551356501d7c7d005ff1d13790a7101f42ffc6a6025" },
    { name: "preimage_len_136_plus_5_bytes", byteLength: 136 + 5, expected: "f504209429ed038f73e928b93e60e1653823b2a4c08b486b8e37628efdcee71e" },
    { name: "preimage_len_136_plus_6_bytes", byteLength: 136 + 6, expected: "b1d3bdc6f4919c636c12da601a2f461e600dc64b5d11b16c8e006edda24f69c2" },
    { name: "preimage_len_136_plus_7_bytes", byteLength: 136 + 7, expected: "f96d97e3f7e634c63bba61fbebb433060f97c4cf47583313fbb54731a4a16429" },
    { name: "preimage_len_136_plus_8_bytes", byteLength: 136 + 8, expected: "bf752058f36f3f2165a8942fb07484fd94509eab97e60f3cb1824e425c5a8caf" },
    { name: "preimage_len_136_plus_9_bytes", byteLength: 136 + 9, expected: "38d0de9a467365424c9f11326d690bbc024283859772af2a3bddcc747c89d4f0" },
    { name: "preimage_len_136x2_minus_9_bytes", byteLength: (136 * 2) - 9, expected: "c9d0c4f5ae0f22eb76e8f5fa60d92f2c1c087ab7eb5f81489f33025387505fe2" },
    { name: "preimage_len_136x2_minus_8_bytes", byteLength: (136 * 2) - 8, expected: "5bbe6b4e86fdd5efec5b11e9eafb228b5b94f091790d2534ec565a8c54f00dd3" },
    { name: "preimage_len_136x2_minus_7_bytes", byteLength: (136 * 2) - 7, expected: "11641540ae8c9221c7ccbb7adad2e3adb4936ae7ccd18f05f784030dba8d0cad" },
    { name: "preimage_len_136x2_minus_6_bytes", byteLength: (136 * 2) - 6, expected: "7657e6402fd451bac4b96f89fee0c7cc169a6751486c1c4015435a0d3898703f" },
    { name: "preimage_len_136x2_minus_5_bytes", byteLength: (136 * 2) - 5, expected: "229d1feae90dc0c71a51940965c73c8ce3e5b5055f899e03dc8607f9fae7cdaf" },
    { name: "preimage_len_136x2_minus_4_bytes", byteLength: (136 * 2) - 4, expected: "b82ba8d9064ab4030dee1a5a907c5c109034c671425d8a7a778602454885efba" },
    { name: "preimage_len_136x2_minus_3_bytes", byteLength: (136 * 2) - 3, expected: "cbffcd0eed2b92b8df590fd545b374c2ff73b61d1e125dc669499d3488f35489" },
    { name: "preimage_len_136x2_minus_2_bytes", byteLength: (136 * 2) - 2, expected: "62e6bd6a4bfa901e8cc4a4bcce78acfaef4977362f4b1a3f965a3d6e3921d840" },
    { name: "preimage_len_136x2_minus_1_bytes", byteLength: (136 * 2) - 1, expected: "d1ee150ef029d86324d5f96055eb6732f54275c05665d0e73830aaa67688add1" },
    { name: "preimage_len_136x2_bytes", byteLength: 136 * 2, expected: "e8bfa50dd3d49e950a93c0369b6045f1ae418c1e728582974d295fd8b57945f6" },
    { name: "preimage_len_136x2_plus_1_bytes", byteLength: (136 * 2) + 1, expected: "6375a95f36969022961a34ef2dd5bc9bd1b91a630c11985b179095480affd4b8" },
    { name: "preimage_len_136x2_plus_2_bytes", byteLength: (136 * 2) + 2, expected: "5fd95ea2bf266e363b61a8452f6b963ffe246d04196b9861c4b5a4f668c1ca83" },
    { name: "preimage_len_136x2_plus_3_bytes", byteLength: (136 * 2) + 3, expected: "774b9925a7558f5c39f84f7aa74d0e8252baf8fb5a5c9b3d46513367483b7887" },
    { name: "preimage_len_136x2_plus_4_bytes", byteLength: (136 * 2) + 4, expected: "22a4f90af5fd121969752b19beb628d70991f1b833157859f32a4060b561130b" },
    { name: "preimage_len_136x2_plus_5_bytes", byteLength: (136 * 2) + 5, expected: "319e73fbbb347fc3dae38bdf7244874e4545a35389f8a985af6ed1e085c40290" },
    { name: "preimage_len_136x2_plus_6_bytes", byteLength: (136 * 2) + 6, expected: "b2616d4a3fca77610896d1395454f69abee05b49cb324aa863459294f9bbeb8c" },
    { name: "preimage_len_136x2_plus_7_bytes", byteLength: (136 * 2) + 7, expected: "cd67501890a6abab56d3974cffab263d31df302ec225205d6459907b13035ef7" },
    { name: "preimage_len_136x2_plus_8_bytes", byteLength: (136 * 2) + 8, expected: "8e9b8db6cf9accee40ce994742abc3fb729682af1c4c1dcec10d24756786dce3" },
    { name: "preimage_len_136x2_plus_9_bytes", byteLength: (136 * 2) + 9, expected: "0e9ad5950b77c84da66a8b7e91447a757fad7770522eacdbf116e23da7450e22" },
    { name: "preimage_len_5440_bytes", byteLength: 5440, expected: "1a66fd6d2d37cb05ba1d68d3c1a7bf93e0430e907495b4351f4eec25f882b751" },
  ];

  test("text vectors", () => {
    assert.equal(hex(hash.keccak256("keccak256")), "b7845733ba102a68c6eb21c3cd2feafafd1130de581d7e73be60b76d775b6704");
    assert.equal(
      hex(hash.keccak256("Transfer(address,address,uint256)")),
      "ddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef",
    );
  });

  test("digest container is not reused across calls", () => {
    assert.equal(hex(hash.keccak256("keccak256")), "b7845733ba102a68c6eb21c3cd2feafafd1130de581d7e73be60b76d775b6704");
    assert.equal(
      hex(hash.keccak256("Transfer(address,address,uint256)")),
      "ddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef",
    );
  });

  test("same container for input and output semantics", () => {
    let digest = hash.keccak256("keccak256");
    digest = hash.keccak256(digest);
    assert.equal(hex(digest), "c0168e0d8493e7a9939bce2051cd56fa67ad757c9af47ad0232d4a39ae760dd8");
  });

  test("1-byte inputs padded to 32 bytes", () => {
    const rightPadded = new Uint8Array(32);
    rightPadded[31] = 0xff;
    assert.equal(hex(hash.keccak256(rightPadded)), "e08ec2af2cfc251225e1968fd6ca21e4044f129bffa95bac3503be8bdb30a367");

    const leftPadded = new Uint8Array(32);
    leftPadded[0] = 0xff;
    assert.equal(hex(hash.keccak256(leftPadded)), "a282094698c0c30b930221fa2e1311ac7871cd86e61417eb71ac92e3d58ba4da");
  });

  test("numeric input", () => {
    assert.equal(hex(hash.keccak256(0n)), "bc36789e7a1e281436464229828f817d6612f7b477d66591ff96a9e064bcc98a");
  });

  test("short byte vectors", () => {
    const cases = [
      { input: new Uint8Array(), expected: "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470" },
      { input: Uint8Array.of(0x01), expected: "5fe7f977e71dba2ea1a68e21057beebb9be2ac30c6410aa38d4f3fbe41dcffd2" },
      { input: Uint8Array.of(0x01, 0x23), expected: "667d3611273365cfb6e64399d5af0bf332ec3e5d6986f76bc7d10839b680eb58" },
      { input: Uint8Array.of(0x01, 0x23, 0x45), expected: "aa4ba4b304228a9d05087e147c9e86d84c708bbbe62bb35b28dab74492f6c726" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67), expected: "652d86dcb7eecf8be8bf4f7fba8cdc4f9b3dddaeebbd5afdf530371de63c0a99" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67, 0x89), expected: "79fad56e6cf52d0c8c2c033d568fc36856ba2b556774960968d79274b0e6b944" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67, 0x89, 0xab), expected: "e2e02bc63edb7760389aee0d233f7d8d75237e03b81d6197f2ef1269ae547576" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd), expected: "d3c552a3b8b0452e4991ff4cd90c1aa1cdaa059826772e20ae4ced5ff5247010" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef), expected: "0c3d72390ac0ce0233c551a3c5278f8625ba996f5985dc8d612a9fc55f1de15a" },
      { input: Uint8Array.of(0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01), expected: "6e2d7e7a5b156cee60602ef6fde9a003b3410633c9e3fcc65611a89afdc2e7e0" },
    ];

    for (const { input, expected } of cases) {
      assert.equal(hex(hash.keccak256(input)), expected);
    }
  });

  test("boundary vectors", () => {
    assert.equal(boundaryCases.length, 40);

    for (const { name, byteLength, expected } of boundaryCases) {
      assert.equal(hex(hash.keccak256(repeatedPatternBytes(byteLength))), expected, name);
    }
  });
});
