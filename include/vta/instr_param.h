// file: instr_param.h
// this file includes vta instruction parameters

#ifndef VTA_INSTR_PARAM_H__
#define VTA_INSTR_PARAM_H__

#include <vta/memory_map.h>
#include <vta/instr_config.h>

namespace ilang {
namespace vta {

// LOAD/STORE instructions
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

// GEMM instuction
#define VTA_GEMM_RESET_FLAG "vta_gemm_reset_flag"
#define VTA_GEMM_RESET_FLAG_BITWIDTH 1

#define VTA_GEMM_UOP_BEGIN "vta_gemm_uop_begin"
#define VTA_GEMM_UOP_BEGIN_BITWIDTH VTA_LOG_UOP_BUFF_DEPTH

#define VTA_GEMM_UOP_END "vta_gemm_uop_end"
#define VTA_GEMM_UOP_END_BITWIDTH (VTA_LOG_UOP_BUFF_DEPTH + 1)

#define VTA_GEMM_ITER_OUT "vta_gemm_iter_out"
#define VTA_GEMM_ITER_OUT_BITWIDTH VTA_LOOP_ITER_WIDTH

#define VTA_GEMM_ITER_IN "vta_gemm_iter_in"
#define VTA_GEMM_ITER_IN_BITWIDTH VTA_LOOP_ITER_WIDTH

#define VTA_GEMM_DST_FACTOR_OUT "vta_gemm_dst_factor_out"
#define VTA_GEMM_DST_FACTOR_OUT_BITWIDTH VTA_LOG_ACC_BUFF_DEPTH

#define VTA_GEMM_DST_FACTOR_IN "vta_gemm_dst_factor_in"
#define VTA_GEMM_DST_FACTOR_IN_BITWIDTH VTA_LOG_ACC_BUFF_DEPTH

#define VTA_GEMM_SRC_FACTOR_OUT "vta_gemm_src_factor_out"
#define VTA_GEMM_SRC_FACTOR_OUT_BITWIDTH VTA_LOG_INP_BUFF_DEPTH

#define VTA_GEMM_SRC_FACTOR_IN "vta_gemm_src_factor_in"
#define VTA_GEMM_SRC_FACTOR_IN_BITWIDTH VTA_LOG_INP_BUFF_DEPTH

#define VTA_GEMM_WGT_FACTOR_OUT "vta_gemm_factor_out"
#define VTA_GEMM_WGT_FACTOR_OUT_BITWIDTH VTA_LOG_WGT_BUFF_DEPTH

#define VTA_GEMM_WGT_FACTOR_IN "vta_gemm_factor_in"
#define VTA_GEMM_WGT_FACTOR_IN_BITWIDTH VTA_LOG_WGT_BUFF_DEPTH

// ALU instruction
#define VTA_ALU_RESET_FLAG "vta_alu_reset_flag"
#define VTA_ALU_RESET_FLAG_BITWIDTH 1

#define VTA_ALU_UOP_BEGIN "vta_alu_uop_begin"
#define VTA_ALU_UOP_BEGIN_BITWIDTH VTA_LOG_UOP_BUFF_DEPTH



/*! GEMM Micro-op start position of the acc_idx field */
#define VTA_UOP_GEM_0_0 0
/*! GEMM Micro-op end position of the acc_idx field */
#define VTA_UOP_GEM_0_1 (VTA_UOP_GEM_0_0 + VTA_LOG_ACC_BUFF_DEPTH - 1)
/*! GEMM Micro-op start position of the inp_idx field */
#define VTA_UOP_GEM_1_0 (VTA_UOP_GEM_0_1 + 1)
/*! GEMM Micro-op end position of the inp_idx field */
#define VTA_UOP_GEM_1_1 (VTA_UOP_GEM_1_0 + VTA_LOG_INP_BUFF_DEPTH - 1)
/*! GEMM Micro-op start position of the wgt_idx field */
#define VTA_UOP_GEM_2_0 (VTA_UOP_GEM_1_1 + 1)
/*! GEMM Micro-op end position of the wgt_idx field */
#define VTA_UOP_GEM_2_1 (VTA_UOP_GEM_2_0 + VTA_LOG_WGT_BUFF_DEPTH - 1)

/*! GEMM Micro-op start position of the acc_idx field */
#define VTA_UOP_ALU_0_0 0
/*! GEMM Micro-op end position of the acc_idx field */
#define VTA_UOP_ALU_0_1 (VTA_UOP_ALU_0_0 + VTA_LOG_ACC_BUFF_DEPTH - 1)
/*! GEMM Micro-op start position of the inp_idx field */
#define VTA_UOP_ALU_1_0 (VTA_UOP_ALU_0_1 + 1)
/*! GEMM Micro-op end position of the inp_idx field */
#define VTA_UOP_ALU_1_1 (VTA_UOP_ALU_1_0 + VTA_LOG_INP_BUFF_DEPTH - 1)
}
}

#endif