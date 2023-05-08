/*
 * Linearithmic Hu-Tucker Coding.
 * Copyright (C) 2018  Pochang Chen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*--

NOTICE: This file has been modified for use in the bsc-m03 project.

--*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * This algorithm needs some temporary memories to work.
 * This function computes how much temporary memories are needed.
 */
size_t hutucker_tmp_size(size_t n);

/**
 * Given the weight of n symbols, determine the length of hu-tucker code
 * of each symbols.
 *
 * Precondition: 
 * n: number of symbols
 * weight[i] (0 <= i < n): the weight of symbol i
 * weight[0] + ... + weight[n - 1] must not exceed UINT_MAX
 * tmp: buffer with size >= hutucker_tmp_size(n)
 *
 * Postcondition:
 * weight[i] (0 <= i < n): the length of hu-tucker code of symbol i
 */
void hutucker_get_lengths(size_t n, unsigned int *weight, void *tmp);

#ifdef __cplusplus
}
#endif
