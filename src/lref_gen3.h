/*
 * lref_gen3.h
 *
 *  Created on: Dec 5, 2013
 *      Author: reboot
 */

#ifndef LREF_GEN3_H_
#define LREF_GEN3_H_

#include <fp_types.h>

#include <stdint.h>
#include <inttypes.h>

__d_format_block zone_format_block, zone_format_block_batch,
    zone_format_block_exp;

__d_ref_to_pval ref_to_val_ptr_gen3;

_d_rtv_lk ref_to_val_lk_gen3;

int
gcb_gen3 (void *buffer, char *key, char *val);

#pragma pack(push, 4)

typedef struct ___d_generic_s800
{
  int32_t i32_1;
  int32_t i32_2;
  uint32_t ui32_1;
  uint32_t ui32_2;
  uint64_t ui64_1;
  uint64_t ui64_2;
  char s_1[255];
  char s_2[255];
  char s_3[128];
  char s_4[128];
} _d_generic_s800, *__d_generic_s800;

#pragma pack(pop)

#define G3_SZ                           sizeof(_d_generic_s800)

#endif /* LREF_GEN3_H_ */
