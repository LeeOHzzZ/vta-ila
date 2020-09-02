#ifndef VTA_TOP_CONFIG_H__
#define VTA_TOP_CONFIG_H__

#include <cmath>

namespace ilang {
namespace vta {
// vta top level parameter: https://tvm.apache.org/docs/vta/dev/config.html
// When a parameter name is preceded with LOG, it means that it describes a value 
// that can only be expressed a power of two. For that reason we describe these 
// parameters by their log2 value. For instance, to describe an integer width of 
// 8-bits for the input data types, we set the LOG_INP_WIDTH to be 3, which is 
// the log2 of 8. Similarly, to descibe a 64kB micro-op buffer, we would set 
// LOG_UOP_BUFF_SIZE to be 16.

// The data is captured from vta_config.json 
// Input data type signed integer width.
#define LOG_INPUT_DATA_BITWIDTH 3
#define INPUT_DATA_BITWIDTH (1 << LOG_INPUT_DATA_BITWIDTH)
// weight data type signed integer width
#define LOG_WEIGHT_BITWIDTH 3
#define WEIGHT_BITWIDTH (1 << LOG_WEIGHT_BITWIDTH)
// accumulator data type signed integer width
#define LOG_ACCUM_BITWIDTH 5
#define ACCUM_BITWIDTH (1 << LOG_ACCUM_BITWIDTH)
// VTA matrix multiply intrinsic input/output dimension 0
#define LOG_BATCH_SIZE 0
#define BATCH_SIZE (1 << LOG_BATCH_SIZE)
// VTA matrix multiply inner dimension
#define LOG_BLOCK_SIZE 4
#define BLOCK_SIZE (1 << LOG_BLOCK_SIZE)
// Micro-op on-chip buffer in bytes
#define LOG_UOP_BUFF_SIZE 15
#define UOP_BUFF_SIZE (1 << LOG_UOP_BUFF_SIZE)
// input on-chip buffer in bytes
#define LOG_INPUT_BUFF_SIZE 15
#define INPUT_BUFF_SIZE (1 << LOG_INPUT_BUFF_SIZE)
// weight on-chip buffer in bytes
#define LOG_WEIGHT_BUFF_SIZE 18
#define WEIGHT_BUFF_SIZE (1 << LOG_WEIGHT_BUFF_SIZE)
// accumulator on-chip buffer in bytes
#define LOG_ACCUM_BUFF_SIZE 17
#define ACCUM_BUFF_SIZE (1 << LOG_ACCUM_BUFF_SIZE)


} // namespace vta
} // namespace ilang