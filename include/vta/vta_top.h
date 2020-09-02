// File: vta_top.h

#ifndef VTA_TOP_H__
#define VTA_TOP_H__

#include <ilang/ilang++.h>
#include <string>

namespace ilang {
namespace vta{

Ila GetVtaIla(const std::string& model_name = "vta");

} // namespace vta
}; // namespace ilang

#endif // VTA_TOP_H__
