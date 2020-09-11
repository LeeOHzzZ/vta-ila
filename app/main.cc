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

  ExportSysCSim(vta, "./sim_model", false);

  return 0;
}
