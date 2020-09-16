// file: child_instr_gemm.cc
#include <ilang/ilang++.h>
#include <vta/vta_top.h>
#include <vector>

namespace ilang {
namespace vta {

auto wgt_elem = SortRef::BV(VTA_WEIGHT_BITWIDTH);
auto inp_elem = SortRef::BV(VTA_INPUT_DATA_BITWIDTH);
auto acc_elem = SortRef::BV(VTA_ACCUM_BITWIDTH);
auto out_elem = SortRef::BV(VTA_OUT_BITWIDTH);
auto tmp_elem = SortRef::BV(VTA_GEMM_MATMUL_SUM_TEMP_BITWIDTH);

std::vector<SortRef> gemm_mac_in = {tmp_elem, wgt_elem, inp_elem};
FuncRef GemmMac("GemmMac", tmp_elem, gemm_mac_in);
FuncRef AccumAddSum("AccumAddSum", acc_elem, acc_elem, tmp_elem);
FuncRef Accum2Out("Accum2Out", out_elem, acc_elem);

void DefineChildGEMM(Ila& m) {
  auto child = m.child("vta_child");

  // -------------- child states --------------//
  auto it_out = child.NewBvState(VTA_GEMM_OUT_LOOP_CNTR, VTA_GEMM_OUT_LOOP_CNTR_BITWIDTH);
  auto it_in = child.NewBvState(VTA_GEMM_IN_LOOP_CNTR, VTA_GEMM_IN_LOOP_CNTR_BITWIDTH);
  auto upc = child.NewBvState(VTA_GEMM_UOP_CNTR, VTA_GEMM_UOP_CNTR_BITWIDTH);

  auto dst_idx = child.NewBvState(VTA_GEMM_DST_IDX, VTA_GEMM_DST_IDX_BITWIDTH);
  auto src_idx = child.NewBvState(VTA_GEMM_SRC_IDX, VTA_GEMM_SRC_IDX_BITWIDTH);
  auto wgt_idx = child.NewBvState(VTA_GEMM_WGT_IDX, VTA_GEMM_WGT_IDX_BITWIDTH);

  auto dst_offset_out = 
    child.NewBvState(VTA_GEMM_DST_OFFSET_OUT, VTA_GEMM_DST_OFFSET_OUT_BITWIDTH);
  auto src_offset_out = 
    child.NewBvState(VTA_GEMM_SRC_OFFSET_OUT, VTA_GEMM_SRC_OFFSET_OUT_BITWIDTH);
  auto wgt_offset_out = 
    child.NewBvState(VTA_GEMM_WGT_OFFSET_OUT, VTA_GEMM_WGT_OFFSET_OUT_BITWIDTH);
  
  auto dst_offset_in = 
    child.NewBvState(VTA_GEMM_DST_OFFSET_IN, VTA_GEMM_DST_OFFSET_IN_BITWIDTH);
  auto src_offset_in =
    child.NewBvState(VTA_GEMM_SRC_OFFSET_IN, VTA_GEMM_SRC_OFFSET_IN_BITWIDTH);
  auto wgt_offset_in = 
    child.NewBvState(VTA_GEMM_WGT_OFFSET_IN, VTA_GEMM_WGT_OFFSET_IN_BITWIDTH);

  auto b_cntr = 
    child.NewBvState(VTA_GEMM_INNER_LOOP_BATCH_CNTR, VTA_GEMM_INNER_LOOP_BATCH_CNTR_BITWIDTH);
  auto oc_cntr = 
    child.NewBvState(VTA_GEMM_INNER_LOOP_OC_CNTR, VTA_GEMM_INNER_LOOP_OC_CNTR_BITWIDTH);
  auto ic_cntr = 
    child.NewBvState(VTA_GEMM_INNER_LOOP_IC_CNTR, VTA_GEMM_INNER_LOOP_IC_CNTR_BITWIDTH);
  auto sum_temp = 
    child.NewBvState(VTA_GEMM_MATMUL_SUM_TEMP, VTA_GEMM_MATMUL_SUM_TEMP_BITWIDTH);


  // -------------- child memory states -------//
  auto w_tensor = 
    child.NewMemState(VTA_GEMM_WGT_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_GEMM_WGT_TENSOR_DATA_BTIWIDTH);
  w_tensor.SetEntryNum(VTA_GEMM_WGT_TENSOR_ENTRY_NUM);
  auto i_tensor = 
    child.NewMemState(VTA_GEMM_INP_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_GEMM_INP_TENSOR_DATA_BITWIDTH);
  i_tensor.SetEntryNum(VTA_GEMM_INP_TENSOR_ENTRY_NUM);
  auto a_tensor = 
    child.NewMemState(VTA_GEMM_ACCUM_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_GEMM_ACCUM_TENSOR_DATA_BITWIDTH);
  a_tensor.SetEntryNum(VTA_GEMM_ACCUM_TENSOR_ENTRY_NUM);
  auto o_tensor = 
    child.NewMemState(VTA_GEMM_OUT_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_GEMM_OUT_TENSOR_DATA_BITWIDTH);
  o_tensor.SetEntryNum(VTA_GEMM_OUT_TENSOR_ENTRY_NUM);

  // -------------- parent states -------------//
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto child_valid_flag = m.state(VTA_CHILD_VALID_FLAG);

  { // child instruction ---- set up gemm child initial states
    auto instr = child.NewInstr("vta_child_gemm_init");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_START));
    instr.SetDecode(is_instr_valid);

