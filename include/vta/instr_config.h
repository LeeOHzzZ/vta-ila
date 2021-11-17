// File: instr_config.h
// this file contains configurations related to vta instructions

#ifndef INSTR_CONFIG_H__
#define INSTR_CONFIG_H__

#include <vta/top_config.h>

namespace ilang {
namespace vta {

// instruction opcode field bitwidth
#define VTA_OPCODE_BITWIDTH 3
// ALU opcode bitwidth
#define VTA_ALU_OPCODE_BITWIDTH 3

// Opcode 
#define VTA_OPCODE_LOAD 0
#define VTA_OPCODE_STORE 1
#define VTA_OPCODE_GEMM 2
#define VTA_OPCODE_ALU 4

// ALU opcode
#define VTA_ALU_OPCODE_MIN 0
#define VTA_ALU_OPCODE_MAX 1
#define VTA_ALU_OPCODE_ADD 2
#define VTA_ALU_OPCODE_SHR 3
#define VTA_ALU_OPCODE_MUL 4

// ------------------------------
// memory type field bitwidth
// ------------------------------
/*! Memory type field bitwidth */
#define VTA_MEMOP_ID_BITWIDTH 2
/*! Load/Store Instruction: DRAM address width*/
#define VTA_MEMOP_SRAM_ADDR_BITWIDTH 16
/*! Load/Store Instruction: DRAM address width*/
#define VTA_MEMOP_DRAM_ADDR_BITWIDTH 32
/*! Load/Store Instruction: transfer size width*/
#define VTA_MEMOP_SIZE_BITWIDTH 16
/*! Load/Store Instruction: stride size width*/
#define VTA_MEMOP_STRIDE_BITWIDTH 16
/*! Load/Store Instruction: padding width*/
#define VTA_MEMOP_PAD_BITWIDTH 4
/*! Load/Store Instruction: padding value encoding width*/
#define VTA_MEMOP_PAD_VAL_BITWIDTH 2

/*! Mem ID constant: uop memory */
#define VTA_MEM_ID_UOP 0
/*! Mem ID constant: weight memory */
#define VTA_MEM_ID_WGT 1
/*! Mem ID constant: input memory */
#define VTA_MEM_ID_INP 2
/*! Mem ID constant: accumulator/bias memory */
#define VTA_MEM_ID_ACC 3
/*! Mem ID constant: output store buffer */
#define VTA_MEM_ID_OUT 4


// ----------------------------
// ALU type 
// ----------------------------

/*! GEMM/ALU Instruction: loop max iter bits */
#define VTA_LOOP_ITER_WIDTH 14
/*! ALU Instruction: immediate bitwidth*/
#define VTA_ALUOP_IMM_BIT_WIDTH 16
/*! ALU Instruction: shift arg bitwidth*/
#define VTA_SHR_ARG_BIT_WIDTH (VTA_LOG_ACCUM_BITWIDTH)
/*! ALU Instruction: multiply arg bitwidth*/
#define VTA_MUL_ARG_BIT_WIDTH 8




} // namespace vta
} // namespace ilang

#endif