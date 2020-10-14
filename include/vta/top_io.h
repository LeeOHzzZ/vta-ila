// File: top_io.h

#ifndef VTA_TOP_IO_H__
#define VTA_TOP_IO_H__

#include <vta/top_config.h>
#include <vta/memory_map.h>

namespace ilang {
namespace vta {

#define VTA_TOP_INSTR_IN "vta_top_instr_in"
#define VTA_TOP_INSTR_IN_BITWIDTH VTA_INSTR_BITWIDTH

// IO for virtual memory
#define VTA_VIR_MEM_MODE_IN "vta_vir_mem_mode_in"
#define VTA_VIR_MEM_MODE_IN_BITWIDTH 3

#define VTA_VIR_MEM_MODE_WR_INP 1
#define VTA_VIR_MEM_MODE_WR_WGT 2
#define VTA_VIR_MEM_MODE_WR_BIAS 3
#define VTA_VIR_MEM_MODE_WR_UOP 4

#define VTA_VIR_MEM_ADDR_IN "vta_vir_mem_addr_in"
#define VTA_VIR_MEM_ADDR_IN_BITWIDTH VTA_MEMORY_ADDR_BITWIDTH

#define VTA_VIR_MEM_INP_DATA_IN "vta_vir_mem_inp_data_in"
#define VTA_VIR_MEM_INP_DATA_IN_BITWIDTH VTA_INPUT_DATA_BITWIDTH

#define VTA_VIR_MEM_WGT_DATA_IN "vta_vir_mem_wgt_data_in"
#define VTA_VIR_MEM_WGT_DATA_IN_BITWIDTH VTA_WEIGHT_BITWIDTH

#define VTA_VIR_MEM_BIAS_DATA_IN "vta_vir_mem_bias_data_in"
#define VTA_VIR_MEM_BIAS_DATA_IN_BITWIDTH VTA_ACCUM_BITWIDTH

#define VTA_VIR_MEM_UOP_DATA_IN "vta_vir_mem_uop_data_in"
#define VTA_VIR_MEM_UOP_DATA_IN_BITWIDTH VTA_UOP_BITWIDTH

} // namespace vta
} // namespace ilang

#endif