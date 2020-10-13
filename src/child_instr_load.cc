// file: child_instr_load_store.cc
// this file defines vta child instructions

#include <ilang/ilang++.h>
#include <vta/vta_top.h>
#include <ilang/util/log.h>

namespace ilang {
namespace vta {

void DefineChildInstrLoadStore(Ila& m) {
  auto child = m.child("vta_child");

  // ----------- child state ------------- //
  child.NewBvState(VTA_LOAD_SRAM_ADDR, VTA_LOAD_SRAM_ADDR_BITWIDTH);
  child.NewBvState(VTA_LOAD_DRAM_ADDR, VTA_LOAD_DRAM_ADDR_BITWIDTH);

  // ------------ parent state ------------ // 
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto valid_flag = m.state(VTA_CHILD_VALID_FLAG);
  
  auto sram_base = m.state(VTA_SRAM_ID);
  auto dram_base = m.state(VTA_DRAM_ID);
  auto y_size = m.state(VTA_Y_SIZE);
  auto x_size = m.state(VTA_X_SIZE);
  auto x_stride = m.state(VTA_X_STRIDE);
  auto x_pad_0 = m.state(VTA_X_PAD_0);
  auto x_pad_1 = m.state(VTA_X_PAD_1);
  auto y_pad_0 = m.state(VTA_Y_PAD_0);
  auto y_pad_1 = m.state(VTA_Y_PAD_1);

  // ------------ extended states ---------- //
  auto sram_base_32 = Concat(BvConst(0, 32-sram_base.bit_width()), sram_base);
  // auto dram_base_32 = Concat(BvConst(0, 32-dram_base.bit_width()), dram_base);
  auto dram_base_32 = dram_base;
  auto x_size_32 = Concat(BvConst(0, 32-x_size.bit_width()), x_size);
  auto x_stride_32 = Concat(BvConst(0, 32-x_stride.bit_width()), x_stride);

  // ------------------------------------------------------------------------
  // Load instruction for weights
  // ------------------------------------------------------------------------

  { // child instruction ---- LOAD WGT 0
    auto instr = child.NewInstr("vta_child_load_wgt_y_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_WGT_Y_SIZE));
    instr.SetDecode(is_instr_valid);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    auto y_cntr_32 = Concat(BvConst(0, 32-y_cntr.bit_width()), y_cntr);
    
    auto sram_addr = (sram_base_32 + y_cntr_32 * x_size_32) * VTA_WGT_MAT_DATA_NUM;
    auto dram_addr = (dram_base_32 + y_cntr_32 * x_stride_32) * VTA_WGT_MAT_DATA_NUM;

    instr.SetUpdate(child.state(VTA_LOAD_X_CNTR), 
                    BvConst(0, VTA_LOAD_X_CNTR_BITWIDTH));
    instr.SetUpdate(child.state(VTA_LOAD_SRAM_ADDR), sram_addr);
    instr.SetUpdate(child.state(VTA_LOAD_DRAM_ADDR), dram_addr);

    instr.SetUpdate(y_cntr, y_cntr + 1);

    auto next_state = BvConst(VTA_CHILD_STATE_LOAD_WGT_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);

  }

  ILA_INFO << "test";
  { // child instruction ---- LOAD WGT 1
    auto instr = child.NewInstr("vta_child_load_wgt_x_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_WGT_X_SIZE));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = child.state(VTA_LOAD_X_CNTR);
    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto dram_addr = child.state(VTA_LOAD_DRAM_ADDR);

    sram_addr = sram_addr + x_cntr_32 * VTA_WGT_MAT_DATA_NUM;
    dram_addr = dram_addr + x_cntr_32 * VTA_WGT_MAT_DATA_NUM;

    auto sram = m.state(VTA_WEIGHT_MEMORY);
    auto sram_next = sram;
    auto dram = m.state(VTA_VIRTUAL_DRAM_WEIGHT);
    auto dram_next = dram;

    for (auto i = 0; i < VTA_WGT_MAT_DATA_NUM; i++) {
      // sram_addr = sram_addr + i;
      // dram_addr = dram_addr + i;
      sram_next = Store(sram_next, sram_addr+i, Load(dram, dram_addr+i));
    }

    instr.SetUpdate(sram, sram_next);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    // end condition for x_cntr and y_cntr are different!
    auto is_x_end = (x_cntr >= x_size - 1);
    auto is_y_end = (y_cntr >= y_size);

    auto next_state = 
      Ite(is_x_end & is_y_end,
          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
      Ite(is_x_end, 
          BvConst(VTA_CHILD_STATE_LOAD_WGT_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
          BvConst(VTA_CHILD_STATE_LOAD_WGT_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH)));

    instr.SetUpdate(x_cntr, x_cntr + 1);
    instr.SetUpdate(state, next_state);

    instr.SetUpdate(valid_flag,
                    Ite(is_x_end & is_y_end,
                        BvConst(VTA_INVALID, VTA_CHILD_VALID_FLAG_BITWIDTH),
                        BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH)));
  }

  // ------------------------------------------------------------------------
  // Load instruction for inputs
  // ------------------------------------------------------------------------
  { // child instruction ---- load_inp_0
    // this instruction add padding for y_offset_0 in the sram
    auto instr = child.NewInstr("vta_child_load_inp_y_offset_0");
    auto is_instr_valid = 
      ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_0));
    instr.SetDecode(is_instr_valid);

    auto pad_cntr = m.state(VTA_LOAD_INP_PAD_CNTR);
    auto y_offset_0 = m.state(VTA_LOAD_Y_OFFSET_0);

    auto pad_cntr_32 = Concat(BvConst(0, 32-pad_cntr.bit_width()), pad_cntr);
    // auto sram_base_32 = Concat(BvConst(0, 32-sram_base.bit_width()), sram_base);
    // TODO: check this sram_addr
    auto sram_addr = (sram_base_32 + pad_cntr_32) * VTA_INP_MAT_DATA_NUM;
    auto sram = m.state(VTA_INPUT_MEMORY);
    auto sram_next = sram;

    for (auto i = 0; i < VTA_INP_MAT_DATA_NUM; i++) {
      // sram_addr = sram_addr + i;
      sram_next = Store(sram_next, sram_addr+i, BvConst(0, VTA_INPUT_MEMORY_DATA_BITWIDTH));
    }

    // update data states
    instr.SetUpdate(sram, sram_next);
    // update control related states
    auto next_pad_cntr = Ite(pad_cntr >= y_offset_0 - 1,
                             BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH), pad_cntr + 1);
    instr.SetUpdate(pad_cntr, next_pad_cntr);
    // instr.SetUpdate(child.state(VTA_LOAD_SRAM_ADDR), sram_addr + 1);

    auto next_state = 
      Ite(pad_cntr >= y_offset_0 - 1,
          BvConst(VTA_CHILD_STATE_LOAD_INP_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
          BvConst(VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_0, VTA_CHILD_INSTR_STATE_BITWIDTH));
  }

  { // child instruction ---- load_inp_1
    // this instruction set parameter for current loop 
    auto instr = child.NewInstr("vta_child_load_inp_y_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_Y_SIZE));
    instr.SetDecode(is_instr_valid);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    auto y_offset_0 = m.state(VTA_LOAD_Y_OFFSET_0);

    // set the sram_addr base
    auto y_offset_0_32 = Concat(BvConst(0, 32-y_offset_0.bit_width()), y_offset_0);
    auto y_cntr_32 = Concat(BvConst(0, 32-y_cntr.bit_width()), y_cntr);

    auto sram_addr = (sram_base_32 + y_offset_0_32 + y_cntr_32 * x_size_32) * VTA_INP_MAT_DATA_NUM;
    auto dram_addr = (dram_base_32 + y_cntr_32 * x_stride_32) * VTA_INP_MAT_DATA_NUM;

    instr.SetUpdate(y_cntr, y_cntr + 1);                 
    instr.SetUpdate(child.state(VTA_LOAD_X_CNTR), BvConst(0, VTA_LOAD_X_CNTR_BITWIDTH));
    
    instr.SetUpdate(child.state(VTA_LOAD_SRAM_ADDR), sram_addr);
    instr.SetUpdate(child.state(VTA_LOAD_DRAM_ADDR), dram_addr);

    instr.SetUpdate(m.state(VTA_LOAD_INP_PAD_CNTR),
                    BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH));

    auto next_state = BvConst(VTA_CHILD_STATE_LOAD_INP_X_PAD_0, VTA_CHILD_INSTR_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- load_inp_2
    // this instruction set the padding for x_pad_0
    auto instr = child.NewInstr("vta_child_load_inp_x_pad_0");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_X_PAD_0));
    instr.SetDecode(is_instr_valid);

    auto pad_cntr = m.state(VTA_LOAD_INP_PAD_CNTR);
    auto pad_cntr_32 = Concat(BvConst(0, 32-pad_cntr.bit_width()), pad_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto sram = m.state(VTA_INPUT_MEMORY);
    auto sram_next = sram;

    for (auto i = 0; i < VTA_INP_MAT_DATA_NUM; i++) {
      auto addr = sram_addr + i;
      sram_next = Store(sram_next, addr, BvConst(0, VTA_INPUT_MEMORY_DATA_BITWIDTH));
    }

    instr.SetUpdate(sram, sram_next);
    instr.SetUpdate(sram_addr, sram_addr + VTA_INP_MAT_DATA_NUM);

    auto x_pad_0_ext = Concat(BvConst(0, pad_cntr.bit_width()-x_pad_0.bit_width()), x_pad_0);
    auto next_pad_cntr = Ite(pad_cntr >= x_pad_0_ext - 1,
                              BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH), pad_cntr + 1);
    instr.SetUpdate(pad_cntr, next_pad_cntr);
    
    auto next_state = Ite(pad_cntr >= x_pad_0_ext - 1,
                          BvConst(VTA_CHILD_STATE_LOAD_INP_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
                          BvConst(VTA_CHILD_STATE_LOAD_INP_X_PAD_0, VTA_CHILD_INSTR_STATE_BITWIDTH));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- load_inp_3
    // this instruction copy input data from dram to sram
    auto instr = child.NewInstr("vta_child_load_inp_x_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_X_SIZE));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = child.state(VTA_LOAD_X_CNTR);
    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto dram_addr = child.state(VTA_LOAD_DRAM_ADDR);

    auto sram = m.state(VTA_INPUT_MEMORY);
    auto sram_next = sram;
    auto dram = m.state(VTA_VIRTUAL_DRAM_INPUT);
    auto dram_next = dram;

    for (auto i = 0; i < VTA_INP_MAT_DATA_NUM; i++) {
      auto addr_s = sram_addr + i;
      auto addr_d = dram_addr + i;
      sram_next = Store(sram_next, addr_s, Load(dram, addr_d));
    }

    instr.SetUpdate(sram, sram_next);
    instr.SetUpdate(x_cntr, x_cntr + 1);

    instr.SetUpdate(sram_addr, sram_addr + VTA_INP_MAT_DATA_NUM);
    instr.SetUpdate(dram_addr, dram_addr + VTA_INP_MAT_DATA_NUM);

    auto next_state = 
      Ite(x_cntr >= x_size - 1,
          BvConst(VTA_CHILD_STATE_LOAD_INP_X_PAD_1, VTA_CHILD_INSTR_STATE_BITWIDTH),
          BvConst(VTA_CHILD_STATE_LOAD_INP_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH));

    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- load_inp_4
    // this instruction add the padding for x_pad_1
    auto instr = child.NewInstr("vta_child_load_inp_x_pad_1");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_X_PAD_1));
    instr.SetDecode(is_instr_valid);

    auto pad_cntr = m.state(VTA_LOAD_INP_PAD_CNTR);
    auto pad_cntr_32 = Concat(BvConst(0, 32-pad_cntr.bit_width()), pad_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto sram = m.state(VTA_INPUT_MEMORY);
    auto sram_next = sram;

    for (auto i = 0; i < VTA_INP_MAT_DATA_NUM; i++) {
      auto addr = sram_addr + i;
      sram_next = Store(sram_next, addr, BvConst(0, VTA_INPUT_MEMORY_DATA_BITWIDTH));
    }

    instr.SetUpdate(sram, sram_next);
    instr.SetUpdate(sram_addr, sram_addr + VTA_INP_MAT_DATA_NUM);

    auto x_pad_1_ext = Concat(BvConst(0, pad_cntr.bit_width()-x_pad_1.bit_width()), x_pad_1);
    auto next_pad_cntr = Ite(pad_cntr >= x_pad_1_ext - 1,
                              BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH), pad_cntr + 1);
    instr.SetUpdate(pad_cntr, next_pad_cntr);
    
    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    auto is_pad_done = (pad_cntr >= x_pad_1_ext - 1);
    auto is_y_done = (y_cntr >= y_size);

    auto next_state = Ite(is_pad_done & is_y_done,
                          BvConst(VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_1, VTA_CHILD_INSTR_STATE_BITWIDTH),
                      Ite(is_pad_done,
                          BvConst(VTA_CHILD_STATE_LOAD_INP_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
                          BvConst(VTA_CHILD_STATE_LOAD_INP_X_PAD_1, VTA_CHILD_INSTR_STATE_BITWIDTH)));
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- load_inp_5
    // this instruction add the padding for y_pad_1
    auto instr = child.NewInstr("vta_child_load_y_offset_1");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_1));
    instr.SetDecode(is_instr_valid);

