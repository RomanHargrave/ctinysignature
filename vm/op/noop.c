#include <stdio.h>
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

OPCODE_IMPL_TOSTR(noop, op, buf, blen) {
   // sure, could be statically replaced with '5'
   return snprintf(buf, blen, "noop");
}

OPCODE_IMPL_STRERR(noop, op, data, buf, blen) {
   return snprintf(buf, blen, "noop does not specify any errors.");
}
