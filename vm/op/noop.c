#include "../vm_impl.h"

/**
 * no-op - do nothing
 */
OPCODE_IMPL_NEW(noop) {
   opcode_t(noop)* op = malloc(sizeof(*op));
   init_opcode(noop, op);
   return op;
}

OPCODE_IMPL_FREE(noop, op) {
   free(op)
}

OPCODE_IMPL_EXEC(noop, op, state) {
   return;
}