    // initiate control parameters
    instr.SetUpdate(dst_offset_out, BvConst(0, dst_offset_out.bit_width()));
    instr.SetUpdate(src_offset_out, BvConst(0, src_offset_out.bit_width()));
    instr.SetUpdate(wgt_offset_out, BvConst(0, wgt_offset_out.bit_width()));

    instr.SetUpdate(dst_offset_in, BvConst(0, dst_offset_in.bit_width()));
    instr.SetUpdate(src_offset_in, BvConst(0, src_offset_in.bit_width()));
    instr.SetUpdate(wgt_offset_in, BvConst(0, wgt_offset_in.bit_width())); 

    // initiate loop cntr
    instr.SetUpdate(it_in, BvConst(0, it_in.bit_width()));
    instr.SetUpdate(it_out, BvConst(0, it_out.bit_width()));
    instr.SetUpdate(upc, m.state(VTA_GEMM_UOP_BEGIN));
    
    auto next_state = BvConst(VTA_CHILD_STATE_GEMM_READ_UOP, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- gemm done
    auto instr = child.NewInstr("vta_child_gemm_done");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_DONE));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(state, BvConst(VTA_CHILD_STATE_IDLE, state.bit_width()));
    instr.SetUpdate(child_valid_flag, BvConst(VTA_INVALID, 1));
  }

  { // child instruction ---- increment gemm out loop parameter
    auto instr = child.NewInstr("vta_child_gemm_out_loop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_OUT_LOOP));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(it_out, it_out + 1);
    instr.SetUpdate(it_in, BvConst(0, it_in.bit_width()));
    // initiate uop cntr
    instr.SetUpdate(upc, m.state(VTA_GEMM_UOP_BEGIN));

    // update offsets
    auto dst_offset_out_next = 
      dst_offset_out + Concat(BvConst(0,1), m.state(VTA_GEMM_DST_FACTOR_OUT));
    auto src_offset_out_next = 
      src_offset_out + Concat(BvConst(0,1), m.state(VTA_GEMM_SRC_FACTOR_OUT));
    auto wgt_offset_out_next = 
      wgt_offset_out + Concat(BvConst(0,1), m.state(VTA_GEMM_WGT_FACTOR_OUT));
    
    instr.SetUpdate(dst_offset_out, dst_offset_out_next);
    instr.SetUpdate(src_offset_out, src_offset_out_next);
    instr.SetUpdate(wgt_offset_out, wgt_offset_out_next);

    auto iter_out = m.state(VTA_GEMM_ITER_OUT);
    auto is_done = (it_out >= iter_out - 1);

    auto dst_offset_in_next = Ite(is_done, dst_offset_in, dst_offset_out_next);
    auto src_offset_in_next = Ite(is_done, src_offset_in, src_offset_out_next);
    auto wgt_offset_in_next = Ite(is_done, wgt_offset_in, wgt_offset_out_next);
    
    instr.SetUpdate(dst_offset_in, dst_offset_in_next);
    instr.SetUpdate(src_offset_in, src_offset_in_next);
    instr.SetUpdate(wgt_offset_in, wgt_offset_in_next);
    
    auto next_state = Ite(is_done,
                          BvConst(VTA_CHILD_STATE_GEMM_DONE, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_GEMM_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- increment gemm in loop parameter
    auto instr = child.NewInstr("vta_child_gemm_in_loop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_GEMM_IN_LOOP));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(it_in, it_in + 1);
    // initiate uop cntr
    instr.SetUpdate(upc, m.state(VTA_GEMM_UOP_BEGIN));

    // update offsets
    auto dst_offset_in_next = 
      dst_offset_in + Concat(BvConst(0,1), m.state(VTA_GEMM_DST_FACTOR_IN));
    auto src_offset_in_next = 
      src_offset_in + Concat(BvConst(0,1), m.state(VTA_GEMM_SRC_FACTOR_IN));
    auto wgt_offset_in_next = 
      wgt_offset_in + Concat(BvConst(0,1), m.state(VTA_GEMM_WGT_FACTOR_IN));
    
    instr.SetUpdate(dst_offset_in, dst_offset_in_next);
    instr.SetUpdate(src_offset_in, src_offset_in_next);
    instr.SetUpdate(wgt_offset_in, wgt_offset_in_next);
    
    auto iter_in = m.state(VTA_GEMM_ITER_IN);
    auto next_state = Ite(it_in >= iter_in - 1,
                          BvConst(VTA_CHILD_STATE_GEMM_OUT_LOOP, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_GEMM_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- increment gemm upc
    auto instr = child.NewInstr("vta_child_gemm_update_upc");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_UPDATE_UPC));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(upc, upc+1);

    // write accum result back to acc_mem and out_mem
    auto acc_mem = m.state(VTA_ACCUM_MEMORY);
    auto out_mem = m.state(VTA_OUT_MEMORY);
    auto a_start_addr = 
      Concat(BvConst(0, 32-dst_idx.bit_width()), dst_idx) * VTA_ACCUM_MAT_DATA_NUM;
    auto o_start_addr = 
      Concat(BvConst(0, 32-dst_idx.bit_width()), dst_idx) * VTA_OUT_MAT_DATA_NUM;
    
    auto acc_mem_next = write_tensor(acc_mem, a_tensor, a_start_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    auto out_mem_next = write_tensor(out_mem, o_tensor, o_start_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);

    instr.SetUpdate(acc_mem, acc_mem_next);
    instr.SetUpdate(out_mem, out_mem_next);

    auto next_state = Ite(upc >= m.state(VTA_GEMM_UOP_END)-1,
                          BvConst(VTA_CHILD_STATE_GEMM_IN_LOOP, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_GEMM_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- fetch uop, load tensors
    auto instr = child.NewInstr("vta_child_gemm_read_uop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_READ_UOP));
    instr.SetDecode(is_instr_valid);

    auto uop_mem = m.state(VTA_UOP_MEMORY);
    auto uop = Load(uop_mem, Concat(BvConst(0, 32-upc.bit_width()), upc));

    // decode indices
    auto dst_idx_next = 
      Concat(BvConst(0,1), Extract(uop, VTA_UOP_GEM_0_1, VTA_UOP_GEM_0_0)) + dst_offset_in;
    auto src_idx_next = 
      Concat(BvConst(0,1), Extract(uop, VTA_UOP_GEM_1_1, VTA_UOP_GEM_1_0)) + src_offset_in;
    auto wgt_idx_next = 
      Concat(BvConst(0,1), Extract(uop, VTA_UOP_GEM_2_1, VTA_UOP_GEM_2_0)) + wgt_offset_in;
    // change the indices into memory base addr
    auto bias_base_addr = 
      Concat(BvConst(0, 32-dst_idx_next.bit_width()), dst_idx_next) * VTA_ACCUM_MAT_DATA_NUM;
    auto inp_base_addr = 
      Concat(BvConst(0, 32-src_idx_next.bit_width()), src_idx_next) * VTA_INP_MAT_DATA_NUM;
    auto wgt_base_addr = 
      Concat(BvConst(0, 32-wgt_idx_next.bit_width()), wgt_idx_next) * VTA_WGT_MAT_DATA_NUM;

    // read tensors
    auto wgt_mem = m.state(VTA_WEIGHT_MEMORY);
    auto w_tensor_next = read_tensor(wgt_mem, w_tensor, wgt_base_addr, VTA_BLOCK_OUT*VTA_BLOCK_IN);
    instr.SetUpdate(w_tensor, w_tensor_next);

    auto inp_mem = m.state(VTA_INPUT_MEMORY);
    auto i_tensor_next = read_tensor(inp_mem, i_tensor, inp_base_addr, VTA_BATCH_SIZE*VTA_BLOCK_IN);
    instr.SetUpdate(i_tensor, i_tensor_next);

    auto acc_mem = m.state(VTA_ACCUM_MEMORY);
    auto a_tensor_next = read_tensor(acc_mem, a_tensor, bias_base_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    instr.SetUpdate(a_tensor, a_tensor_next);

    // initiate inner loop temp
    instr.SetUpdate(sum_temp, BvConst(0, sum_temp.bit_width()));
    instr.SetUpdate(b_cntr, BvConst(0, b_cntr.bit_width()));
    instr.SetUpdate(oc_cntr, BvConst(0, oc_cntr.bit_width()));
    instr.SetUpdate(ic_cntr, BvConst(0, ic_cntr.bit_width()));
    // control states update
    auto next_state = BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_MATMUL, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- increment inner loop batch cntr
    auto instr = child.NewInstr("vta_child_gemm_inner_loop_batch_cntr");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_INNER_LOOP_BATCH_IDX));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(b_cntr, b_cntr+1);

    instr.SetUpdate(oc_cntr, BvConst(0, oc_cntr.bit_width()));
    instr.SetUpdate(ic_cntr, BvConst(0, ic_cntr.bit_width()));
    instr.SetUpdate(sum_temp, BvConst(0, sum_temp.bit_width()));

    auto next_state = 
      Ite(b_cntr >= VTA_BATCH_SIZE - 1,
          BvConst(VTA_CHILD_STATE_GEMM_UPDATE_UPC, state.bit_width()),
          BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_MATMUL, state.bit_width()));

    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- increment inner loop out_channel cntr
    auto instr = child.NewInstr("vta_child_gemm_inner_loop_oc_cntr");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_GEMM_INNER_LOOP_OC_IDX));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(oc_cntr, oc_cntr+1);
    instr.SetUpdate(ic_cntr, BvConst(0, ic_cntr.bit_width()));
    instr.SetUpdate(sum_temp, BvConst(0, sum_temp.bit_width()));

    // update sum
    auto accum = Load(a_tensor, b_cntr * oc_cntr);
    auto accum_next = AccumAddSum(accum, sum_temp);

    auto a_tensor_idx = b_cntr * oc_cntr;
    auto acc_elem_next = Ite(m.state(VTA_GEMM_RESET_FLAG) == VTA_VALID,
                             BvConst(0, VTA_ACCUM_BITWIDTH), accum_next);
    instr.SetUpdate(a_tensor, Store(a_tensor, a_tensor_idx, acc_elem_next));

    auto o_tensor_idx = b_cntr * oc_cntr;
    instr.SetUpdate(o_tensor, Store(o_tensor, o_tensor_idx, Accum2Out(accum_next)));

    auto next_state = 
      Ite(oc_cntr >= VTA_BLOCK_OUT - 1,
          BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_BATCH_IDX, state.bit_width()),
          BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_MATMUL, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- matrix multiplication
    auto instr = child.NewInstr("vta_child_gemm_inner_loop_matmul");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_INNER_LOOP_MATMUL));
    instr.SetDecode(is_instr_valid);

    auto w_tensor_idx = oc_cntr * ic_cntr;
    auto i_tensor_idx = b_cntr * ic_cntr;
    auto wgt = Load(w_tensor, w_tensor_idx);
    auto inp = Load(i_tensor, i_tensor_idx);
    std::vector<ExprRef> mac_in = {sum_temp, wgt, inp};
    
    instr.SetUpdate(sum_temp, GemmMac(mac_in));

    instr.SetUpdate(ic_cntr, ic_cntr + 1);

    auto next_state = 
      Ite(ic_cntr >= VTA_BLOCK_IN - 1,
          BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_OC_IDX, state.bit_width()),
          BvConst(VTA_CHILD_STATE_GEMM_INNER_LOOP_MATMUL, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }
  
}

}
}