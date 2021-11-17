#include <systemc>
#include <vta.h>
#include <ap_int.h>

sc_biguint<8> vta::Acc2Out(sc_biguint<32> in) {
  sc_bigint<32> in_s = in;
  ap_int<32> src = in.to_int();
  ap_int<8> out = src.range(7,0);
  sc_bigint<8> result_s = out.to_int();
  sc_biguint<8> result = result_s;
  return result;
}

sc_biguint<32> vta::AccAdd(sc_biguint<32> in_0, sc_biguint<32> in_1) {
  sc_bigint<32> in_0_s = in_0;
  sc_bigint<32> in_1_s = in_1;
  ap_int<32> src_0 = in_0_s.to_int();
  ap_int<32> src_1 = in_1_s.to_int();
  ap_int<32> add_val = src_0 + src_1;
  sc_bigint<32> result_s = add_val.to_int();
  sc_bigint<32> result = result_s;
  return result;
}

sc_biguint<32> vta::AccMax(sc_biguint<32> in_0, sc_biguint<32> in_1) {
  sc_bigint<32> in_0_s = in_0;
  sc_bigint<32> in_1_s = in_1;
  ap_int<32> src_0 = in_0_s.to_int();
  ap_int<32> src_1 = in_1_s.to_int();
  ap_int<32> out = (src_0 > src_1) ? src_0 : src_1;
  sc_bigint<32> result_s = out.to_int();
  sc_biguint<32> result = result_s;
  return result;
}

sc_biguint<32> vta::AccMin(sc_biguint<32> in_0, sc_biguint<32> in_1) {
  sc_bigint<32> in_0_s = in_0;
  sc_bigint<32> in_1_s = in_1;
  ap_int<32> src_0 = in_0_s.to_int();
  ap_int<32> src_1 = in_1_s.to_int();
  ap_int<32> out = (src_0 < src_1) ? src_0 : src_1;
  sc_bigint<32> result_s = out.to_int();
  sc_biguint<32> result = result_s;
  return result;
}

sc_biguint<32> vta::AccShr(sc_biguint<32> in_0, sc_biguint<32> in_1) {
  sc_bigint<32> in_0_s = in_0;
  sc_bigint<32> in_1_s = in_1;
  ap_int<32> src_0 = in_0_s.to_int();
  ap_int<32> src_1 = in_1_s.to_int();
  ap_int<5> shft_by = src_1.range(4, 0);
  ap_int<32> shr_val = src_0 >> shft_by;
  sc_bigint<32> result_s = shr_val.to_int();
  sc_biguint<32> result = result_s;
  return result;
}

sc_biguint<32> vta::AccMul(sc_biguint<32> in_0, sc_biguint<32> in_1) {
  sc_bigint<32> in_0_s = in_0;
  sc_bigint<32> in_1_s = in_1;
  ap_int<32> src_0 = in_0_s.to_int();
  ap_int<32> src_1 = in_1_s.to_int();
  ap_int<32> mul_out = src_0 * src_1;
  sc_bigint<32> result_s = mul_out.to_int();
  sc_biguint<32> result = result_s;
  return result;
}

sc_biguint<8> vta::Accum2Out(sc_biguint<32> accum_in) {
  sc_bigint<32> accum_in_s = accum_in;
  ap_int<32> accum = accum_in_s.to_int();
  ap_int<8> out = (ap_int<8>) accum.range(7, 0);
  sc_bigint<8> result_s = out.to_int();
  sc_biguint<8> result = result_s;
  return result;
}

sc_biguint<32> vta::AccumAddSum(sc_biguint<32> accum_in, sc_biguint<21> sum_in) {
  sc_bigint<32> accum_in_s = accum_in;
  sc_bigint<21> sum_in_s = sum_in;
  ap_int<32> accum = accum_in_s.to_int();
  ap_int<21> tmp = sum_in_s.to_int();
  accum = accum + (ap_int<32>)tmp;
  sc_bigint<32> result_s = accum.to_int();
  sc_biguint<32> result = result_s;
  return result;
}

sc_biguint<21> vta::GemmMac(sc_biguint<21> sum_in, sc_biguint<8> wgt_in, sc_biguint<8> inp_in) {
  sc_bigint<21> sum_in_s = sum_in;
  sc_bigint<8> wgt_in_s = wgt_in;
  sc_bigint<8> inp_in_s = inp_in;
  ap_int<21> sum = sum_in_s.to_int();
  ap_int<8> wgt = wgt_in_s.to_int();
  ap_int<8> inp = inp_in_s.to_int();
  ap_int<17> prod_dsp = wgt * inp;
  sum = sum + (ap_int<21>)prod_dsp;
  sc_bigint<21> result_s = sum.to_int();
  sc_biguint<21> result = result_s;
  return result;
}

sc_biguint<32> vta::Imm2Acc(sc_biguint<16> in) {
  sc_bigint<16> in_s = in;
  sc_bigint<32> result = in_s.to_int();
  return result;
}