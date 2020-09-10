// File: internal_state.cc

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineInternalState(Ila& m) {
  // internal states for child instructions
  m.NewBvState(VTA_CHILD_INSTR_STATE, VTA_CHILD_INSTR_STATE_BITWIDTH);
  m.NewBvState(VTA_CHILD_VALID_FLAG, VTA_CHILD_VALID_FLAG_BITWIDTH);

  // internal states for load/store instruction
  m.NewBvState(VTA_SRAM_ID, VTA_SRAM_ID_BITWIDTH);
  m.NewBvState(VTA_DRAM_ID, VTA_DRAM_ID_BITWIDTH);
  m.NewBvState(VTA_Y_SIZE, VTA_Y_SIZE_BITWIDTH);
  m.NewBvState(VTA_X_SIZE, VTA_X_SIZE_BITWIDTH);
  m.NewBvState(VTA_X_STRIDE, VTA_X_STRIDE_BITWIDTH);
  m.NewBvState(VTA_Y_PAD_0, VTA_Y_PAD_0_BITWIDTH);
  m.NewBvState(VTA_Y_PAD_1, VTA_Y_PAD_1_BITWIDTH);
  m.NewBvState(VTA_X_PAD_0, VTA_X_PAD_0_BITWIDTH);
  m.NewBvState(VTA_X_PAD_1, VTA_X_PAD_1_BITWIDTH);

  m.NewBvState(VTA_LOAD_Y_CNTR, VTA_LOAD_Y_CNTR_BITWIDTH);
  m.NewBvState(VTA_LOAD_Y_OFFSET_0, VTA_LOAD_Y_OFFSET_0_BITWIDTH);
  m.NewBvState(VTA_LOAD_Y_OFFSET_1, VTA_LOAD_Y_OFFSET_1_BITWIDTH);

  m.NewBvState(VTA_LOAD_INP_PAD_CNTR, VTA_LOAD_INP_PAD_CNTR_BITWIDTH);
}

} // namespace vta
} // namespace ilang