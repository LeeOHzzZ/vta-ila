// File: vta_instr.cc
// this file defines top VTA instructions
#include <ilang/ilang++.h>
#include <vta/vta_top.h>
#include <ilang/util/log.h>

namespace ilang {
namespace vta {

void DefineInstr(Ila& m) {
  
  auto vta_ins_in = m.input(VTA_TOP_INSTR_IN);
  auto opcode = Extract(vta_ins_in, VTA_OPCODE_BITWIDTH-1, 0);
  // use ins_temp for slicing instructions parameters
  auto ins_temp = (vta_ins_in >> VTA_OPCODE_BITWIDTH);
  
  { // instruction load
    // load weights
    auto instr = m.NewInstr("vta_load_wgt");
    // skip unused parameters
    ins_temp = ins_temp >> 4;
    
    auto memory_type = Extract(ins_temp, VTA_MEMOP_ID_BITWIDTH-1, 0);
    ins_temp = (ins_temp >> VTA_MEMOP_ID_BITWIDTH);

    auto is_opcode_load = (opcode == VTA_OPCODE_LOAD);
    auto is_wgt = (memory_type == VTA_MEM_ID_WGT);

    instr.SetDecode(is_opcode_load & is_wgt);

    // decode instruction parameters from the instruction
    auto sram_base = Extract(ins_temp, VTA_MEMOP_SRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SRAM_ADDR_BITWIDTH;
    
    auto dram_base = Extract(ins_temp, VTA_MEMOP_DRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_DRAM_ADDR_BITWIDTH;
    
    auto y_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;
    
    auto x_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;

    auto x_stride = Extract(ins_temp, VTA_MEMOP_STRIDE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_STRIDE_BITWIDTH;
    
    instr.SetUpdate(m.state(VTA_SRAM_ID), sram_base);
    instr.SetUpdate(m.state(VTA_DRAM_ID), dram_base);
    instr.SetUpdate(m.state(VTA_Y_SIZE), y_size);
    instr.SetUpdate(m.state(VTA_X_SIZE), x_size);
    instr.SetUpdate(m.state(VTA_X_STRIDE), x_stride);

    instr.SetUpdate(m.state(VTA_LOAD_Y_CNTR), BvConst(0, VTA_LOAD_Y_CNTR_BITWIDTH));

    instr.SetUpdate(m.state(VTA_CHILD_VALID_FLAG),
                    BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_INSTR_STATE),
                    BvConst(VTA_CHILD_STATE_LOAD_WGT_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH));
  }

  { // instruction load inputs
    auto instr = m.NewInstr("vta_load_input");
    // skip unused parameters
    ins_temp = ins_temp >> 4;

    auto memory_type = Extract(ins_temp, VTA_MEMOP_ID_BITWIDTH-1, 0);
    ins_temp = (ins_temp >> VTA_MEMOP_ID_BITWIDTH);

    auto is_opcode_load = (opcode == VTA_OPCODE_LOAD);
    auto is_inp = (memory_type == VTA_MEM_ID_INP);

    instr.SetDecode(is_opcode_load & is_inp);

    // decode instruction parameters from the instruction
    auto sram_base = Extract(ins_temp, VTA_MEMOP_SRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SRAM_ADDR_BITWIDTH;
    
    auto dram_base = Extract(ins_temp, VTA_MEMOP_DRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_DRAM_ADDR_BITWIDTH;
    
    auto y_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;
    
    auto x_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;

    auto x_stride = Extract(ins_temp, VTA_MEMOP_STRIDE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_STRIDE_BITWIDTH;

    auto y_pad_0 = Extract(ins_temp, VTA_MEMOP_PAD_BITWIDTH-1, 0);
    auto y_pad_1 = Extract(ins_temp, 2*VTA_MEMOP_PAD_BITWIDTH-1, VTA_MEMOP_PAD_BITWIDTH);
    auto x_pad_0 = Extract(ins_temp, 3*VTA_MEMOP_PAD_BITWIDTH-1, 2*VTA_MEMOP_PAD_BITWIDTH);
    auto x_pad_1 = Extract(ins_temp, 4*VTA_MEMOP_PAD_BITWIDTH-1, 3*VTA_MEMOP_PAD_BITWIDTH);

    instr.SetUpdate(m.state(VTA_SRAM_ID), sram_base);
    instr.SetUpdate(m.state(VTA_DRAM_ID), dram_base);
    instr.SetUpdate(m.state(VTA_Y_SIZE), y_size);
    instr.SetUpdate(m.state(VTA_X_SIZE), x_size);
    instr.SetUpdate(m.state(VTA_X_STRIDE), x_stride);
    instr.SetUpdate(m.state(VTA_Y_PAD_0), y_pad_0);
    instr.SetUpdate(m.state(VTA_Y_PAD_1), y_pad_1);
    instr.SetUpdate(m.state(VTA_X_PAD_0), x_pad_0);
    instr.SetUpdate(m.state(VTA_X_PAD_1), x_pad_1);

    auto pad_cntr = m.state(VTA_LOAD_INP_PAD_CNTR);
    // auto x_size_ext = 
    //   Concat(BvConst(0, pad_cntr.bit_width() - x_size.bit_width()), x_size);
    auto x_size_ext = x_size;
    auto x_pad_0_ext = 
      Concat(BvConst(0, pad_cntr.bit_width() - x_pad_0.bit_width()), x_pad_0);
    auto x_pad_1_ext = 
      Concat(BvConst(0, pad_cntr.bit_width() - x_pad_1.bit_width()), x_pad_1);
    auto y_pad_0_ext = 
      Concat(BvConst(0, pad_cntr.bit_width() - y_pad_0.bit_width()), y_pad_0);
    auto y_pad_1_ext = 
      Concat(BvConst(0, pad_cntr.bit_width() - y_pad_1.bit_width()), y_pad_1);

    auto y_offset_0 = (x_pad_0_ext + x_size_ext + x_pad_1_ext) * y_pad_0_ext;
    auto y_offset_1 = (x_pad_0_ext + x_size_ext + x_pad_1_ext) * y_pad_1_ext;

    // initiate the cntr
    instr.SetUpdate(m.state(VTA_LOAD_Y_CNTR), BvConst(0, VTA_LOAD_Y_CNTR_BITWIDTH));
    instr.SetUpdate(pad_cntr, BvConst(0, VTA_LOAD_INP_PAD_CNTR_BITWIDTH));
    instr.SetUpdate(m.state(VTA_LOAD_Y_OFFSET_0), y_offset_0);
    instr.SetUpdate(m.state(VTA_LOAD_Y_OFFSET_1), y_offset_1);

    instr.SetUpdate(m.state(VTA_CHILD_VALID_FLAG),
                    BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_INSTR_STATE),
                    BvConst(VTA_CHILD_STATE_LOAD_INP_Y_OFFSET_0, VTA_CHILD_INSTR_STATE_BITWIDTH));
  }

  { // instruction load uops
    auto instr = m.NewInstr("vta_load_uop");
    // skip unused parameters
    ins_temp = ins_temp >> 4;

    auto memory_type = Extract(ins_temp, VTA_MEMOP_ID_BITWIDTH-1, 0);
    ins_temp = (ins_temp >> VTA_MEMOP_ID_BITWIDTH);

    auto is_opcode_load = (opcode == VTA_OPCODE_LOAD);
    auto is_uop = (memory_type == VTA_MEM_ID_UOP);

    instr.SetDecode(is_opcode_load & is_uop);

    auto sram_base = Extract(ins_temp, VTA_MEMOP_SRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SRAM_ADDR_BITWIDTH;
    
    auto dram_base = Extract(ins_temp, VTA_MEMOP_DRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_DRAM_ADDR_BITWIDTH;

    // omit the y_size, which is unused
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;
    auto x_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);

    instr.SetUpdate(m.state(VTA_SRAM_ID), sram_base);
    instr.SetUpdate(m.state(VTA_DRAM_ID), dram_base);
    instr.SetUpdate(m.state(VTA_X_SIZE), x_size);

    instr.SetUpdate(m.state(VTA_LOAD_X_CNTR), BvConst(0, VTA_LOAD_X_CNTR_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_VALID_FLAG),
                    BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_INSTR_STATE),
                    BvConst(VTA_CHILD_STATE_LOAD_UOP, VTA_CHILD_INSTR_STATE_BITWIDTH));
  }

  { // instruction load_bias
    auto instr = m.NewInstr("vta_load_bias");
    // skip unused parameters
    ins_temp = ins_temp >> 4;

    auto memory_type = Extract(ins_temp, VTA_MEMOP_ID_BITWIDTH-1, 0);
    ins_temp = (ins_temp >> VTA_MEMOP_ID_BITWIDTH);

    auto is_opcode_load = (opcode == VTA_OPCODE_LOAD);
    auto is_bias = (memory_type == VTA_MEM_ID_ACC);
    instr.SetDecode(is_opcode_load & is_bias);

    // decode instruction parameters from the instruction
    auto sram_base = Extract(ins_temp, VTA_MEMOP_SRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SRAM_ADDR_BITWIDTH;
    
    auto dram_base = Extract(ins_temp, VTA_MEMOP_DRAM_ADDR_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_DRAM_ADDR_BITWIDTH;
    
    auto y_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;
    
    auto x_size = Extract(ins_temp, VTA_MEMOP_SIZE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_SIZE_BITWIDTH;

    auto x_stride = Extract(ins_temp, VTA_MEMOP_STRIDE_BITWIDTH-1, 0);
    ins_temp >> VTA_MEMOP_STRIDE_BITWIDTH;
    
    instr.SetUpdate(m.state(VTA_SRAM_ID), sram_base);
    instr.SetUpdate(m.state(VTA_DRAM_ID), dram_base);
    instr.SetUpdate(m.state(VTA_Y_SIZE), y_size);
    instr.SetUpdate(m.state(VTA_X_SIZE), x_size);
    instr.SetUpdate(m.state(VTA_X_STRIDE), x_stride);

    instr.SetUpdate(m.state(VTA_LOAD_Y_CNTR), BvConst(0, VTA_LOAD_Y_CNTR_BITWIDTH));

    instr.SetUpdate(m.state(VTA_CHILD_VALID_FLAG),
                    BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_INSTR_STATE),
                    BvConst(VTA_CHILD_STATE_LOAD_BIAS_Y_SIZE, VTA_CHILD_INSTR_STATE_BITWIDTH));
  
  }

  { // vta instruction for GEMM
    auto instr = m.NewInstr("vta_gemm");
    auto is_opcode_gemm = (opcode == VTA_OPCODE_GEMM);
    instr.SetDecode(is_opcode_gemm);

    // extract parameters from the instruction
    // skip unused parameter
    ins_temp = ins_temp >> 5;

    auto uop_bgn = Extract(ins_temp, VTA_GEMM_UOP_BEGIN_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_UOP_BEGIN_BITWIDTH;
    auto uop_end = Extract(ins_temp, VTA_GEMM_UOP_END_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_UOP_END_BITWIDTH;
    auto iter_out = Extract(ins_temp, VTA_GEMM_ITER_OUT_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_ITER_OUT_BITWIDTH;
    auto iter_in = Extract(ins_temp, VTA_GEMM_ITER_IN_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_ITER_IN_BITWIDTH;
    auto dst_factor_out = Extract(ins_temp, VTA_GEMM_DST_FACTOR_OUT_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_DST_FACTOR_OUT_BITWIDTH;
    auto dst_factor_in = Extract(ins_temp, VTA_GEMM_DST_FACTOR_IN_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_DST_FACTOR_IN_BITWIDTH;
    auto src_factor_out = Extract(ins_temp, VTA_GEMM_SRC_FACTOR_OUT_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_SRC_FACTOR_OUT_BITWIDTH;
    auto src_factor_in = Extract(ins_temp, VTA_GEMM_SRC_FACTOR_IN_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_SRC_FACTOR_IN_BITWIDTH;
    auto wgt_factor_out = Extract(ins_temp, VTA_GEMM_WGT_FACTOR_OUT_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_WGT_FACTOR_OUT_BITWIDTH;
    auto wgt_factor_in = Extract(ins_temp, VTA_GEMM_WGT_FACTOR_IN_BITWIDTH-1, 0);
    ins_temp >> VTA_GEMM_WGT_FACTOR_IN_BITWIDTH;

    instr.SetUpdate(m.state(VTA_GEMM_UOP_BEGIN), uop_bgn);
    instr.SetUpdate(m.state(VTA_GEMM_UOP_END), uop_end);
    instr.SetUpdate(m.state(VTA_GEMM_ITER_OUT), iter_out);
    instr.SetUpdate(m.state(VTA_GEMM_ITER_IN), iter_in);
    instr.SetUpdate(m.state(VTA_GEMM_DST_FACTOR_OUT), dst_factor_out);
    instr.SetUpdate(m.state(VTA_GEMM_DST_FACTOR_IN), dst_factor_in);
    instr.SetUpdate(m.state(VTA_GEMM_SRC_FACTOR_OUT), src_factor_in);
    instr.SetUpdate(m.state(VTA_GEMM_SRC_FACTOR_IN), src_factor_in);
    instr.SetUpdate(m.state(VTA_GEMM_WGT_FACTOR_OUT), wgt_factor_out);
    instr.SetUpdate(m.state(VTA_GEMM_WGT_FACTOR_IN), wgt_factor_in);

    instr.SetUpdate(m.state(VTA_CHILD_VALID_FLAG),
                    BvConst(VTA_VALID, VTA_CHILD_VALID_FLAG_BITWIDTH));
    instr.SetUpdate(m.state(VTA_CHILD_INSTR_STATE),
                    BvConst(VTA_CHILD_STATE_GEMM_START, VTA_CHILD_INSTR_STATE_BITWIDTH));
  }


}

}
}