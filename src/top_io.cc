// file: top_io.cc

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineTopIO(Ila& m) {
  m.NewBvInput(VTA_TOP_INSTR_IN, VTA_TOP_INSTR_IN_BITWIDTH);
}

} // namespace vta
} // namespace ilang