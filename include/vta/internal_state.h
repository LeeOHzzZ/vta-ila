// file: internal_state.h

#ifndef VTA_INTERNAL_STATE_H__
#define VTA_INTERNAL_STATE_H__

#include <vta/memory_map.h>
#include <vta/instr_config.h>

namespace ilang {
namespace vta {

// use a FSM to regulate all the child instructions
#define VTA_CHILD_INSTR_STATE "vta_child_instr_state"
#define VTA_CHILD_INSTR_STATE_BITWIDTH 4

#define VTA_CHILD_VALID_FLAG "vta_child_instr_valid_flag"
#define VTA_CHILD_VALID_FLAG_BITWIDTH 1

#define VTA_VALID 1
#define VTA_INVALID 0

#define VTA_CHILD_STATE_IDLE 0
#define VTA_CHILD_STATE_LOAD_WGT_Y_SIZE 1
#define VTA_CHILD_STATE_LOAD_WGT_X_SIZE 2
#define VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_0 3
#define VTA_CHILD_STATE_LOAD_INP_Y_SIZE 4
#define VTA_CHILD_STATE_LOAD_INP_X_PAD_0 5
#define VTA_CHILD_STATE_LOAD_INP_X_SIZE 6
#define VTA_CHILD_STATE_LOAD_INP_X_PAD_1 7
#define VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_1 8

// internal states for LOAD/STORE instruction
#define VTA_SRAM_ID "vta_sram_id"
#define VTA_SRAM_ID_BITWIDTH VTA_MEMOP_SRAM_ADDR_BITWIDTH

#define VTA_DRAM_ID "vta_dram_id"
#define VTA_DRAM_ID_BITWIDTH VTA_MEMOP_DRAM_ADDR_BITWIDTH

#define VTA_Y_SIZE "vta_y_size"
#define VTA_Y_SIZE_BITWIDTH VTA_MEMOP_SIZE_BITWIDTH

#define VTA_X_SIZE "vta_x_size"
#define VTA_X_SIZE_BITWIDTH VTA_MEMOP_SIZE_BITWIDTH

#define VTA_X_STRIDE "vta_x_stride"
#define VTA_X_STRIDE_BITWIDTH VTA_MEMOP_STRIDE_BITWIDTH

#define VTA_Y_PAD_0 "vta_y_pad_0"
#define VTA_Y_PAD_0_BITWIDTH VTA_MEMOP_PAD_BITWIDTH
#define VTA_Y_PAD_1 "vta_y_pad_1"
#define VTA_Y_PAD_1_BITWIDTH VTA_MEMOP_PAD_BITWIDTH
#define VTA_X_PAD_0 "vta_x_pad_0"
#define VTA_X_PAD_0_BITWIDTH VTA_MEMOP_PAD_BITWIDTH
#define VTA_X_PAD_1 "vta_x_pad_1"
#define VTA_X_PAD_1_BITWIDTH VTA_MEMOP_PAD_BITWIDTH

// states for load
#define VTA_LOAD_Y_CNTR "vta_load_y_cntr"
#define VTA_LOAD_Y_CNTR_BITWIDTH (VTA_MEMOP_SIZE_BITWIDTH)

#define VTA_LOAD_X_CNTR "vta_load_x_cntr"
#define VTA_LOAD_X_CNTR_BITWIDTH VTA_MEMOP_SIZE_BITWIDTH

#define VTA_LOAD_SRAM_ADDR "vta_load_sram_addr"
#define VTA_LOAD_SRAM_ADDR_BITWIDTH VTA_MEMORY_ADDR_BITWIDTH

#define VTA_LOAD_DRAM_ADDR "vta_load_dram_addr"
#define VTA_LOAD_DRAM_ADDR_BITWIDTH VTA_MEMORY_ADDR_BITWIDTH

#define VTA_LOAD_INP_PAD_CNTR "vta_load_inp_pad_cntr"
#define VTA_LOAD_INP_PAD_CNTR_BITWIDTH VTA_MEMOP_SRAM_ADDR_BITWIDTH

#define VTA_LOAD_Y_OFFSET_0 "vta_load_y_offset_0"
#define VTA_LOAD_Y_OFFSET_0_BITWIDTH VTA_MEMOP_SRAM_ADDR_BITWIDTH

#define VTA_LOAD_Y_OFFSET_1 "vta_load_y_offset_1"
#define VTA_LOAD_Y_OFFSET_1_BITWIDTH VTA_MEMOP_SRAM_ADDR_BITWIDTH


}
}

#endif