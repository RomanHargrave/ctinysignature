#include "../vm.h"

#if !defined(__VM_IMPL_H)
#define __VM_IMPL_H

#define OPCODE_IMPL_NEW(name) \
   opcode_t(name)* OPCODE(name) ## _new()

#define OPCODE_IMPL_FREE(name, ptr_name) \
   void OPCODE(name) ## _free(opcode_t(name)* ptr_name)

#define OPCODE_IMPL_EXEC(name, ptr_name, state_name) \
   void OPCODE(name) ## _exec(opcode_t(name)* ptr_name, struct vm_state* state_name)

#define OPCODE_IMPL_TOSTR(name, ptr_name, buf_name, blen_name) \
   ssize_t OPCODE(name) ## _tostr(opcode_t(name)* ptr_name, char* buf_name, size_t blen_name)

#define OPCODE_IMPL_STRERR(name, ptr_name, data_name, buf_name, blen_name) \
   ssize_t OPCODE(name) ## _strerr(opcode_t(name)* ptr_name, void* data_name, char* buf_name, size_t blen_name);

#define init_opcode(name, instance) instance->id = OPCODE_ID(name)

#endif
