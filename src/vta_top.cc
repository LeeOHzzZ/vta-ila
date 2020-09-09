// File: vta_top.cc

#include <vta/vta_top.h>

#include <ilang/util/log.h>

namespace ilang {
namespace vta {

Ila GetVtaIla(const std::string& model_name) {
  auto m = Ila(model_name);

  DefineTopIO(m);
  DefineArchState(m);
  DefineInternalState(m);
  DefineInstr(m);

  return m;
}

} // namespace vta
}; // namespace ilang
