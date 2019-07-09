/**
 * Binary Pattern VM
 * 'fs' opcode implementation
 *
 * (C) 2019 Roman Hargrave <roman@hargrave.info>
 */

#include "../vm_impl.h"

OPCODE_IMPL_NEW(fs) {
   opcode_t(fs)* op = malloc(sizeof(*op));
   init_opcode(fs, op);
   return fs;
}

OPCODE_IMPL_FREE(fs, op) {
   if (op) {
      free(op);
   }
}

OPCODE_IMPL_EXEC(fs, op, state) {
}
