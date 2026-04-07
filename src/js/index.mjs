import createIden3MathRawModule from "./iden3math_raw.mjs";

function toDecimalString(value) {
  if (typeof value === "bigint") {
    return value.toString(10);
  }

  if (typeof value === "number") {
    if (!Number.isSafeInteger(value)) {
      throw new RangeError("Numbers must be safe integers. Use bigint or string for large values.");
    }
    return BigInt(value).toString(10);
  }

  if (typeof value === "string") {
    const normalized = value.trim();
    if (normalized.length === 0) {
      throw new TypeError("Expected a non-empty numeric string.");
    }
    return BigInt(normalized).toString(10);
  }

  throw new TypeError("Expected a bigint, safe integer, or numeric string.");
}

function toUint8Array(value) {
  if (value instanceof Uint8Array) {
    return value;
  }

  if (ArrayBuffer.isView(value)) {
    return new Uint8Array(value.buffer, value.byteOffset, value.byteLength);
  }

  if (value instanceof ArrayBuffer) {
    return new Uint8Array(value);
  }

  if (Array.isArray(value)) {
    return Uint8Array.from(value);
  }

  throw new TypeError("Expected a bytes-like value.");
}

function toNativeBigInt(value) {
  return BigInt(toDecimalString(value));
}

function assertIndex(index) {
  if (!Number.isInteger(index) || index < 0) {
    throw new RangeError("Index must be a non-negative integer.");
  }
  return index;
}

function pointToRaw(point) {
  if (!(point instanceof Point)) {
    throw new TypeError("Expected ec.Point.");
  }
  return {
    x: point.x().toString(10),
    y: point.y().toString(10),
  };
}

function pointFromRaw(point) {
  if (point === null || point === undefined) {
    return null;
  }
  return new Point(BigInt(point.x), BigInt(point.y));
}

function extPointToRaw(point) {
  if (!(point instanceof ExtPoint)) {
    throw new TypeError("Expected ec.twisted_edwards.ExtPoint.");
  }
  return {
    X: point.X().toString(10),
    Y: point.Y().toString(10),
    Z: point.Z().toString(10),
    T: point.T().toString(10),
  };
}

function extPointFromRaw(point) {
  if (point === null || point === undefined) {
    return null;
  }
  return new ExtPoint(BigInt(point.X), BigInt(point.Y), BigInt(point.Z), BigInt(point.T));
}

let Point = class {};
let ExtPoint = class {};

