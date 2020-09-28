// file: vir_mem_store.cc
// this file contains instructions for virtual memory store

#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineVirMemInstr(Ila& m) {
  auto mode = m.input(VTA_VIR_MEM_MODE_IN);
  
  { // instruction for writing inp data
    auto instr = m.NewInstr("vir_wr_inp");
    instr.SetDecode(mode == VTA_VIR_MEM_MODE_WR_INP);

    auto inp_mem = m.state(VTA_VIRTUAL_DRAM_INPUT);
    auto addr = m.input(VTA_VIR_MEM_ADDR_IN);
    auto data = m.input(VTA_VIR_MEM_INP_DATA_IN);

    instr.SetUpdate(inp_mem, Store(inp_mem, addr, data));
  }

  { // instruction for writing weight data
    auto instr = m.NewInstr("vir_wr_wgt");
    instr.SetDecode(mode == VTA_VIR_MEM_MODE_WR_BIAS);

    auto wgt_mem = m.state(VTA_VIRTUAL_DRAM_WEIGHT);
    auto addr = m.input(VTA_VIR_MEM_ADDR_IN);
    auto data = m.input(VTA_VIR_MEM_WGT_DATA_IN);

    instr.SetUpdate(wgt_mem, Store(wgt_mem, addr, data));
  }

  { // instruction for writing bias data
    auto instr = m.NewInstr("vir_wr_bias");
    instr.SetDecode(mode == VTA_VIR_MEM_MODE_WR_BIAS);

    auto bias_mem = m.state(VTA_VIRTUAL_DRAM_BIAS);
    auto addr = m.input(VTA_VIR_MEM_ADDR_IN);
    auto data = m.input(VTA_VIR_MEM_BIAS_DATA_IN);
    instr.SetUpdate(bias_mem, Store(bias_mem, addr, data));
  }

  { // instruction for writing uop 
    auto instr = m.NewInstr("vir_wr_uop");
    instr.SetDecode(mode == VTA_VIR_MEM_MODE_WR_UOP);

    auto uop_mem = m.state(VTA_VIRTUAL_UOP_DRAM);
    auto addr = m.input(VTA_VIR_MEM_ADDR_IN);
    auto data = m.input(VTA_VIR_MEM_UOP_DATA_IN);
    instr.SetUpdate(uop_mem, Store(uop_mem, addr, data));
  }
}

}
}