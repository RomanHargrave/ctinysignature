/**
 * Binary Pattern VM
 * 'fs' opcode implementation
 *
 * (C) 2019 Roman Hargrave <roman@hargrave.info>
 */

#include <stdio.h>
#include "../vm_impl.h"

enum op_fs_err {
   FS_SEEK_EXCEED = 0
};

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
   if (op->imm + state->input_pos > state->input_size) {
      state->disposition  = VM_DISPOSITION_OP_ERROR;
      state->opcode_error = &FS_SEEK_EXCEED;
   } else {
      state->input_pos += op->imm;
   }
}

OPCODE_IMPL_TOSTR(fs, op, buf, blen) {
   return snprintf(buf, blen, "fs %d", op->imm);
}

OPCODE_IMPL_STRERR(fs, op, data, buf, blen) {
   if (data == NULL) {
      return snprintf(buf, blen, "No error code specified");
   } else {
      enum op_fs_err err = *((enum op_fs_err*) data);

      switch (err) {
         case FS_SEEK_EXCEED:
            return snprintf(buf, blen, "Forward seek of %d bytes would exceed end of input", op->imm);
         default:
            return snprintf(buf, blen, "Unknown error %x", err);
      }
   }
}
