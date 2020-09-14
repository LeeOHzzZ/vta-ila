// file: child_instr_gemm.cc
#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineChildGEMM(Ila& m) {
  auto child = m.child("vta_child");

  // -------------- child states --------------//
  auto it_out = child.NewBvState(VTA_GEMM_OUT_LOOP_CNTR, VTA_GEMM_OUT_LOOP_CNTR_BITWIDTH);
  auto it_in = child.NewBvState(VTA_GEMM_IN_LOOP_CNTR, VTA_GEMM_IN_LOOP_CNTR_BITWIDTH);
  auto upc = child.NewBvState(VTA_GEMM_UOP_CNTR, VTA_GEMM_UOP_CNTR_BITWIDTH);

  // -------------- parent states -------------//
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto child_valid_flag = m.state(VTA_CHILD_VALID_FLAG);

  { // child instruction ---- set up gemm child initial states
    auto instr = child.NewInstr("vta_child_gemm_init");
    auto is_instr_valid = ((child_valid_flag == VTA_VALID) &
                            (state == VTA_CHILD_STATE_GEMM_START));
    instr.SetDecode(is_instr_valid);

    

  }

  
}

}
}