export async function createIden3Math(options = {}) {
  const raw = await createIden3MathRawModule(options);
  const Endian = Object.freeze({
    LE: raw.Endian.LE,
    BE: raw.Endian.BE,
  });

  function normalizeEndian(endian) {
    if (endian !== Endian.LE && endian !== Endian.BE) {
      throw new TypeError("Endian must be Endian.LE or Endian.BE.");
    }
    return endian.value;
  }

  Point = class Point {
    constructor(x = 0n, y = 0n) {
      this._x = toNativeBigInt(x);
      this._y = toNativeBigInt(y);
    }

    x() {
      return this._x;
    }

    y() {
      return this._y;
    }

    equals(other) {
      return other instanceof Point && this._x === other._x && this._y === other._y;
    }

    notEquals(other) {
      return !this.equals(other);
    }
  };

  ExtPoint = class ExtPoint {
    constructor(X = 0n, Y = 1n, Z = 1n, T = 0n) {
      this._X = toNativeBigInt(X);
      this._Y = toNativeBigInt(Y);
      this._Z = toNativeBigInt(Z);
      this._T = toNativeBigInt(T);
    }

    X() {
      return this._X;
    }

    Y() {
      return this._Y;
    }

    Z() {
      return this._Z;
    }

    T() {
      return this._T;
    }

    equals(other) {
      return (
        other instanceof ExtPoint &&
        this._X === other._X &&
        this._Y === other._Y &&
        this._Z === other._Z &&
        this._T === other._T
      );
    }

    notEquals(other) {
      return !this.equals(other);
    }
  };

  class BitVec1D {
    constructor() {
      this._raw = new raw.RawBitVec1D();
    }

    size() {
      return this._raw.size();
    }

    push(bit) {
      this._raw.push(Boolean(bit));
    }

    get(index) {
      return this._raw.get(assertIndex(index));
    }

    at(index) {
      return this.get(index);
    }

    get length() {
      return this.size();
    }
  }

  class Fp1 {
    constructor(prime) {
      this._primeString = toDecimalString(prime);
      this._prime = BigInt(this._primeString);
      this._raw = new raw.RawFp1(this._primeString);
    }

    prime() {
      return this._prime;
    }

    mod_reduce(a) {
      return BigInt(this._raw.mod_reduce(toDecimalString(a)));
    }

    add(a, b) {
      return BigInt(this._raw.add(toDecimalString(a), toDecimalString(b)));
    }

    sub(a, b) {
      return BigInt(this._raw.sub(toDecimalString(a), toDecimalString(b)));
    }

    mul(a, b) {
      return BigInt(this._raw.mul(toDecimalString(a), toDecimalString(b)));
    }

    div(a, b) {
      const result = this._raw.div(toDecimalString(a), toDecimalString(b));
      return result === null ? null : BigInt(result);
    }

    pow(a, b) {
      const result = this._raw.pow(toDecimalString(a), toDecimalString(b));
      return result === null ? null : BigInt(result);
    }

    square(a) {
      return BigInt(this._raw.square(toDecimalString(a)));
    }

    sqrt(a) {
      const result = this._raw.sqrt(toDecimalString(a));
      return result === null ? null : BigInt(result);
    }

    mod_inv(a) {
      const result = this._raw.mod_inv(toDecimalString(a));
      return result === null ? null : BigInt(result);
    }

    neg(a) {
      return BigInt(this._raw.neg(toDecimalString(a)));
    }

    has_sqrt(a) {
      return this._raw.has_sqrt(toDecimalString(a));
    }
  }

  class Curve {
    constructor(field, a, d) {
      if (!(field instanceof Fp1)) {
        throw new TypeError("Curve field must be an instance of Fp1.");
      }

      this._field = field;
      this._a = toNativeBigInt(a);
      this._d = toNativeBigInt(d);
      this._raw = new raw.RawCurve(field._primeString, this._a.toString(10), this._d.toString(10));
    }

    field() {
      return this._field;
    }

    a() {
      return this._a;
    }

    d() {
      return this._d;
    }
  }

  const prime = Object.freeze({
    bn254() {
      return BigInt(raw.prime_bn254());
    },
  });

  const random = Object.freeze({
    get_bytes(len) {
      return raw.random_get_bytes(len);
    },

    get_integer(p) {
      return BigInt(raw.random_get_integer(toDecimalString(p)));
    },
  });

  const hash = Object.freeze({
    blake256(input) {
      if (typeof input === "string") {
        return raw.hash_blake256_text(input);
      }
      return raw.hash_blake256_bytes(toUint8Array(input));
    },

    keccak256(input) {
      if (typeof input === "string") {
        return raw.hash_keccak256_text(input);
      }
      if (typeof input === "bigint" || typeof input === "number") {
        return raw.hash_keccak256_bigint(toDecimalString(input));
      }
      return raw.hash_keccak256_bytes(toUint8Array(input));
    },

    mimc_sponge(preimages, outputs, key, preimageEndian = Endian.BE, keyEndian = Endian.BE, digestEndian = Endian.BE) {
      const normalizedPreimages = preimages.map((preimage) => toUint8Array(preimage));
      return raw.hash_mimc_sponge(
        normalizedPreimages,
        outputs,
        toUint8Array(key),
        normalizeEndian(preimageEndian),
        normalizeEndian(keyEndian),
        normalizeEndian(digestEndian),
      );
    },

    pedersen(preimage) {
      return raw.hash_pedersen(toUint8Array(preimage));
    },
  });

  const babyjubField = new Fp1(prime.bn254());

  const babyjub = Object.freeze({
    prime() {
      return BigInt(raw.babyjub_prime());
    },

    finite_field() {
      return babyjubField;
    },

    group_order() {
      return BigInt(raw.babyjub_group_order());
    },

    sub_group_order() {
      return BigInt(raw.babyjub_sub_group_order());
    },

    zero() {
      return pointFromRaw(raw.babyjub_zero());
    },

    generator() {
      return pointFromRaw(raw.babyjub_generator());
    },

    add(a, b) {
      return pointFromRaw(raw.babyjub_add(pointToRaw(a), pointToRaw(b)));
    },

    mul_scalar(point, scalar) {
      return pointFromRaw(raw.babyjub_mul_scalar(pointToRaw(point), toDecimalString(scalar)));
    },

    in_sub_group(point) {
      return raw.babyjub_in_sub_group(pointToRaw(point));
    },

    in_curve(point) {
      return raw.babyjub_in_curve(pointToRaw(point));
    },

    compress(point, endian) {
      return raw.babyjub_compress(pointToRaw(point), normalizeEndian(endian));
    },

    decompress(packed, endian) {
      return pointFromRaw(raw.babyjub_decompress(toUint8Array(packed), normalizeEndian(endian)));
    },
  });

  const twistedEdwards = Object.freeze({
    Curve,
    ExtPoint,

    identity() {
      return pointFromRaw(raw.twisted_edwards_identity());
    },

    ext_identity() {
      return extPointFromRaw(raw.twisted_edwards_ext_identity());
    },

    to_ext(curve, point) {
      return extPointFromRaw(raw.twisted_edwards_to_ext(curve._raw, pointToRaw(point)));
    },

    to_affine(curve, point) {
      return pointFromRaw(raw.twisted_edwards_to_affine(curve._raw, extPointToRaw(point)));
    },

    in_curve(curve, point) {
      if (point instanceof Point) {
        return raw.twisted_edwards_in_curve_affine(curve._raw, pointToRaw(point));
      }
      if (point instanceof ExtPoint) {
        return raw.twisted_edwards_in_curve_ext(curve._raw, extPointToRaw(point));
      }
      throw new TypeError("Point must be ec.Point or ec.twisted_edwards.ExtPoint.");
    },

    equivalent(curve, a, b) {
      return raw.twisted_edwards_equivalent(curve._raw, extPointToRaw(a), extPointToRaw(b));
    },

    add(curve, a, b) {
      if (a instanceof Point && b instanceof Point) {
        return pointFromRaw(raw.twisted_edwards_add_affine(curve._raw, pointToRaw(a), pointToRaw(b)));
      }
      if (a instanceof ExtPoint && b instanceof ExtPoint) {
        return extPointFromRaw(raw.twisted_edwards_add_ext(curve._raw, extPointToRaw(a), extPointToRaw(b)));
      }
      throw new TypeError("Point kinds must match for addition.");
    },

    dbl(curve, point) {
      if (point instanceof Point) {
        return pointFromRaw(raw.twisted_edwards_dbl_affine(curve._raw, pointToRaw(point)));
      }
      if (point instanceof ExtPoint) {
        return extPointFromRaw(raw.twisted_edwards_dbl_ext(curve._raw, extPointToRaw(point)));
      }
      throw new TypeError("Point must be ec.Point or ec.twisted_edwards.ExtPoint.");
    },

    mul_scalar(curve, point, scalar) {
      if (point instanceof Point) {
        return pointFromRaw(raw.twisted_edwards_mul_scalar_affine(curve._raw, pointToRaw(point), toDecimalString(scalar)));
      }
      if (point instanceof ExtPoint) {
        return extPointFromRaw(raw.twisted_edwards_mul_scalar_ext(curve._raw, extPointToRaw(point), toDecimalString(scalar)));
      }
      throw new TypeError("Point must be ec.Point or ec.twisted_edwards.ExtPoint.");
    },
  });

  const ec = Object.freeze({
    Point,
    babyjub,
    twisted_edwards: twistedEdwards,
  });

  return Object.freeze({
    Bit: false,
    Byte: 0,
    Endian,
    BitVec1D,
    Fp1,
    prime,
    random,
    hash,
    ec,
  });
}

export default createIden3Math;
