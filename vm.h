#include <stdint.h>

#if !defined(__VM_H)
#define __VM_H

#if !defined(VM_REGISTER_COUNT) || VM_REGISTER_COUNT < 3
#  define VM_REGISTER_COUNT 3
#endif


struct vm_state;

#define OPCODE(name) OP_ ## name;
#define OPCODE_ID(name) OPCODE(name) ## _id
#define opcode_t(name) struct OPCODE(name)

#define OPCODE_DEF(id, name, fields) \
   static uint16_t OPCODE_ID(name) = id; \
   struct OPCODE(name) { uint16_t const id; fields ; }; \
   struct OPCODE(name) * OPCODE(name) ## _new(); \
   void OPCODE(name) ## _free(struct OPCODE(name) *); \
   void OPCODE(name) ## _exec(struct OPCODE(name), struct vm_state *);

#define opcode_new(name) OPCODE(name) ## _new()
#define opcode_free(name, ptr) OPCODE(name) ## _free(ptr)
#define opcode_exec(name, ptr, state) OPCODE(name) ## _exec(ptr, state)

/**
 * Does nothing
 */
OPCODE_DEF(0x0000, noop, )

/**
 * Forward seek by <imm> bytes
 */
OPCODE_DEF(0x0001, fs,
      uint64_t imm;
)

/**
 * Forward seek by number of bytes in register #<reg>
 */
OPCODE_DEF(0x0002, fsr,
      uint8_t reg;
)

/**
 * Reverse seek by <imm> bytes
 */
OPCODE_DEF(0x0003, rs,
      uint64_t imm;
)

/**
 * Reverse seek by number of bytes in register #<reg>
 */
OPCODE_DEF(0x0004, rsr,
      uint8_t reg;
)

/**
 * Move cursor directly to position <imm> on input
 */
OPCODE_DEF(0x0005, si,
      uint64_t imm;
)

/**
 * Move cursor directly to position in register #<reg> on input
 */
OPCODE_DEF(0x0006, sr,
      uint8_t reg;
)

/**
 * Copy cursor position in to register #<reg>
 */
OPCODE_DEF(0x0007, tel,
      uint8_t reg;
)

/**
 * Compare eight bits of input data (starting at cursor) to <imm>
 * 
 * If equal, r0 will be zero. If not equal, r0 will be non-zero.
 */
OPCODE_DEF(0x0010, icmpb,
      uint8_t imm;
)

/**
 * Compare sixteen bits of input data (starting at cursor) to <imm>
 *
 * If equal, r0 will be zero. If not equal, r0 will be non-zero.
 */
OPCODE_DEF(0x0011, icmpw,
      uint16_t imm;
)

/**
 * Compare thirty-two bits of input to <imm>
 *
 * If equal, r0 will be zero. If not equal, r0 will be non-zero.
 */
OPCODE_DEF(0x0012, icmpd,
      uint32_t imm;
)

/**
 * Compare sixty-four bits of input to <imm>.
 *
 * If equal, r0 will be zero. If not equal, r0 will be non-zero.
 */
OPCODE_DEF(0x0013, icmpq,
      uint32_t imm;
)

/**
 * Compare four lowest bits of current byte to four lowest bits of <imm>
 *
 * If equal, r0 will be zero. If not, r0 will be non-zero.
 */
OPCODE_DEF(0x0014, icmpln,
      uint8_t imm;
)

/**
 * Compare four highest bits of current byte to four highest bits of <imm>
 *
 * If equal, r0 will be zero. If not, r0 will be non-zero.
 */
OPCODE_DEF(0x0015, icmphn,
      uint8_t imm;
)

/**
 * Compare (immediate byte sequence)
 *
 * Compares <size> bytes starting at the cursor with <block>
 *
 * If equal, r0 will be zero. If not equal, r0 will be non-zero.
 */
OPCODE_DEF(0x001F, icmps,
      size_t   size;
      uint8_t* sequence;
)

/**
 * Subtract register #<rhs_reg> from #<lhs_reg> and store absolute value in #<dest_reg>
 */
OPCODE_DEF(0x0020, sub,
      uint8_t dest_reg;
      uint8_t lhs_reg;
      uint8_t rhs_reg;
)

/**
 * Subtract <rhs_imm> from register #<lhs_reg> and store absolute value in #<dest_reg>
 */
OPCODE_DEF(0x0021, subi,
      uint8_t dest_reg;
      uint8_t lhs_reg;
      uint64_t rhs_imm;
)

/**
 * Add register #<lhs_reg> to register #<rhs_reg> and store absolute value in register #<rhs_reg>
 */
OPCODE_DEF(0x0022, add,
      uint8_t dest_reg;
      uint8_t lhs_reg;
      uint8_t rhs_reg;
)

/**
 * Add register #<lhs_reg> to <rhs_imm> and store in #<dest_reg>
 */
