// File: vta_top.cc

#include <vta/vta_top.h>

#include <ilang/util/log.h>

namespace ilang {
namespace vta {

Ila GetVtaIla(const std::string& model_name) {
  auto m = Ila(model_name);
  
  SetUnsignedComparison(true);

  DefineTopIO(m);
  DefineArchState(m);
  DefineInternalState(m);

  DefineInstr(m);
  DefineChild(m);
  DefineChildInstrLoadStore(m);
  DefineChildGEMM(m);
  DefineChildALU(m);

  DefineVirMemInstr(m);

  m.SetValid(((m.input(VTA_TOP_INSTR_IN) & BvConst(0, VTA_INSTR_BITWIDTH)) > 0) |
              (m.input(VTA_VIR_MEM_MODE_IN) > 0));

  return m;
}

void DefineChild(Ila& m) {
  auto child = m.NewChild("vta_child");
  auto state = m.state(VTA_CHILD_INSTR_STATE);
  auto valid_flag = m.state(VTA_CHILD_VALID_FLAG);

  auto is_child_valid = ((valid_flag == VTA_VALID) & (state != VTA_CHILD_STATE_IDLE));
  child.SetValid(is_child_valid);
}

} // namespace vta
}; // namespace ilang
