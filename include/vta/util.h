// file: util.h
#ifndef VTA_UTIL_H__
#define VTA_UTIL_H__

#include <ilang/ilang++.h>
#include <ilang/util/log.h>

namespace ilang {
namespace vta {

inline ExprRef read_tensor(const ExprRef& src_mem, const ExprRef& tensor, const ExprRef& start_addr,
                            const int& size) 
{
  auto tensor_next = tensor;
  ILA_ASSERT(start_addr.bit_width() == 32);

  for (auto i = 0; i < size; i++) {
    auto src_addr = start_addr + i;
    auto dst_addr = BvConst(i, 32);
    auto data = Load(src_mem, src_addr);
    tensor_next = Store(tensor_next, dst_addr, data);
  }

  return tensor_next;
}

inline ExprRef write_tensor(const ExprRef& dst_mem, const ExprRef& tensor, const ExprRef& start_addr,
                            const int& size)
{
  auto mem_next = dst_mem;
  ILA_ASSERT(start_addr.bit_width() == 32);
  
  for (auto i = 0; i < size; i++) {
    auto src_addr = BvConst(i, 32);
    auto dst_addr = start_addr + i;
    auto data = Load(tensor, src_addr);
    mem_next = Store(mem_next, dst_addr, data);
  }

  return mem_next;
}

}
}

#endif