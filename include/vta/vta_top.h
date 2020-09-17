// File: vta_top.h

#ifndef VTA_TOP_H__
#define VTA_TOP_H__

#include <ilang/ilang++.h>
#include <string>

#include <vta/top_config.h>
#include <vta/top_io.h>
#include <vta/instr_config.h>
#include <vta/memory_map.h>
#include <vta/instr_param.h>
#include <vta/internal_state.h>
#include <vta/util.h>


namespace ilang {
namespace vta{

Ila GetVtaIla(const std::string& model_name = "vta");

// define top level IO ports
void DefineTopIO(Ila& m);
// define architectural states
void DefineArchState(Ila& m);
// define internal states
void DefineInternalState(Ila& m);

// define instructions
void DefineInstr(Ila& m);
// define child
void DefineChild(Ila& m);
// define child instructions for load instruction
void DefineChildInstrLoad(Ila& m);
// define child instruction for GEMM
void DefineChildGEMM(Ila& m);
// define child instruction for alu
void DefineChildALU(Ila& m);

} // namespace vta
}; // namespace ilang

#endif // VTA_TOP_H__
