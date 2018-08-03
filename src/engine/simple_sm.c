/**
 * Simple state-machine matching engine
 */

#include<ctinysignature/pattern.h>

#include<stdbool.h>
#include<stdint.h>
#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>

enum E_STEP_TYPE {
   T_HALT        = 0,
   T_MATCH_VALUE = 1,
   T_ADVANCE   = 2
};

struct s_match_step {

   enum E_STEP_TYPE type;

   uint8_t data;

   struct s_match_step* next;

};

static inline struct s_match_step*
cts_ssm_new_step (enum E_STEP_TYPE p_type)
{
   struct s_match_step* new = malloc(sizeof(struct s_match_step));
   new->type = p_type;
   new->next = NULL;

   return new;
}

static void
cts_ssm_dump_program (struct s_match_step* p_program)
{
   if (p_program != NULL)
   {
      struct s_match_step* current = p_program;
      while (current != NULL)
      {
         switch (current->type)
         {
            case T_HALT:
               printf("HALT:");
               break;
            case T_ADVANCE:
               printf("SKIP:");
               break;
            case T_MATCH_VALUE:
               printf("M %02X:", current->data);
               break;
            default:
               printf("? %u:", current->type);
               break;
         }
         current = current->next;
      }
      printf("\n");
   }
}

/**
 * External-facing
 * Turns struct s_rule* in to a program for cts_ssm
 */
void*
cts_ssm_compile (struct s_rule* p_inp)
{

   if (p_inp == NULL)
   {
      return cts_ssm_new_step(T_HALT);
   }
   else
   {
      struct s_match_step* head     = NULL;
      struct s_match_step* current  = head;

      for (size_t byte_idx = 0; byte_idx < p_inp->match_length; ++byte_idx)
      {
         if (head == NULL)
         {
            head = cts_ssm_new_step(T_MATCH_VALUE);
            current = head;
         }
         else
         {
            current->next = cts_ssm_new_step(T_MATCH_VALUE);
            current = current->next;
         }

         current->type = T_MATCH_VALUE;
         current->data = *(p_inp->match_pattern + byte_idx);
      }

      for (size_t skip_num = 0; skip_num < p_inp->undefined_trailing_length; ++skip_num)
      {
         current->next = cts_ssm_new_step(T_ADVANCE);
         current = current->next;

         current->type = T_ADVANCE;
         current->data = 0;
      }

      current->next = cts_ssm_compile(p_inp->next_pattern);

      return head;
   }
}

/**
 * External-facing
 * De-allocate the entire program
 */
void
cts_ssm_delete_program (void* p_program)
{
   if (p_program != NULL)
   {
      struct s_match_step* step = (struct s_match_step*) p_program;
      cts_ssm_delete_program(step->next);
      free(p_program);
   }
}

/**
 * External-facing
 * Counts the maximum number of bytes that the passed program can read
 */
size_t
cts_ssm_count_bytes (void* p_program)
{
   struct s_match_step* prog_step = (struct s_match_step*) p_program;
   size_t bytes_read = 0;

   if (p_program == NULL)
   {
      return 0;
   }
   else
   {
      do
      {
         switch (prog_step->type)
         {
            case T_ADVANCE:
            case T_MATCH_VALUE:
               ++bytes_read;
            case T_HALT:
            default:
               prog_step = prog_step->next;
         }
      }
      while (prog_step != NULL);
   }

   return bytes_read;
}


/**
 * Evaluate the program against input <p_candidate> limiting read bytes to <p_length_limit>
 * A suitable value for <p_length_limit> to ensure complete execution of a program can be
 * obtained by calling `cts_ssm_count_bytes' which will count the number of read/advance
 * steps in the program.
 */
uint8_t
cts_ssm_execute (void* p_program, uint8_t* p_candidate, size_t p_length_limit)
{
   struct s_match_step* step = (struct s_match_step*) p_program;

   if (p_program == NULL)
   {
      return true;
   }
   else
   if (p_length_limit == 0 && step->type != T_HALT)
   {
      return false;
   }
   else
   {
      uint8_t truth = 0;

      switch (step->type)
      {
         case T_MATCH_VALUE:
            /*
             * T_MATCH_VALUE should short-circuit on false conditions
             */
            if (*p_candidate == step->data)
            {
               return cts_ssm_execute(step->next, p_candidate + 1, p_length_limit - 1);
            }
            else
            {
               return false;
            }
         case T_ADVANCE:
            return cts_ssm_execute(step->next, p_candidate + 1, p_length_limit - 1);
         case T_HALT:
            return true;
         default:
            return false;
      }
   }
}

void*
cts_ssm_search (void* p_program, uint8_t* p_candidate, size_t p_limit)
{
   if (p_program == NULL || p_candidate == NULL || p_limit == 0)
   {
      return NULL;
   }

   struct s_match_step* step = (struct s_match_step*) p_program;

   size_t const exec_len = cts_ssm_count_bytes(step);

   printf("program has execute read length of %lu bytes\n", exec_len);

   if (exec_len > p_limit)
   {
      return NULL;
   }

   for (size_t offset = 0; offset < p_limit; ++offset)
   {
      if (cts_ssm_execute(step, p_candidate + offset, p_limit - offset) == true)
      {
         return p_candidate + offset;
      }
   }

   return NULL;
}

// test with GTA core and current player struct pattern
int
main (void)
{
   struct s_rule* rule_wordlptr = cts_parse_rule("48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07");
   void* ssm_prog_worldptr = cts_ssm_compile(rule_wordlptr);
   cts_ssm_dump_program(ssm_prog_worldptr);

   // open gta coredump
   struct stat gta_core_stat;
   int fd_gta_core = open("test_core", O_RDONLY);
   fstat(fd_gta_core, &gta_core_stat);
   void* core_base_addr = mmap(NULL, gta_core_stat.st_size, PROT_READ, MAP_PRIVATE, fd_gta_core, 0);


   printf("search pattern from %p\n", core_base_addr);
   void* found_at = cts_ssm_search(ssm_prog_worldptr, core_base_addr, gta_core_stat.st_size);
   printf("search complete, result = %p\n", found_at);

   munmap(core_base_addr, gta_core_stat.st_size);

   uintptr_t offset_from_base = found_at - core_base_addr;

   printf("offset is 0x%X\n", offset_from_base);

   return 0;
}