OPCODE_DEF(0x0024, addi,
      uint8_t dest_reg;
      uint8_t lhs_reg;
      uint64_t rhs_reg;
)

/**
 * Load register immediate
 *
 * Loads a 64-bit value (<imm>) in to register #<reg> (the return register)
 */
OPCODE_DEF(0x0030, lri,
      uint64_t imm;
      uint8_t  reg;
)

/**
 * Load register from input material, eight bits (byte)
 *
 * Loads eight bits from the input material in to register #<reg> from the input,
 * advancing the cursor
 */
OPCODE_DEF(0x0031, lrb,
      uint8_t reg;
)

/**
 * Load register from input material, sixteen blts (word)
 */
OPCODE_DEF(0x0032, lrw,
      uint8_t reg;
)

/**
 * Load register from input material, thirty-two bits (double-word)
 */
OPCODE_DEF(0x0033, lrd,
      uint8_t reg;
)

/**
 * Load register from input material, sixty-four bits (quad-word)
 */
OPCODE_DEF(0x0034, lrq,
      uint8_t reg;
)

/**
 * Load register #<dest> from register #<src>
 */
OPCODE_DEF(0x0035, lr,
      uint8_t src;
      uint8_t dest;
)

/**
 * Push contents of register #<reg> on to the stack
 */
OPCODE_DEF(0x0036, push,
      uint8_t reg;
)

/**
 * Pops a element off the stack and stores it in register #<reg>
 */
OPCODE_DEF(0x0037, pop,
      uint8_t reg;
)

/**
 * Halt program execution and yield <imm>
 */
OPCODE_DEF(0xF001, yield,
      uint64_t imm;
)

/**
 * Halt program execution and yield contents of #<reg>
 */
OPCODE_DEF(0xF002, yieldr,
      uint8_t reg;
)

/**
 * Set program counter to <imm> if #<reg> == 0
 */
OPCODE_DEF(0xF010, jz,
      uint64_t imm;
      uint8_t  reg;
)

/**
 * Set program counter to <imm> if #<reg> != 0
 */
OPCODE_DEF(0xF011, jnz,
      uint64_t imm;
      uint8_t  reg;
)

/**
 * Set program counter to <imm>
 */
OPCODE_DEF(0xF012, jmp,
      uint64_t imm;
)

enum vm_disposition {
   /**
    * The initial state of the VM
    */
   VM_DISPOSITION_INITIAL   = 0,

   /**
    * State that the VM is in when executing instructions
    */
   VM_DISPOSITION_EXECUTE   = 1,

   /**
    * State that the VM is placed in to by a yield instruction.
    * The VM will halt and return a result to the caller.
    */
   VM_DISPOSITION_YIELD     = 2,

   /**
    * State that the VM is placed in to when a VM-level error occurs.
    * The VM will halt and return an error code to the caller.
    */
   VM_DISPOSITION_ERROR     = 3,

   /**
    * State that the VM is placed in to when it has exhausted the program buffer
    * without evaluating a yield instruction.
    * The VM will halt and return an empty result to the caller.
    */
   VM_DISPOSITION_EXHAUSTED = 4,

   /**
    * State that the VM is placed in to by a breakpoint or pause instruction.
    */
   VM_DISPOSITION_HALT      = 5,

   VM_DISPOSITION_MAX
};

struct vm_state {
   /**
    * The VM disposition is used to signal execution state changes to the
    * VM execution thread.
    */
   enum vm_disposition disposition;

   uint64_t result;
   uint64_t error;

   uint8_t* input;
   uint8_t* input_tip;
   uint64_t input_size;
   uint64_t input_pos;

   opcode_t(noop)** pgm;
   opcode_t(noop)*  pgm_tip;
   uint64_t         pgm_size;
   uint64_t         pgm_pos;

   uint64_t* stack;
   uint64_t* stack_tip;
   uint64_t  stack_size;
   uint64_t  stack_pos;

   uint64_t  reg[VM_REGISTER_COUNT];
};

/**
 * Creates and initializes a new VM state.
 */
struct vm_state* vm_new();

/**
 * Sets the VM input data, resetting the cursor
 */
int vm_set_input(uint8_t* /* input */, uint64_t /* input size */);

/**
 * Sets the VM program, resetting related counters.
 */
int vm_set_pgm(opcode_t(noop)** /* program */, uint64_t /* instruction count */);

/**
 * Resize the VM stack to the given number of sixty-four bit integers
 */
int vm_resize_stack(uint64_t /* new number of stack elements */);

void vm_free(struct vm_state*);

inline uint64_t* vm_register(struct vm_state* state, uint8_t id) {
   if (id > VM_REGISTER_COUNT) {
      return NULL;
   } else {
      return &state->reg[id];
   }
}

#endif
