#ifndef VTA_TOP_CONFIG_H__
#define VTA_TOP_CONFIG_H__


namespace ilang {
namespace vta {
// vta top level parameter: https://tvm.apache.org/docs/vta/dev/config.html
// When a parameter name is preceded with LOG, it means that it describes a value 
// that can only be expressed a power of two. For that reason we describe these 
// parameters by their log2 value. For instance, to describe an integer width of 
// 8-bits for the input data types, we set the LOG_INP_WIDTH to be 3, which is 
// the log2 of 8. Similarly, to descibe a 64kB micro-op buffer, we would set 
// LOG_UOP_BUFF_SIZE to be 16.

// some hardware specs are also defined in hw_spec.h

// The data is captured from vta_config.json 
// Input data type signed integer width.
#define VTA_LOG_INPUT_DATA_BITWIDTH 3
#define VTA_INPUT_DATA_BITWIDTH (1 << VTA_LOG_INPUT_DATA_BITWIDTH)
// weight data type signed integer width
#define VTA_LOG_WEIGHT_BITWIDTH 3
#define VTA_WEIGHT_BITWIDTH (1 << VTA_LOG_WEIGHT_BITWIDTH)
// accumulator data type signed integer width
#define VTA_LOG_ACCUM_BITWIDTH 5
#define VTA_ACCUM_BITWIDTH (1 << VTA_LOG_ACCUM_BITWIDTH)

// TODO: confirm output size
#define VTA_LOG_OUT_BITWIDTH VTA_LOG_INPUT_DATA_BITWIDTH
#define VTA_OUT_BITWIDTH (1 << VTA_LOG_OUT_BITWIDTH)

// VTA matrix multiply intrinsic input/output dimension 0
#define VTA_LOG_BATCH_SIZE 0
#define VTA_BATCH_SIZE (1 << VTA_LOG_BATCH_SIZE)
// VTA matrix multiply inner dimension
#define VTA_LOG_BLOCK_SIZE 4
#define VTA_BLOCK_SIZE (1 << VTA_LOG_BLOCK_SIZE)

// the missing configs can be found by running "vta_config.py --cflags"

// TODO: confirm the block in and block out values
// cannot find in the specs
#define VTA_LOG_BLOCK_IN VTA_LOG_BLOCK_SIZE
#define VTA_LOG_BLOCK_OUT VTA_LOG_BLOCK_SIZE
#define VTA_BLOCK_IN (1 << VTA_LOG_BLOCK_IN)
#define VTA_BLOCK_OUT (1 << VTA_LOG_BLOCK_OUT)



// instructions related
#define VTA_LOG_INSTR_WIDTH 7
#define VTA_INSTR_BITWIDTH (1 << VTA_LOG_INSTR_WIDTH)

#define VTA_LOG_UOP_WIDTH 5
#define VTA_UOP_BITWIDTH (1 << VTA_LOG_UOP_WIDTH)

// Memory bus width
// TODO: cannot find the value of this parameter
// This bus width is the hardware input bitwidth
// TODO: we can abstract away the bus width, this should be a low leve implementation detail
#define VTA_LOG_BUS_WIDTH 6
#define VTA_BUS_BITWIDTH (1 << VTA_LOG_BUS_WIDTH)





} // namespace vta
} // namespace ilang

#endif