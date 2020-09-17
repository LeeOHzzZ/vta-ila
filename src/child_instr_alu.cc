// file: child_instr_alu.cc

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

auto imm_type = SortRef::BV(VTA_ALU_IMM_BITWIDTH);
auto acc_type = SortRef::BV(VTA_ACCUM_BITWIDTH);
auto out_type = SortRef::BV(VTA_OUT_BITWIDTH);
FuncRef Imm2Acc("Imm2Acc", acc_type, imm_type);
FuncRef AccMin("AccMin", acc_type, acc_type, acc_type);
FuncRef AccMax("AccMax", acc_type, acc_type, acc_type);
FuncRef AccAdd("AccAdd", acc_type, acc_type, acc_type);
FuncRef AccShr("AccShr", acc_type, acc_type, acc_type);
FuncRef Acc2Out("Acc2Out", out_type, acc_type);


void DefineChildALU(Ila& m) {

  auto child = m.child("vta_child");

  // ---------- child states --------------//
  auto it_out = child.NewBvState(VTA_ALU_OUT_LOOP_CNTR, VTA_ALU_OUT_LOOP_CNTR_BITWIDTH);
  auto it_in = child.NewBvState(VTA_ALU_IN_LOOP_CNTR, VTA_ALU_IN_LOOP_CNTR_BTIWIDT);
  auto upc = child.NewBvState(VTA_ALU_UOP_CNTR, VTA_ALU_UOP_CNTR_BITWIDTH);
  auto i_cntr = 
    child.NewBvState(VTA_ALU_IN_LOOP_BATCH_CNTR, VTA_ALU_IN_LOOP_BATCH_CNTR_BITWIDTH);
  auto b_cntr = 
    child.NewBvState(VTA_ALU_IN_LOOP_BLOCK_CNTR, VTA_ALU_IN_LOOP_BLOCK_CNTR_BITWIDTH);
  
  auto dst_idx = child.NewBvState(VTA_ALU_DST_IDX, VTA_ALU_DST_IDX_BITWIDTH);
  auto src_idx = child.NewBvState(VTA_ALU_SRC_IDX, VTA_ALU_SRC_IDX_BITWIDTH);

  auto dst_offset_out = 
    child.NewBvState(VTA_ALU_DST_OFFSET_OUT, VTA_ALU_DST_OFFSET_OUT_BITWIDTH);
  auto dst_offset_in = 
    child.NewBvState(VTA_ALU_DST_OFFSET_IN, VTA_ALU_DST_OFFSET_IN_BITWIDTH);
  auto src_offset_out = 
    child.NewBvState(VTA_ALU_SRC_OFFSET_OUT, VTA_ALU_SRC_OFFSET_OUT_BITWIDTH);
  auto src_offset_in = 
    child.NewBvState(VTA_ALU_SRC_OFFSET_IN, VTA_ALU_SRC_OFFSET_IN_BITWIDTH);
  
  // child memory states
  auto src_tensor = 
    child.NewMemState(VTA_ALU_SRC_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_ALU_SRC_TENSOR_DATA_BITWIDTH);
  auto dst_tensor = 
    child.NewMemState(VTA_ALU_DST_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_ALU_DST_TENSOR_DATA_BITWIDTH);
  auto o_tensor = 
    child.NewMemState(VTA_ALU_OUT_TENSOR, VTA_MEMORY_ADDR_BITWIDTH, VTA_ALU_OUT_TENSOR_DATA_BITWIDTH);

  // -------------- parent states -------------//
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto child_valid_flag = m.state(VTA_CHILD_VALID_FLAG);
  auto alu_opcode = m.state(VTA_ALU_OPCODE);

  { // instruction alu start
    auto instr = child.NewInstr("vta_child_alu_start");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_START));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(dst_offset_out, BvConst(0, dst_offset_out.bit_width()));
    instr.SetUpdate(src_offset_out, BvConst(0, src_offset_out.bit_width()));
    instr.SetUpdate(dst_offset_in, BvConst(0, dst_offset_in.bit_width()));
    instr.SetUpdate(src_offset_in, BvConst(0, src_offset_in.bit_width()));

    instr.SetUpdate(it_out, BvConst(0, it_out.bit_width()));
    instr.SetUpdate(it_in, BvConst(0, it_in.bit_width()));
    auto uop_begin = m.state(VTA_ALU_UOP_BEGIN);
    auto upc_next = Concat(BvConst(0, upc.bit_width()-uop_begin.bit_width()), uop_begin);
    instr.SetUpdate(upc, upc_next);

    auto next_state = BvConst(VTA_CHILD_STATE_ALU_READ_UOP, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu out loop
    auto instr = child.NewInstr("vta_child_alu_out_loop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_ALU_OUT_LOOP));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(it_out, it_out+1);
    instr.SetUpdate(it_in, BvConst(0, it_in.bit_width()));
    //initiate uop cntr
    auto uop_begin = m.state(VTA_ALU_UOP_BEGIN);
    auto upc_next = Concat(BvConst(0, upc.bit_width()-uop_begin.bit_width()), uop_begin);
    instr.SetUpdate(upc, upc_next);

    // update offsets
    auto dst_offset_out_next = 
      dst_offset_out + Concat(BvConst(0,1), m.state(VTA_ALU_DST_FACTOR_OUT));
    auto src_offset_out_next = 
      src_offset_out + Concat(BvConst(0,1), m.state(VTA_ALU_SRC_FACTOR_OUT));
    instr.SetUpdate(dst_offset_out, dst_offset_out_next);
    instr.SetUpdate(src_offset_out, src_offset_out_next);

    auto iter_out = m.state(VTA_ALU_ITER_OUT);
    auto is_done = (it_out >= iter_out - 1);

    auto dst_offset_in_next = Ite(is_done, dst_offset_in, dst_offset_out_next);
    auto src_offset_in_next = Ite(is_done, src_offset_in, src_offset_out_next);
    instr.SetUpdate(dst_offset_in, dst_offset_in_next);
    instr.SetUpdate(src_offset_in, src_offset_in_next);

    auto next_state = Ite(it_out >= m.state(VTA_ALU_ITER_OUT) - 1,
                          BvConst(VTA_CHILD_STATE_ALU_DONE, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_ALU_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu in loop
    auto instr = child.NewInstr("vta_child_alu_in_loop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_ALU_IN_LOOP));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(it_in, it_in+1);
    // initiate uop cntr
    auto uop_begin = m.state(VTA_ALU_UOP_BEGIN);
    auto upc_next = Concat(BvConst(0, upc.bit_width()-uop_begin.bit_width()), uop_begin);
    instr.SetUpdate(upc, upc_next);

    // update offsets
    auto dst_offset_in_next = 
      dst_offset_in + Concat(BvConst(0,1), m.state(VTA_ALU_DST_FACTOR_IN));
    auto src_offset_in_next = 
      src_offset_in + Concat(BvConst(0,1), m.state(VTA_ALU_SRC_FACTOR_IN));
    instr.SetUpdate(dst_offset_in, dst_offset_in_next);
    instr.SetUpdate(src_offset_in, src_offset_in_next);

    auto next_state = Ite(it_in >= m.state(VTA_ALU_ITER_IN) - 1,
                          BvConst(VTA_CHILD_STATE_ALU_OUT_LOOP, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_ALU_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // instruction update upc
    auto instr = child.NewInstr("vta_child_alu_update_upc");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_ALU_UPDATE_UPC));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(upc, upc+1);

    // write the result back into memory
    auto acc_mem = m.state(VTA_ACCUM_MEMORY);
    auto out_mem = m.state(VTA_OUT_MEMORY);
    auto a_start_addr = 
      Concat(BvConst(0, 32-dst_idx.bit_width()), dst_idx) * VTA_ACCUM_MAT_DATA_NUM;
    auto o_start_addr = 
      Concat(BvConst(0, 32-dst_idx.bit_width()), dst_idx) * VTA_OUT_MAT_DATA_NUM;
    
    auto acc_mem_next = 
      write_tensor(acc_mem, dst_tensor, a_start_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    auto out_mem_next = 
      write_tensor(out_mem, o_tensor, o_start_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    
    instr.SetUpdate(acc_mem, acc_mem_next);
    instr.SetUpdate(out_mem, out_mem_next);

    auto uop_end = m.state(VTA_ALU_UOP_END);
    auto uop_end_ext = Concat(BvConst(0, upc.bit_width()-uop_end.bit_width()), uop_end);
    auto next_state = Ite(upc >= uop_end_ext-1,
                          BvConst(VTA_CHILD_STATE_ALU_IN_LOOP, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_ALU_READ_UOP, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // instruction read uop
    auto instr = child.NewInstr("vta_child_alu_read_uop");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_READ_UOP));
    instr.SetDecode(is_instr_valid);

    auto uop_mem = m.state(VTA_UOP_MEMORY);
    auto uop = Load(uop_mem, upc);

    // decode indices
    auto dst_idx_next = 
      Concat(BvConst(0,1), Extract(uop, VTA_UOP_ALU_0_1, VTA_UOP_ALU_0_0)) + dst_offset_in;
    auto src_idx_next = 
      Concat(BvConst(0,1), Extract(uop, VTA_UOP_ALU_1_1, VTA_UOP_ALU_1_0)) + src_offset_in;
    auto src_base_addr = 
      Concat(BvConst(0, 32-src_idx.bit_width()), src_idx_next) * VTA_ACCUM_MAT_DATA_NUM;
    auto dst_base_addr = 
      Concat(BvConst(0, 32-dst_idx.bit_width()), dst_idx_next) * VTA_ACCUM_MAT_DATA_NUM;
    
    auto acc_mem = m.state(VTA_ACCUM_MEMORY);
    auto src_tensor_next = read_tensor(acc_mem, src_tensor, src_base_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    auto dst_tensor_next = read_tensor(acc_mem, dst_tensor, dst_base_addr, VTA_BATCH_SIZE*VTA_BLOCK_OUT);
    instr.SetUpdate(src_tensor, src_tensor_next);
    instr.SetUpdate(dst_tensor, dst_tensor_next);

    // initiate inner loop cntr
    instr.SetUpdate(i_cntr, BvConst(0, i_cntr.bit_width()));
    instr.SetUpdate(b_cntr, BvConst(0, b_cntr.bit_width()));
    auto next_state = BvConst(VTA_CHILD_STATE_ALU_COMPUTE, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // instruction in loop batch idx
    auto instr = child.NewInstr("vta_child_alu_in_loop_batch_idx");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_IN_LOOP_BATCH_IDX));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(i_cntr, i_cntr+1);
    instr.SetUpdate(b_cntr, BvConst(0, b_cntr.bit_width()));

    auto next_state = Ite(i_cntr >= VTA_BATCH_SIZE - 1,
                          BvConst(VTA_CHILD_STATE_ALU_UPDATE_UPC, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_ALU_COMPUTE, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // instruction in loop block idx
    auto instr = child.NewInstr("vta_child_alu_in_loop_block_idx");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) & 
                            (state == VTA_CHILD_STATE_ALU_IN_LOOP_BLOCK_IDX));
    instr.SetDecode(is_instr_valid);

    instr.SetUpdate(b_cntr, b_cntr+1);
    auto next_state = Ite(b_cntr >= VTA_BLOCK_OUT - 1,
                          BvConst(VTA_CHILD_STATE_ALU_IN_LOOP_BATCH_IDX, state.bit_width()),
                          BvConst(VTA_CHILD_STATE_ALU_COMPUTE, state.bit_width()));
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu compute ---- min
    auto instr = child.NewInstr("vta_child_alu_compute_min");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_COMPUTE) &
                            (alu_opcode == VTA_ALU_OPCODE_MIN));
    instr.SetDecode(is_instr_valid);

    auto src_0 = Load(dst_tensor, i_cntr*b_cntr);
    auto imm = m.state(VTA_ALU_IMM);
    auto src_1 = Ite(m.state(VTA_ALU_USE_IMM_FLAG) == VTA_VALID,
                     Imm2Acc(imm), Load(src_tensor, i_cntr*b_cntr));
    auto result = AccMin(src_0, src_1);

    auto dst_tensor_next = Store(dst_tensor, i_cntr*b_cntr, result);
    auto o_tensor_next = Store(o_tensor, i_cntr*b_cntr, Acc2Out(result));
    instr.SetUpdate(dst_tensor, dst_tensor_next);
    instr.SetUpdate(o_tensor, o_tensor_next);

    auto next_state = BvConst(VTA_CHILD_STATE_ALU_IN_LOOP_BLOCK_IDX, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu compute ---- max
    auto instr = child.NewInstr("vta_child_alu_compute_max");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_COMPUTE) &
                            (alu_opcode == VTA_ALU_OPCODE_MAX));
    instr.SetDecode(is_instr_valid);

    auto src_0 = Load(dst_tensor, i_cntr*b_cntr);
    auto imm = m.state(VTA_ALU_IMM);
    auto src_1 = Ite(m.state(VTA_ALU_USE_IMM_FLAG) == VTA_VALID,
                     Imm2Acc(imm), Load(src_tensor, i_cntr*b_cntr));
    auto result = AccMax(src_0, src_1);

    auto dst_tensor_next = Store(dst_tensor, i_cntr*b_cntr, result);
    auto o_tensor_next = Store(o_tensor, i_cntr*b_cntr, Acc2Out(result));
    instr.SetUpdate(dst_tensor, dst_tensor_next);
    instr.SetUpdate(o_tensor, o_tensor_next);

    auto next_state = BvConst(VTA_CHILD_STATE_ALU_IN_LOOP_BLOCK_IDX, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu compute ---- add
    auto instr = child.NewInstr("vta_child_alu_compute_add");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_COMPUTE) &
                            (alu_opcode == VTA_ALU_OPCODE_ADD));
    instr.SetDecode(is_instr_valid);

    auto src_0 = Load(dst_tensor, i_cntr*b_cntr);
    auto imm = m.state(VTA_ALU_IMM);
    auto src_1 = Ite(m.state(VTA_ALU_USE_IMM_FLAG) == VTA_VALID,
                     Imm2Acc(imm), Load(src_tensor, i_cntr*b_cntr));
    auto result = AccAdd(src_0, src_1);

    auto dst_tensor_next = Store(dst_tensor, i_cntr*b_cntr, result);
    auto o_tensor_next = Store(o_tensor, i_cntr*b_cntr, Acc2Out(result));
    instr.SetUpdate(dst_tensor, dst_tensor_next);
    instr.SetUpdate(o_tensor, o_tensor_next);

    auto next_state = BvConst(VTA_CHILD_STATE_ALU_IN_LOOP_BLOCK_IDX, state.bit_width());
    instr.SetUpdate(state, next_state);
  }

  { // instruction alu compute ---- shr
    auto instr = child.NewInstr("vta_child_alu_compute_add");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_ALU_COMPUTE) &
                            (alu_opcode == VTA_ALU_OPCODE_SHR));
    instr.SetDecode(is_instr_valid);

    auto src_0 = Load(dst_tensor, i_cntr*b_cntr);
    auto imm = m.state(VTA_ALU_IMM);
    auto src_1 = Ite(m.state(VTA_ALU_USE_IMM_FLAG) == VTA_VALID,
                     Imm2Acc(imm), Load(src_tensor, i_cntr*b_cntr));
    auto result = AccShr(src_0, src_1);

    auto dst_tensor_next = Store(dst_tensor, i_cntr*b_cntr, result);
    auto o_tensor_next = Store(o_tensor, i_cntr*b_cntr, Acc2Out(result));
    instr.SetUpdate(dst_tensor, dst_tensor_next);
    instr.SetUpdate(o_tensor, o_tensor_next);

    auto next_state = BvConst(VTA_CHILD_STATE_ALU_IN_LOOP_BLOCK_IDX, state.bit_width());
    instr.SetUpdate(state, next_state);
  }
}


}
}