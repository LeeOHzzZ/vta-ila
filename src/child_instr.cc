// file: child_instr.cc
// this file defines vta child instructions

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineChildInstr(Ila& m) {
  auto child = m.NewChild("vta_child");
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto valid_flag = m.state(VTA_CHILD_VALID_FLAG);

  auto is_child_valid = ((valid_flag == VTA_VALID) & (state != VTA_CHILD_STATE_IDLE));
  child.SetValid(is_child_valid);


  // ----------- child state ------------- //
  child.NewBvState(VTA_LOAD_WGT_SRAM_ADDR, VTA_LOAD_WGT_SRAM_ADDR_BITWIDTH);
  child.NewBvState(VTA_LOAD_WGT_DRAM_ADDR, VTA_LOAD_WGT_DRAM_ADDR_BITWIDTH);
  child.NewBvState(VTA_LOAD_WGT_X_CNTR, VTA_LOAD_WGT_X_CNTR_BITWIDTH);

  { // child instruction ---- LOAD WGT 0
    auto instr = child.NewInstr("vta_child_load_wgt_0");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_WGT_0));
    instr.SetDecode(is_instr_valid);

    auto y_cntr = m.state(VTA_LOAD_WGT_Y_CNTR);
    auto sram_base = m.state(VTA_SRAM_ID);
    auto dram_base = m.state(VTA_DRAM_ID);
    auto y_size = m.state(VTA_Y_SIZE);
    auto x_size = m.state(VTA_X_SIZE);
    auto x_stride = m.state(VTA_X_STRIDE);

    auto sram_base_32 = Concat(BvConst(0, 32-sram_base.bit_width()), sram_base);
    auto dram_base_32 = Concat(BvConst(0, 32-dram_base.bit_width()), dram_base);
    auto y_cntr_32 = Concat(BvConst(0, 32-y_cntr.bit_width()), y_cntr);
    auto x_size_32 = Concat(BvConst(0, 32-x_size.bit_width()), x_size);
    auto x_stride_32 = Concat(BvConst(0, 32-x_stride.bit_width()), x_stride);
    
    auto sram_addr = (sram_base_32 + y_cntr_32 * x_size_32) * VTA_WGT_MAT_DATA_NUM;
    auto dram_addr = (dram_base_32 + y_cntr_32 * x_stride_32) * VTA_WGT_MAT_DATA_NUM;

    instr.SetUpdate(child.state(VTA_LOAD_WGT_X_CNTR), 
                    BvConst(0, VTA_LOAD_WGT_X_CNTR_BITWIDTH));
    instr.SetUpdate(child.state(VTA_LOAD_WGT_SRAM_ADDR), sram_addr);
    instr.SetUpdate(child.state(VTA_LOAD_WGT_DRAM_ADDR), dram_addr);

    instr.SetUpdate(y_cntr, y_cntr + 1);

    auto next_state = BvConst(VTA_CHILD_STATE_LOAD_WGT_1, VTA_CHILD_INSTR_STATE_BITWIDTH);
    instr.SetUpdate(state, next_state);
  }

  { // child instruction ---- LOAD WGT 1
    auto instr = child.NewInstr("vta_child_load_wgt_1");
    auto is_instr_valid = ((valid_flag == VTA_VALID) & (state == VTA_CHILD_STATE_LOAD_WGT_1));
    instr.SetDecode(is_instr_valid);

    auto x_cntr = child.state(VTA_LOAD_WGT_X_CNTR);
    auto x_cntr_32 = Concat(BvConst(0, 32-x_cntr.bit_width()), x_cntr);

    auto sram_addr = child.state(VTA_LOAD_WGT_SRAM_ADDR);
    auto dram_addr = child.state(VTA_LOAD_WGT_DRAM_ADDR);

    sram_addr = sram_addr + x_cntr_32 * VTA_WGT_MAT_DATA_NUM;
    dram_addr = dram_addr + x_cntr_32 * VTA_WGT_MAT_DATA_NUM;

    auto sram = child.state(VTA_WEIGHT_MEMORY);
    auto sram_next = sram;
    auto dram = child.state(VTA_VIRTUAL_DRAM);
    auto dram_next = dram;

    for (auto i = 0; i < VTA_WGT_MAT_DATA_NUM; i++) {
      sram_addr = sram_addr + i;
      dram_addr = dram_addr + i;
      sram_next = Store(sram_next, sram_addr, Load(dram, dram_addr));
    }

    instr.SetUpdate(sram, sram_next);

    auto y_size = m.state(VTA_Y_SIZE);
    auto x_size = m.state(VTA_X_SIZE);
    auto y_cntr = m.state(VTA_LOAD_WGT_Y_CNTR);
    // end condition for x_cntr and y_cntr are different!
    auto is_x_end = (x_cntr >= x_size - 1);
    auto is_y_end = (y_cntr >= y_size);

    auto next_state = Ite(is_x_end & is_y_end,
                          BvConst(VTA_CHILD_STATE_IDLE, VTA_CHILD_INSTR_STATE_BITWIDTH),
                      Ite(is_x_end, 
                          BvConst(VTA_CHILD_STATE_LOAD_WGT_0, VTA_CHILD_INSTR_STATE_BITWIDTH),
                          BvConst(VTA_CHILD_STATE_LOAD_WGT_1, VTA_CHILD_INSTR_STATE_BITWIDTH)));
    
    instr.SetUpdate(x_cntr, x_cntr + 1);
    instr.SetUpdate(state, next_state);

  }
  
}

} // namespace vta
} // namespace ilang