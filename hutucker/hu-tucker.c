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

#include "hu-tucker.h"

typedef struct {
    // number of (terminal or internal) nodes under this segnode
    size_t n;
    // number of terminal node under this segnode, if n >= 1
    size_t m;
    // index of minimum weight in the leftmost block, if n >= 1
    size_t l;
    // index of minimum weight in the rightmost block, if n >= 1
    size_t r;
    // indices of minimum weight pair in the same block, if n >= 2
    size_t i, j;
} segnode;

static void segupdate(segnode *pa, segnode *lc, segnode *rc, unsigned int *w) {
    if (!lc->n) {
        *pa = *rc;
        return;
    }
    if (!rc->n) {
        *pa = *lc;
        return;
    }
    pa->n = lc->n + rc->n;
    pa->m = lc->m + rc->m;
    pa->l = ( lc->m || w[lc->l] <= w[rc->l]) ? lc->l : rc->l;
    pa->r = (!rc->m && w[lc->r] <= w[rc->r]) ? lc->r : rc->r;
    pa->i = lc->r;
    pa->j = rc->l;
    if (lc->n >= 2 && w[lc->i] + w[lc->j] <= w[pa->i] + w[pa->j]) {
        pa->i = lc->i;
        pa->j = lc->j;
    }
    if (rc->n >= 2 && w[rc->i] + w[rc->j] < w[pa->i] + w[pa->j]) {
        pa->i = rc->i;
        pa->j = rc->j;
    }
}
static void segterminal(segnode *x, size_t id) {
    x->n = x->m = 1;
    x->l = x->r = id;
}
static void seginternal(segnode *x, size_t id) {
    x->n = 1;
    x->m = 0;
    x->l = x->r = id;
}
static void segnone(segnode *x) {
    x->n = 0;
}
static size_t raise_power_of_two(size_t n) {
    size_t ans = 1;
    while (ans < n)
        ans *= 2;
    return ans;
}

size_t hutucker_tmp_size(size_t n) {
    // TODO check overflow for very large n
    size_t m = raise_power_of_two(n);
    return sizeof(segnode) * (2 * m - 1) +
           sizeof(size_t) * (n + (2 * n - 1) + (2 * n - 1));
}

void hutucker_get_lengths(size_t n, unsigned int *weight, void *tmp) {
    size_t m = raise_power_of_two(n);
    segnode *seg = (segnode *) tmp;
    size_t *cur = (size_t *) (seg + 2 * m - 1);
    size_t *pa = (size_t *) (cur + n);
    size_t *level = (size_t *) (pa + 2 * n - 1);

    for (size_t i = 0; i < n; i++) {
        segterminal(seg + m - 1 + i, i);
        cur[i] = i;
    }
    for (size_t i = n; i < m; i++)
        segnone(seg + m - 1 + i);

    for (size_t i = m - 2; i != (size_t) -1; i--)
        segupdate(seg + i, seg + 2 * i + 1, seg + 2 * i + 2, weight);

    for (size_t k = 0; k < n - 1; k++) {
        size_t i = seg->i, j = seg->j;

        weight[i] += weight[j];
        pa[cur[i]] = pa[cur[j]] = n + k;
        cur[i] = n + k;

        seginternal(seg + m - 1 + i, i);
        for (size_t l = m + i; l /= 2; )
            segupdate(seg + l - 1, seg + 2 * l - 1, seg + 2 * l, weight);

        segnone(seg + m - 1 + j);
        for (size_t l = m + j; l /= 2; )
            segupdate(seg + l - 1, seg + 2 * l - 1, seg + 2 * l, weight);
    }

    level[2 * n - 2] = 0;
    for (size_t i = 2 * n - 3; i != (size_t) -1; i--)
        level[i] = level[pa[i]] + 1;
    for (size_t i = 0; i < n; i++)
        weight[i] = (unsigned int)level[i];
}
