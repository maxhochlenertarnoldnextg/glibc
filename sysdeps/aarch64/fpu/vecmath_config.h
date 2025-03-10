/* Configuration for libmvec routines.
   Copyright (C) 2023-2024 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _VECMATH_CONFIG_H
#define _VECMATH_CONFIG_H

#include <math_private.h>

/* Deprecated config option from Arm Optimized Routines which ensures
   fp exceptions are correctly triggered. This is not intended to be
   supported in GLIBC, however we keep it for ease of development.  */
#define WANT_SIMD_EXCEPT 0

/* Return ptr but hide its value from the compiler so accesses through it
   cannot be optimized based on the contents.  */
#define ptr_barrier(ptr)                                                      \
  ({                                                                          \
    __typeof (ptr) __ptr = (ptr);                                             \
    __asm("" : "+r"(__ptr));                                                  \
    __ptr;                                                                    \
  })

static inline uint64_t
asuint64 (double f)
{
  union
  {
    double f;
    uint64_t i;
  } u = { f };
  return u.i;
}

#define V_LOG_POLY_ORDER 6
#define V_LOG_TABLE_BITS 7
extern const struct v_log_data
{
  /* Shared data for vector log and log-derived routines (e.g. asinh).  */
  double poly[V_LOG_POLY_ORDER - 1];
  double ln2;
  struct
  {
    double invc, logc;
  } table[1 << V_LOG_TABLE_BITS];
} __v_log_data attribute_hidden;

#define V_EXP_TAIL_TABLE_BITS 8
extern const uint64_t __v_exp_tail_data[1 << V_EXP_TAIL_TABLE_BITS] attribute_hidden;
#define V_EXP_TABLE_BITS 7
extern const uint64_t __v_exp_data[1 << V_EXP_TABLE_BITS] attribute_hidden;

#define V_LOG2_TABLE_BITS 7
extern const struct v_log2_data
{
  double poly[5];
  double invln2;
  struct
  {
    double invc, log2c;
  } table[1 << V_LOG2_TABLE_BITS];
} __v_log2_data attribute_hidden;

#define V_LOG10_TABLE_BITS 7
extern const struct v_log10_data
{
  double poly[5];
  double invln10, log10_2;
  struct
  {
    double invc, log10c;
  } table[1 << V_LOG10_TABLE_BITS];
} __v_log10_data attribute_hidden;

extern const struct erff_data
{
  struct
  {
    float erf, scale;
  } tab[513];
} __erff_data attribute_hidden;

extern const struct sv_erff_data
{
  float erf[513];
  float scale[513];
} __sv_erff_data attribute_hidden;

extern const struct erf_data
{
  struct
  {
    double erf, scale;
  } tab[769];
} __erf_data attribute_hidden;

extern const struct sv_erf_data
{
  double erf[769];
  double scale[769];
} __sv_erf_data attribute_hidden;

extern const struct erfc_data
{
  struct
  {
    double erfc, scale;
  } tab[3488];
} __erfc_data attribute_hidden;

extern const struct erfcf_data
{
  struct
  {
    float erfc, scale;
  } tab[645];
} __erfcf_data attribute_hidden;

#endif
