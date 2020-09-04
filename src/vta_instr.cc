// File: vta_instr.cc
#include <ilang/ilang++.h>
#include <vta/vta_top.h>

namespace ilang {
namespace vta {

void DefineInstr(Ila& m) {
  
  auto vta_ins_in = m.input(VTA_TOP_INSTR_IN);
  auto opcode = Extract(vta_ins_in, VTA_OPCODE_BITWIDTH-1, 0);
  
  { // instruction load
    // load weights
    auto instr = m.NewInstr("vta_load_wgt");
    // use ins_temp for slicing instructions parameters
    auto ins_temp = (vta_ins_in >> VTA_OPCODE_BITWIDTH);
    
    auto memory_type = Extract(ins_temp, VTA_MEMOP_ID_BITWIDTH-1, 0);
    ins_temp = (ins_temp >> VTA_MEMOP_ID_BITWIDTH);

    auto is_opcode_load = (opcode == VTA_OPCODE_LOAD);
    auto is_wgt = (memory_type == VTA_MEM_ID_WGT);

    instr.SetDecode(is_opcode_load & is_wgt);

    

  }
}

}
}