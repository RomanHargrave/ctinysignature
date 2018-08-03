#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctinysignature/pattern.h>

static inline uint8_t
cts_hex_nibble_val (char p_hex_nibble) {
   if (p_hex_nibble >= '0' && p_hex_nibble <= '9')
   {
      return (uint8_t) (p_hex_nibble - 48);
   }
   else
   if (p_hex_nibble >= 'A' && p_hex_nibble <= 'F')
   {
      return (uint8_t) (p_hex_nibble - 55);
   }
   else
   if (p_hex_nibble >= 'a' && p_hex_nibble <= 'f')
   {
      return (uint8_t) (p_hex_nibble - 87);
   }
}

/**
 * Is character [A-Fa-f0-9]
 */
static inline uint8_t
cts_is_hex_char (char p_test)
{
   return (p_test >= '0' && p_test <= '9') || (p_test >= 'A' && p_test <= 'F') || (p_test >= 'a' && p_test <= 'f');
}

struct s_rule*
cts_parse_rule (char* p_rule_spec)
{
   size_t const spec_len = strlen(p_rule_spec);

   // Allocate a buffer to store the pattern in while reading it. spec_len / 2 should represent the maximum theoretical size of the compiled pattern.
   size_t pattern_buf_size = sizeof(uint8_t) * ceil(((double) spec_len / 2.f));
   uint8_t* pattern_buffer = malloc(pattern_buf_size);

   printf("pattern buffer size: %lu\n", pattern_buf_size);

   // Track the number of bytes actually written in to the pattern buffer
   size_t pattern_length = 0;

   size_t trailing_blanks = 0;

   // Return value
   struct s_rule* segment_rule = NULL;

   size_t tok_idx = 0;

   // Read in pattern data
   {
      do
      {
         char tok = p_rule_spec[tok_idx++];

         if ((tok == '?' && pattern_length > 0) || tok == 0)
         {
            --tok_idx; // Rewind by one for correct accounting
            break;
         }
         else
         if (cts_is_hex_char(tok)) // The high nibble is present
         {
            if (tok_idx < spec_len) // Is another character available
            {
               char next_tok = p_rule_spec[tok_idx++];

               if (cts_is_hex_char(tok)) // Expect low nibble
               {
                  *(pattern_buffer + pattern_length++) = cts_hex_nibble_val(tok) << 4 | cts_hex_nibble_val(next_tok);
               }
               else
               {
                  fprintf(stderr, "Hanging character `%c` (pos %u in pattern `%s`) needs a low nibble but the next character (`%c`) is not a hexadecimal digit\n", tok, tok_idx - 2, p_rule_spec, next_tok);
                  goto cleanup_retn;
               }
            }
            else
            {
               fprintf(stderr, "Hanging character `%c` (pos %u in pattern `%s`) needs a low nibble but EOS has been reached\n", tok, tok_idx - 1, p_rule_spec);
               goto cleanup_retn;
            }
         }
      }
      while (tok_idx < spec_len);

      // Count trailer
      do
      {
         char tok = p_rule_spec[tok_idx++];
         fprintf(stderr, "check trailer: %c\n", tok);
         if (tok == 0 || cts_is_hex_char(tok))
         {
            --tok_idx; // Rewind by one for correct accounting
            break;
         }
         else
         if (tok == '?')
         {
            // Allow for ?? to mean one byte, for aesthetic reasons
            if (tok_idx < spec_len && (p_rule_spec[tok_idx] == '?'))
            {
               ++tok_idx; // Skip
            }

            ++trailing_blanks;
         }
      }
      while (tok_idx < spec_len);

   } // end reading pattern data

   if (tok_idx > spec_len)
   {
      fprintf(stderr, "Pattern string iterator overrun (at %u but string is %u bytes)\n", tok_idx, spec_len);
      goto cleanup_retn;
   }

   // If a pattern was read, copy it out of the match data buffer in to a new rule
   if (pattern_length > 0) {
      segment_rule = cts_new_rule(pattern_length);
      segment_rule->undefined_trailing_length = trailing_blanks;
      memcpy(segment_rule->match_pattern, pattern_buffer, pattern_length);
      free(pattern_buffer);
      pattern_buffer = NULL;

      // More to read?
      if (spec_len > tok_idx)
      {
         // Read next rule from end of this rule spec
         segment_rule->next_pattern = cts_parse_rule(p_rule_spec + tok_idx);
      }

   }


cleanup_retn:
   if (pattern_buffer)
   {
      free(pattern_buffer);
   }

   return segment_rule;
}
