// file: arch_state.cc
#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineArchState(Ila& m) {
  // input memory
  auto input_mem = 
    m.NewMemState(VTA_INPUT_MEMORY, VTA_MEMORY_ADDR_BITWIDTH, VTA_INPUT_MEMORY_DATA_BITWIDTH);
  input_mem.SetEntryNum(VTA_INPUT_MEMORY_ENTRY_NUM);

  // weight memory
  auto weight_memory = 
    m.NewMemState(VTA_WEIGHT_MEMORY, VTA_MEMORY_ADDR_BITWIDTH, VTA_WEIGHT_MEMORY_DATA_BITWIDTH);
  weight_memory.SetEntryNum(VTA_WEIGHT_MEMORY_ENTRY_NUM);

  auto out_memory = 
    m.NewMemState(VTA_OUT_MEMORY, VTA_MEMORY_ADDR_BITWIDTH, VTA_OUT_MEMORY_DATA_BITWIDTH);
  out_memory.SetEntryNum(VTA_OUT_MEMORY_ENTRY_NUM);

  // accummulation memory
  auto accum_memory = 
    m.NewMemState(VTA_ACCUM_MEMORY, VTA_MEMORY_ADDR_BITWIDTH, VTA_ACCUM_MEMORY_DATA_BITWIDTH);
  accum_memory.SetEntryNum(VTA_ACCUM_MEMORY_ENTRY_NUM);

  // uop memory
  auto uop_memory = 
    m.NewMemState(VTA_UOP_MEMORY, VTA_MEMORY_ADDR_BITWIDTH, VTA_UOP_MEMORY_DATA_BITWIDTH);
  uop_memory.SetEntryNum(VTA_UOP_MEMORY_ENTRY_NUM);


  // virtual dram
  auto vir_dram = 
    m.NewMemState(VTA_VIRTUAL_DRAM, VTA_MEMORY_ADDR_BITWIDTH, VTA_VIRTUAL_DRAM_DATA_BITWIDTH);
  vir_dram.SetEntryNum(VTA_VIRTUAL_DRAM_ENTRY_NUM);

  auto vir_uop_dram = 
    m.NewMemState(VTA_VIRTUAL_UOP_DRAM, VTA_MEMORY_ADDR_BITWIDTH, VTA_VIRTUAL_UOP_DRAM_DATA_BITWIDTH);
  vir_uop_dram.SetEntryNum(VTA_VIRTUAL_UOP_DRAM_ENTRY_NUM);

}

}
}