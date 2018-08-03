#pragma once

#include <ctinysignature/pattern.h>

/**
 * Compile a program for the SSM matcher
 */
void* cts_ssm_compile(struct s_rule*);

/**
 * Delete a program for the SSM matcher
 */
void cts_ssm_delete_program(void*);

/**
 * Approximate the maximum number of bytes that a program will read
 */
size_t cts_ssm_count_bytes(void*);

/**
 * Evaluate #0 against #1 at +0 with read length limit of #2
 */
uint8_t cts_ssm_execute(void*, uint8_t*, size_t*);

