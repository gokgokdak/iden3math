export type BigNumberish = bigint | number | string;
export type BytesLike = Uint8Array | ArrayBuffer | ArrayLike<number> | ArrayBufferView;
export type EndianValue = number;

export interface PointInstance {
  x(): bigint;
  y(): bigint;
  equals(other: PointInstance): boolean;
  notEquals(other: PointInstance): boolean;
}

export interface PointConstructor {
  new (x?: BigNumberish, y?: BigNumberish): PointInstance;
}

export interface ExtPointInstance {
  X(): bigint;
  Y(): bigint;
  Z(): bigint;
  T(): bigint;
  equals(other: ExtPointInstance): boolean;
  notEquals(other: ExtPointInstance): boolean;
}

export interface ExtPointConstructor {
  new (X?: BigNumberish, Y?: BigNumberish, Z?: BigNumberish, T?: BigNumberish): ExtPointInstance;
}

export interface BitVec1DInstance {
  size(): number;
  push(bit: boolean): void;
  get(index: number): boolean;
  at(index: number): boolean;
  readonly length: number;
}

export interface BitVec1DConstructor {
  new (): BitVec1DInstance;
}

export interface Fp1Instance {
  prime(): bigint;
  mod_reduce(a: BigNumberish): bigint;
  add(a: BigNumberish, b: BigNumberish): bigint;
  sub(a: BigNumberish, b: BigNumberish): bigint;
  mul(a: BigNumberish, b: BigNumberish): bigint;
  div(a: BigNumberish, b: BigNumberish): bigint | null;
  pow(a: BigNumberish, b: BigNumberish): bigint | null;
  square(a: BigNumberish): bigint;
  sqrt(a: BigNumberish): bigint | null;
  mod_inv(a: BigNumberish): bigint | null;
  neg(a: BigNumberish): bigint;
  has_sqrt(a: BigNumberish): boolean;
}

export interface Fp1Constructor {
  new (prime: BigNumberish): Fp1Instance;
}

export interface CurveInstance {
  field(): Fp1Instance;
  a(): bigint;
  d(): bigint;
}

export interface CurveConstructor {
  new (field: Fp1Instance, a: BigNumberish, d: BigNumberish): CurveInstance;
}

export interface Iden3MathModule {
  Bit: boolean;
  Byte: number;
  Endian: {
    LE: EndianValue;
    BE: EndianValue;
  };
  BitVec1D: BitVec1DConstructor;
  Fp1: Fp1Constructor;
  prime: {
    bn254(): bigint;
  };
  random: {
    get_bytes(len: number): Uint8Array;
    get_integer(p: BigNumberish): bigint;
  };
  hash: {
    blake256(input: string | BytesLike): Uint8Array;
    keccak256(input: string | BytesLike | BigNumberish): Uint8Array;
    mimc_sponge(
      preimages: BytesLike[],
      outputs: number,
      key: BytesLike,
      preimageEndian?: EndianValue,
      keyEndian?: EndianValue,
      digestEndian?: EndianValue,
    ): Uint8Array[];
    pedersen(preimage: BytesLike): Uint8Array;
  };
  ec: {
    Point: PointConstructor;
    babyjub: {
      prime(): bigint;
      finite_field(): Fp1Instance;
      group_order(): bigint;
      sub_group_order(): bigint;
      zero(): PointInstance;
      generator(): PointInstance;
      add(a: PointInstance, b: PointInstance): PointInstance;
      mul_scalar(point: PointInstance, scalar: BigNumberish): PointInstance;
      in_sub_group(point: PointInstance): boolean;
      in_curve(point: PointInstance): boolean;
      compress(point: PointInstance, endian: EndianValue): Uint8Array;
      decompress(packed: BytesLike, endian: EndianValue): PointInstance | null;
    };
    twisted_edwards: {
      Curve: CurveConstructor;
      ExtPoint: ExtPointConstructor;
      identity(): PointInstance;
      ext_identity(): ExtPointInstance;
      to_ext(curve: CurveInstance, point: PointInstance): ExtPointInstance;
      to_affine(curve: CurveInstance, point: ExtPointInstance): PointInstance | null;
      in_curve(curve: CurveInstance, point: PointInstance | ExtPointInstance): boolean;
      equivalent(curve: CurveInstance, a: ExtPointInstance, b: ExtPointInstance): boolean;
      add(curve: CurveInstance, a: PointInstance, b: PointInstance): PointInstance;
      add(curve: CurveInstance, a: ExtPointInstance, b: ExtPointInstance): ExtPointInstance;
      dbl(curve: CurveInstance, point: PointInstance): PointInstance;
      dbl(curve: CurveInstance, point: ExtPointInstance): ExtPointInstance;
      mul_scalar(curve: CurveInstance, point: PointInstance, scalar: BigNumberish): PointInstance;
      mul_scalar(curve: CurveInstance, point: ExtPointInstance, scalar: BigNumberish): ExtPointInstance;
    };
  };
}

export declare function createIden3Math(options?: Record<string, unknown>): Promise<Iden3MathModule>;

export default createIden3Math;