    auto pad_cntr = m.state(VTA_LOAD_INP_PAD_CNTR);
    auto y_offset_1 = m.state(VTA_LOAD_Y_OFFSET_1);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto sram = m.state(VTA_INPUT_MEMORY);
    auto sram_next = sram;

    for (auto i = 0; i < VTA_INP_MAT_DATA_NUM; i++) {
      auto addr = sram_addr + i;
      sram_next = Store(sram_next, addr, BvConst(0, VTA_INPUT_MEMORY_DATA_BITWIDTH));
    }

    instr.SetUpdate(sram, sram_next);
    instr.SetUpdate(sram_addr, sram_addr + VTA_INP_MAT_DATA_NUM);

    auto is_pad_done = (pad_cntr >= y_offset_1 - 1);
    auto next_pad_cntr = 
      Ite(is_pad_done, BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH), pad_cntr+1);
    instr.SetUpdate(pad_cntr, next_pad_cntr);
    auto next_state = Ite(is_pad_done,
                          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
                          BvConst(VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_1, VTA_CHILD_INSTR_STATE_BITWIDTH));
    instr.SetUpdate(state, next_state);
    instr.SetUpdate(valid_flag,
                    Ite(is_pad_done,
                        BvConst(VTA_INVALID, VTA_CHILD_VALID_FLAG_BITWIDTH),
                        BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH)));
  }

  // ------------------------------------------------------------------------
  // Load instruction for UOPs
  // ------------------------------------------------------------------------

  { // child instruction ---- load_uop
    auto instr = child.NewInstr("vta_child_load_uop");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_UOP));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = m.state(VTA_LOAD_X_CNTR);
    auto uop_mem = m.state(VTA_UOP_MEMORY);
    auto uop_dram = m.state(VTA_VIRTUAL_UOP_DRAM);

    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);
    auto sram_addr = sram_base_32 + x_cntr_32;
    auto dram_addr = dram_base_32 + x_cntr_32;

    auto uop_mem_next = Store(uop_mem, sram_addr, Load(uop_dram, dram_addr));
    instr.SetUpdate(uop_mem, uop_mem_next);

    auto is_done = (x_cntr >= x_size - 1);
    auto next_state = Ite(is_done, 
                          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
                          BvConst(VTA_CHILD_STATE_LOAD_UOP, VTA_CHILD_INSTR_STATE_BITWIDTH));
    instr.SetUpdate(state, next_state);
    instr.SetUpdate(x_cntr, x_cntr + 1);
  }

  // ------------------------------------------------------------------------
  // Load instruction for bias
  // ------------------------------------------------------------------------

  { // child instruction ---- load_bias_y_size
    auto instr = child.NewInstr("vta_child_load_bias_y_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_BIAS_Y_SIZE));
    instr.SetDecode(is_instr_valid);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    auto y_cntr_32 = Concat(BvConst(0, 32-y_cntr.bit_width()), y_cntr);
    
    auto sram_addr = (sram_base_32 + y_cntr_32 * x_size_32) * VTA_ACCUM_MAT_DATA_NUM;
    auto dram_addr = (dram_base_32 + y_cntr_32 * x_stride_32) * VTA_ACCUM_MAT_DATA_NUM;

    instr.SetUpdate(child.state(VTA_LOAD_X_CNTR), 
                    BvConst(0, VTA_LOAD_X_CNTR_BITWIDTH));
    instr.SetUpdate(child.state(VTA_LOAD_SRAM_ADDR), sram_addr);
    instr.SetUpdate(child.state(VTA_LOAD_DRAM_ADDR), dram_addr);

    instr.SetUpdate(y_cntr, y_cntr + 1);

    auto next_state = BvConst(VTA_CHILD_STATE_LOAD_BIAS_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);

  }

  { // child instruction ---- load_bias_x_size
    auto instr = child.NewInstr("vta_child_load_bias_x_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_BIAS_X_SIZE));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = child.state(VTA_LOAD_X_CNTR);
    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto dram_addr = child.state(VTA_LOAD_DRAM_ADDR);

    sram_addr = sram_addr + x_cntr_32 * VTA_ACCUM_MAT_DATA_NUM;
    dram_addr = dram_addr + x_cntr_32 * VTA_ACCUM_MAT_DATA_NUM;

    // TODO: dram addr don't need to add dram base for current simulation?

    auto sram = m.state(VTA_ACCUM_MEMORY);
    auto sram_next = sram;
    auto dram = m.state(VTA_VIRTUAL_DRAM_BIAS);

    for (auto i = 0; i < VTA_ACCUM_MAT_DATA_NUM; i++) {
      sram_next = Store(sram_next, sram_addr+i, Load(dram, dram_addr+i));
    }

    instr.SetUpdate(sram, sram_next);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    // end condition for x_cntr and y_cntr are different!
    auto is_x_end = (x_cntr >= x_size - 1);
    auto is_y_end = (y_cntr >= y_size);

    auto next_state = 
      Ite(is_x_end & is_y_end,
          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
      Ite(is_x_end, 
          BvConst(VTA_CHILD_STATE_LOAD_BIAS_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
          BvConst(VTA_CHILD_STATE_LOAD_BIAS_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH)));

    instr.SetUpdate(x_cntr, x_cntr + 1);
    instr.SetUpdate(state, next_state);

    instr.SetUpdate(valid_flag,
                    Ite(is_x_end & is_y_end,
                        BvConst(VTA_INVALID, VTA_CHILD_VALID_FLAG_BITWIDTH),
                        BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH)));
  }
  
  // ------------------------------------------------------------------------
  // Store instruction for output
  // ------------------------------------------------------------------------
  // store instruction is using the same child states from load.

  { // child instruction ---- store output into virtual output dram y size
    auto instr = child.NewInstr("vta_child_store_y_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_STORE_Y_SIZE));
    instr.SetDecode(is_instr_valid);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    auto y_cntr_32 = Concat(BvConst(0, 32-y_cntr.bit_width()), y_cntr);

    auto sram_addr = (sram_base_32 + y_cntr_32 * x_size_32) * VTA_OUT_MAT_DATA_NUM;
    auto dram_addr = (dram_base_32 + y_cntr_32 * x_stride_32) * VTA_OUT_MAT_DATA_NUM;

    instr.SetUpdate(child.state(VTA_LOAD_X_CNTR), 
                    BvConst(0, VTA_LOAD_X_CNTR_BITWIDTH));
    instr.SetUpdate(child.state(VTA_LOAD_SRAM_ADDR), sram_addr);
    instr.SetUpdate(child.state(VTA_LOAD_DRAM_ADDR), dram_addr);

    instr.SetUpdate(y_cntr, y_cntr + 1);

    auto next_state = BvConst(VTA_CHILD_STATE_STORE_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- store output into virtual output dram x cntr loop
    auto instr = child.NewInstr("vta_child_store_x_size");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_STORE_X_SIZE));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = child.state(VTA_LOAD_X_CNTR);
    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);

    auto sram_addr = child.state(VTA_LOAD_SRAM_ADDR);
    auto dram_addr = child.state(VTA_LOAD_DRAM_ADDR);

    sram_addr = sram_addr + x_cntr_32 * VTA_OUT_MAT_DATA_NUM;
    dram_addr = dram_addr + x_cntr_32 * VTA_OUT_MAT_DATA_NUM;

    auto sram = m.state(VTA_OUT_MEMORY);
    auto dram = m.state(VTA_VIRTUAL_DRAM_OUT);
    auto dram_next = dram;

    for (auto i = 0; i < VTA_OUT_MAT_DATA_NUM; i++) {
      dram_next = Store(dram_next, dram_addr+i, Load(sram, sram_addr+i));
    }
    instr.SetUpdate(dram, dram_next);

    auto y_cntr = m.state(VTA_LOAD_Y_CNTR);
    // end condition for x_cntr and y_cntr are different!
    auto is_x_end = (x_cntr >= x_size - 1);
    auto is_y_end = (y_cntr >= y_size);

    auto next_state = 
      Ite(is_x_end & is_y_end,
          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
      Ite(is_x_end, 
          BvConst(VTA_CHILD_STATE_STORE_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH),
          BvConst(VTA_CHILD_STATE_STORE_X_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH)));

    instr.SetUpdate(x_cntr, x_cntr + 1);
    instr.SetUpdate(state, next_state);

    instr.SetUpdate(valid_flag,
                    Ite(is_x_end & is_y_end,
                        BvConst(VTA_INVALID, VTA_CHILD_VALID_FLAG_BITWIDTH),
                        BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH)));
  }
}

} // namespace vta
} // namespace ilang