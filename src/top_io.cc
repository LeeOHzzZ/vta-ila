// file: top_io.cc

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineTopIO(Ila& m) {
  m.NewBvInput(VTA_TOP_INSTR_IN, VTA_TOP_INSTR_IN_BITWIDTH);
  // IO for virtual memory
  m.NewBvInput(VTA_VIR_MEM_MODE_IN, VTA_VIR_MEM_MODE_IN_BITWIDTH);
  m.NewBvInput(VTA_VIR_MEM_ADDR_IN, VTA_VIR_MEM_ADDR_IN_BITWIDTH);
  m.NewBvInput(VTA_VIR_MEM_INP_DATA_IN, VTA_VIR_MEM_INP_DATA_IN_BITWIDTH);
  m.NewBvInput(VTA_VIR_MEM_WGT_DATA_IN, VTA_VIR_MEM_WGT_DATA_IN_BITWIDTH);
  m.NewBvInput(VTA_VIR_MEM_BIAS_DATA_IN, VTA_VIR_MEM_BIAS_DATA_IN_BITWIDTH);
  m.NewBvInput(VTA_VIR_MEM_UOP_DATA_IN, VTA_VIR_MEM_UOP_DATA_IN_BITWIDTH);
}

} // namespace vta
} // namespace ilang