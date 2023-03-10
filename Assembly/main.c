#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "support.h"
#include "wires.h"
#include "arithmetic.h"
#include "memories.h"
#include "alu.h"

#define HLT 0x0
#define NOP 0x1
#define MOV_RtoR 0x2
#define MOV_ItoR 0x3
#define MOV_RtoM 0x4
#define MOV_MtoR 0x5
#define ARITHMETIC 0x6
#define JCC 0x7
#define CALL 0x8
#define RET 0x9
#define PUSH 0xA
#define POP 0xB

#define REG_SP from_int(4)

int main(int argc, char* argv[]) {

    // Check command line parameters.
    if (argc < 2)
        error("missing name of programfile to simulate");

    trace_reader_p tracer = NULL;
    if (argc == 3) {
        tracer = create_trace_reader(argv[2]);
    }

    // Setup global state.
    // Each cycle ends by capturing computations in global state.

    // Registerfile:
    // - we use separate read and write ports for sp updates. This can be optimized away.
    // - There are other options - the datapath diagram, for example, uses only two
    //   read ports and two write ports - one for writing results from the ALU, another
    //   for writing results from memory.
    mem_p regs = memory_create(16,3,2); // 8-byte cells, 3 readports, 2 writeport

    // Memory:
    // - Shared instruction and data memory. Two read ports for instruction fetch,
    //   one read and one write for data.
    mem_p mem = memory_create(1024,3,1); // 8-byte cells, 3 readports, 1 writeport
    memory_read_from_file(mem, argv[1]);
    // special registers
    val pc = from_int(0);
    conditions cc; cc.of = cc.sf = cc.zf = false;

    // a stop signal for stopping the simulation.
    bool stop = false;

    // We need the instruction number for trace validation
    int instruction_number = 0;

    while (!stop) { // for each cycle:

        instruction_number++;
        // fetch next instruction
        val inst_word = memory_read_unaligned(mem, 0, 1, pc, true);

        // decode - here we pick apart the instruction in major and minor opcode,
	      // register specifiers and any immediate value. Next we identify all the
	      // instructions and generate control signals:
        val major_op = pick_bits(4,4, inst_word);
        val minor_op = pick_bits(0,4, inst_word);

        // instruction
        bool is_halt = is(HLT, major_op);
        bool is_move = is(MOV_RtoR, major_op) | is(MOV_ItoR, major_op) | is(MOV_RtoM, major_op) | is(MOV_MtoR, major_op);
        bool is_itor = is(MOV_ItoR, major_op);
        bool is_rtom = is(MOV_RtoM, major_op);
        bool is_mtor = is(MOV_MtoR, major_op);
        bool is_cmov = is_move & !is_itor & !is_rtom & !is_mtor;
        bool is_arit = is(ARITHMETIC, major_op);
        bool is_jcc  = is(JCC, major_op);
        bool is_call = is(CALL, major_op);
        bool is_ret  = is(RET, major_op);
        bool is_push = is(PUSH, major_op);
        bool is_pop  = is(POP, major_op);

        // conditional move/branch
        bool is_always  = is(ALWAYS, minor_op);
        bool is_lessEq  = is(LE, minor_op);
        bool is_less    = is(LT, minor_op);
        bool is_equal   = is(EQ, minor_op);
        bool is_notEq   = is(NE, minor_op);
        bool is_greatEq = is(GE, minor_op);
        bool is_greater = is(GT, minor_op);

        // operations for arithmetic
        bool is_addq = is(ADD, minor_op);
        bool is_subq = is(SUB, minor_op);
        bool is_andq = is(AND, minor_op);
        bool is_xorq = is(XOR, minor_op);
        bool is_cmpq = is(CMP, minor_op);

        bool has_regs = is_move | is_arit | is_push | is_pop;
        bool has_imm  = is_itor | is_rtom | is_mtor | is_jcc | is_call;
        bool uses_rsp = is_push | is_pop  | is_call | is_ret;

        val size = or(use_if( has_regs & has_imm, from_int(6)),
                   or(use_if( has_regs &!has_imm, from_int(2)),
                   or(use_if(!has_regs &!has_imm, from_int(1)),
                      use_if(!has_regs & has_imm, from_int(5)))));

        val reg_a = pick_bits(12,4, inst_word);
        val reg_b = pick_bits(8,4, inst_word);

        val imm_bytes = or(use_if(!has_regs, pick_bits( 8, 32, inst_word)),
                           use_if( has_regs, pick_bits(16, 32, inst_word)));

        val imm = imm_bytes;

        //val sign_extended_imm = sign_extend(31,imm);

        val next_inst_pc = add(pc, size);

        stop = is_halt;

        // execute - for now, this is just reading out operands from their registers
        val op_a = memory_read(regs, 0, reg_a, has_regs);
        val op_b = memory_read(regs, 1, reg_b, has_regs);

        // read the stack pointer
        val rsp = memory_read(regs, 2, REG_SP, uses_rsp);

        // Read memory specified by imm.
        val mem_op_b = memory_read(mem, 2, add(op_a, imm), is_mtor);

        // Read the value in memory that the stack pointer points at.
        val mem_stack_top = memory_read(mem, 2, rsp, is_pop | is_ret);

        // Determine type of condition
        val condition = use_if(is_cmov | is_jcc, or(use_if(is_always , from_int(ALWAYS)),
                                                 or(use_if(is_lessEq , from_int(LE)),
                                                 or(use_if(is_less   , from_int(LT)),
                                                 or(use_if(is_equal  , from_int(EQ)),
                                                 or(use_if(is_notEq  , from_int(NE)),
                                                 or(use_if(is_greatEq, from_int(GE)),
                                                    use_if(is_greater, from_int(GT)))))))));

        bool condition_holds = eval_condition(cc, condition);

        // Determine type of operation;
        val operation = use_if(is_arit, or(use_if(is_addq, from_int(ADD)),
                                        or(use_if(is_subq, from_int(SUB)),
                                        or(use_if(is_andq, from_int(AND)),
                                        or(use_if(is_xorq, from_int(XOR)),
                                           use_if(is_cmpq, from_int(CMP)))))));

        // executes the operation
        alu_execute_result op_q = alu_execute(operation, op_a, op_b);

        // Sets the flags
        cc.of = use_if(is_arit, from_int(op_q.cc.of)).val;
        cc.sf = use_if(is_arit, from_int(op_q.cc.sf)).val;
        cc.zf = use_if(is_arit, from_int(op_q.cc.zf)).val;

        // select result for register update
        val datapath_result = or(use_if(is_cmov, op_a),
                              or(use_if(is_itor, imm),
                              or(use_if(is_mtor, mem_op_b),
                              or(use_if(is_rtom, op_a),
                              or(use_if(is_arit, op_q.result),
                              or(use_if(is_push | is_call , alu_execute(from_int(SUB), from_int(8), rsp).result),
                                (use_if(is_pop  | is_ret  , add(rsp, from_int(8))))))))));

        // what to write to memory
        val memory_result = or(use_if(is_call, add(pc, size)),
                            or(use_if(is_push, op_a),
                               use_if(is_rtom, datapath_result)));


        // pick target register
        val target_reg = or(use_if(uses_rsp, REG_SP),
                         or(use_if(is_mtor, reg_a),
                            use_if(is_cmov | is_itor | is_arit, reg_b)));

        val target_mem = or(use_if(is_rtom, add(op_b, imm)),
                            use_if(is_push | is_call, datapath_result));


        bool reg_wr_enable = (is_cmov & condition_holds) | is_itor | (is_arit & !is_cmpq) | is_push | is_pop | is_call;
        bool mem_wr_enable = is_rtom | is_push | is_call;
        printf("%s%d\n", "reg_wr_enable = ", reg_wr_enable);
        // Determine next position:
        val next_pc = or(use_if(is_call, imm),
                      or(use_if(is_ret, mem_stack_top),
                      or(use_if(is_jcc  & condition_holds, imm),
                      or(use_if(is_jcc  & !condition_holds, next_inst_pc),
                        (use_if(!is_jcc & !is_call & !is_ret, next_inst_pc))))));
        // potentially pretty-print something to show progress before
        // ending cycle and overwriting state from start of cycle:
        for (int j=0; j<size.val; ++j) {
          unsigned int byte = (inst_word.val >> (8*j)) & 0xff;
            printf("%x ", byte);
        }
        if (reg_wr_enable) {
            printf("\t\tr%ld = %lx\n", target_reg.val, datapath_result.val);
        }
        else if (mem_wr_enable) {
            printf("\t\tr%ld = %lx\n", target_mem.val, datapath_result.val);
        }
        else {
            printf("\n");
        }
        if ((tracer != NULL) & !stop) {
            // Validate values written to registers and memory against trace from
            // reference simulator. We do this *before* updating registers in order
            // to have the old register content available in case trace validation
            // fails. If so, the error function is called, and by setting a breakpoint
            // on it, all values are easy to inspect with a debugger.
            validate_pc_wr(tracer, instruction_number, next_pc);
            if (reg_wr_enable) {
                validate_reg_wr(tracer, instruction_number, target_reg, datapath_result);
            }
            else if (mem_wr_enable) {
                validate_mem_wr(tracer, instruction_number, target_mem, memory_result);
            }
	    // For A2 you'll need to add validation for the other register written
	    // (for instructions which do write the other register)
	    // AND you'll need to add a call to validate memory writes to check
	    // movq %rX,D(%rY) once that is implemented. You can use validate_mem_wr
	    // found in "support.h"
        }

        // determine PC for next cycle. Right now we can only execute next in sequence.

        // store results at end of cycle
        // For A2 you need to add updating the condition code register and you'll
        // need to write to an additional register for some instructions and implement
        // writing to memory.
        pc = next_pc;
        // Update a register (may also be stack pointer)
        memory_write(regs, 1, target_reg, datapath_result, reg_wr_enable);
        // Update a register (only used during pop)
        memory_write(regs, 2, reg_a, mem_stack_top, is_pop);
        // Update some memory
        memory_write(mem, 1, target_mem, memory_result, mem_wr_enable);
    }
    printf("Done\n");
}
