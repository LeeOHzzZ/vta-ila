// File: main.cc

#include <vta/vta_top.h>

#include <fstream>
#include <ilang/util/log.h>

using namespace ilang;
using namespace vta;

int main() {
  // get the ILA model
  auto vta = GetVtaIla("vta");

  // example - export ILA to Verilog
  ILA_INFO << "Export " << vta << " to vta.v";
  std::ofstream fw("vta.v");
  vta.ExportToVerilog(fw);
  fw.close();

  return 0;
}
