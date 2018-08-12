#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


/**
 * Forward-linked search pattern info.
 * Contains a specific search pattern length, the pattern, and then optionally the amount of "free" data following before the next portion is evaluated
 */
struct s_rule {
   /**
    * Length of the pattern in bytes
    */
   size_t match_length;

   /**
    * Pattern itself, in contiguous memory compatible with memcmp
    */
   uint8_t* match_pattern;

   /**
    * Length in bytes of 'free' space to be skipped before stepping to the next portion of the pattern
    */
   size_t undefined_trailing_length;

   struct s_rule* next_pattern;
};

/**
 * Delete a rule from memory along with its pattern, and return the next rule
 */
static inline struct s_rule* cts_delete_rule(struct s_rule* spec) {
   if(spec != 0) {
      struct s_rule* next_retn = NULL;

      if(spec->next_pattern != 0) {
         next_retn = spec->next_pattern;
      }

      if(spec->match_pattern != 0) {
         free(spec->match_pattern);
      }

      free(spec);

      return next_retn;
   } else {
      return NULL;
   }
}

/**
 * Delete an entire rule chain
 */
static inline void cts_delete_rule_recursive(struct s_rule* head) {
   if(head == 0) {
      return;
   } else {
      cts_delete_rule_recursive(cts_delete_rule(head));
   }
}

/**
 * Create a new rule and allocate space for a pattern of <p_pattern_length>
 */
static inline struct s_rule* cts_new_rule(size_t p_pattern_length) {
   struct s_rule* rule = malloc(sizeof(*rule));

   rule->match_length              = p_pattern_length;
   rule->match_pattern             = malloc(sizeof(uint8_t) * p_pattern_length);
   rule->undefined_trailing_length = 0;
   rule->next_pattern              = NULL;

   return rule;

}

/**
 * An individual search result
 */
struct s_search_result {
   /**
    * Memory at start of search pattern
    */
   void* start_of_result;

   /**
    * Memory at end of search pattern
    */
   void* end_of_result;
};


struct s_rule* cts_parse_rule(char*);
