// File: main.cc

#include <vta/vta_top.h>

#include <fstream>
#include <ilang/util/log.h>

using namespace ilang;
using namespace vta;

int main() {
  SetToStdErr(1);
  // get the ILA model
  auto vta = GetVtaIla("vta");

  ILA_INFO << "#child: " << vta.child_num();
  ILA_INFO << "#instr: " << vta.instr_num();
  ILA_INFO << "#input: " << vta.input_num();
  ILA_INFO << "#state: " << vta.state_num();
  for (auto i = 0; i < vta.instr_num(); i++) {
    ILA_INFO << "instr." << i << " " << vta.instr(i);
  }
  for (auto i = 0; i < vta.child_num(); i++) {
    ILA_INFO << "child #" << i << '\t' << vta.child(i);
    ILA_INFO << "child #instr: " << vta.child(i).instr_num();
    ILA_INFO << "child #state: " << vta.child(i).state_num();
    for (auto j = 0; j < vta.child(i).instr_num(); j++) {
      ILA_INFO << "child instr." << j << " " << vta.child(i).instr(j);
    }
  }

  ILA_INFO << "BITWIDTH INFO:";
  ILA_INFO << "OPCODE: " << VTA_OPCODE_BITWIDTH;
  ILA_INFO << "ALU_OPCODE: " << VTA_ALU_OPCODE_BITWIDTH;
  ILA_INFO << "IMM: " << VTA_ALU_IMM_BITWIDTH;
  ILA_INFO << "MEM_TYPE: " << VTA_MEMOP_ID_BITWIDTH;
  ILA_INFO << "SRAM_ADDR: " << VTA_MEMOP_SRAM_ADDR_BITWIDTH;
  ILA_INFO << "DRAM_ADDR: " << VTA_MEMOP_DRAM_ADDR_BITWIDTH;
  ILA_INFO << "SIZE: " << VTA_MEMOP_SIZE_BITWIDTH;
  ILA_INFO << "STRIDE: " << VTA_MEMOP_STRIDE_BITWIDTH;
  ILA_INFO << "PAD: " << VTA_MEMOP_PAD_BITWIDTH;
  ILA_INFO << "UOP_BEGIN: " << VTA_ALU_UOP_BEGIN_BITWIDTH;
  ILA_INFO << "UOP_END: " << VTA_ALU_UOP_END_BITWIDTH;
  ILA_INFO << "ITER: " << VTA_LOOP_ITER_WIDTH;
  ILA_INFO << "DST_FACTOR: " << VTA_ALU_DST_FACTOR_IN_BITWIDTH;
  ILA_INFO << "SRC_FACTOR: " << VTA_ALU_SRC_FACTOR_IN_BITWIDTH;
  ILA_INFO << "WGT_FACTOR: " << VTA_GEMM_WGT_FACTOR_IN_BITWIDTH;

  ILA_INFO << "DRAM_BIAS_ENTRY_NUM: " << VTA_VIRTUAL_DRAM_BIAS_ENTRY_NUM;

  ExportSysCSim(vta, "./sim_model", false);

  return 0;
}
