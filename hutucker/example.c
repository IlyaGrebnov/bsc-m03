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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hu-tucker.h"

int main() {
    size_t n;
    if (scanf("%zu", &n) != 1)
        return 1;
    if (n < 1) {
        errno = EINVAL;
        perror(NULL);
        return 1;
    }

    unsigned long *weight = calloc(n, sizeof(unsigned long));
    if (!weight) {
        perror("calloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++)
        scanf("%lu", weight + i);

    unsigned long sumweight = 0;
    for (size_t i = 0; i < n; i++) {
        sumweight += weight[i];
        if (sumweight < weight[i]) {
            errno = EOVERFLOW;
            perror(NULL);
            return 1;
        }
    }

    unsigned long *tmp = malloc(hutucker_tmp_size(n));
    if (!tmp) {
        perror("malloc");
        return 1;
    }
    hutucker_get_lengths(n, weight, tmp);
    free(tmp);

    unsigned long maxlength = 0;
    for (size_t i = 0; i < n; i++)
        if (weight[i] > maxlength)
            maxlength = weight[i];

    unsigned char *str = malloc(maxlength + 1);
    if (!str) {
        perror("malloc");
        return 1;
    }
    for (size_t i = 0, l = 0; i < n; i++) {
        if (l < weight[i])
            memset(str + l, '0', weight[i] - l);
        l = weight[i];
        str[l] = '\0';
        puts(str);
        for (size_t j = l - 1; j != (size_t) -1; j--)
            if ((str[j] ^= '0' ^ '1') == '1')
                break;
    }

    free(str);
    free(weight);
}
