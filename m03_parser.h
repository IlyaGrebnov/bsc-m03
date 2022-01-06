/*--

This file is a part of bsc-m03 project.

    Copyright (c) 2021-2022 Ilya Grebnov <ilya.grebnov@gmail.com>

    bsc-m03 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    bsc-m03 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with bsc-m03.  If not, see <https://www.gnu.org/licenses/>.

--*/

#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include "common/platform.h"
#include "common/rangecoder.h"

#include "hutucker/hu-tucker.h"

#include "m03_model.h"

#define OPTIMAL_ABT_SMALL_THRESHOLD   (7)
#define OPTIMAL_ABT_LARGE_THRESHOLD   (257)

#pragma warning( push )
#pragma warning( disable : 6385 )
#pragma warning( disable : 6386 )

typedef struct offset_queue
{
    int32_t *   offsets;
    ptrdiff_t   count;
    ptrdiff_t   size;

    bool initialize(ptrdiff_t size)
    {
        this->count     = 0;
        this->size      = size;
        this->offsets   = (int32_t *)malloc(this->size * sizeof(int32_t));

        return this->offsets != NULL;
    }

    INLINE void push_offset(const int32_t offset)
    {
        if (this->count == this->size)
        {
            this->offsets = this->resize();
        }

        this->offsets[this->count++] = offset;
    }

    INLINE void reset() { this->count = 0; }

    INLINE void sort() { std::stable_sort(this->offsets, this->offsets + this->count); }

    NOINLINE int32_t * resize()
    {
        return (int32_t *)realloc(this->offsets, (this->size += this->size) * sizeof(int32_t));
    }

    void destroy()
    {
        if (this->offsets != NULL) { free(this->offsets); this->offsets = NULL; }
    }

} offset_queue;

template <class symbol_t>
class m03_parser: m03_model
{
public:

    bool initialize(symbol_t * L, int32_t n, int32_t primary_index, int32_t * root_frequencies, int32_t k, RangeCoder * coder, m03_mode mode)
    {
        memset(this, 0, sizeof(m03_parser));

        this->L                 = L;
        this->n                 = n;
        this->primary_index     = primary_index;
        this->root_frequencies  = root_frequencies;
        this->k                 = k;

        if ((this->contexts = (symbol_context *)malloc(n * sizeof(symbol_context))) == NULL)
        {
            this->destroy();
            return false;
        }

        if ((this->hutucker_tmp = malloc(hutucker_tmp_size(MAX_ALPHABET_SIZE + 1))) == NULL)
        {
            this->destroy();
            return false;
        }

        if (!current_segments.initialize(next_power_of_2(std::max(n / 4, 64))))
        {
            this->destroy();
            return false;
        }

        if (!next_segments.initialize(next_power_of_2(std::max(n / 4, 64))))
        {
            this->destroy();
            return false;
        }

        this->initialize_model(coder, mode);
        this->initialize_alphabetic_tree_roots();

        return true;
    }

    void run()
    {
        if (this->mode == m03_mode::encoding)
        {
            this->encode_root_frequencies(this->root_frequencies, this->k, this->n - 1);
            this->initialize_root_context(this->root_frequencies);
            this->parse_contexts();

            for (ptrdiff_t p = 0; p < n; ++p)
            {
                assert(p == this->primary_index || this->contexts[p].count  == 1   );
                assert(p == this->primary_index || this->contexts[p].symbol == L[p]);
            }
        }
        else
        {
            this->decode_root_frequencies(this->root_frequencies, this->k, this->n - 1);
            this->initialize_root_context(this->root_frequencies);
            this->parse_contexts();

            for (ptrdiff_t p = 0; p < n; ++p)
            {
                L[p] = this->contexts[p].symbol;
            }
        }
    }

    void destroy()
    {
        if (this->contexts      != NULL) { free(this->contexts);        this->contexts      = NULL; }
        if (this->hutucker_tmp  != NULL) { free(this->hutucker_tmp);    this->hutucker_tmp  = NULL; }

        this->current_segments.destroy();
        this->next_segments.destroy();
    }

private:

#pragma pack(push, 1)

    typedef struct symbol_context
    {
        int32_t     count;
        int32_t     offset;
        symbol_t    symbol;
    } symbol_context;

#pragma pack(pop)

    symbol_t *          L;
    int32_t             n;
    int32_t             primary_index;
    int32_t *           root_frequencies;
    int32_t             k;

    symbol_context *    contexts;
    offset_queue        current_segments;
    offset_queue        next_segments;
    void *              hutucker_tmp;

    int32_t             parent_frequencies  [MAX_ALPHABET_SIZE + 1];
    int32_t             left_frequencies    [MAX_ALPHABET_SIZE + 1];
    symbol_context      left_contexts       [MAX_ALPHABET_SIZE + 1];

    int32_t             alphabetic_tree_keys[OPTIMAL_ABT_LARGE_THRESHOLD];
    int32_t             alphabetic_tree_weight[OPTIMAL_ABT_LARGE_THRESHOLD];
    int64_t             alphabetic_tree_cost[OPTIMAL_ABT_LARGE_THRESHOLD][OPTIMAL_ABT_LARGE_THRESHOLD];
    uint8_t             alphabetic_tree_root[OPTIMAL_ABT_LARGE_THRESHOLD][OPTIMAL_ABT_LARGE_THRESHOLD];

    void initialize_alphabetic_tree_roots()
    {
        for (int32_t l = 0; l < OPTIMAL_ABT_LARGE_THRESHOLD - 1; ++l)
        {
            this->alphabetic_tree_root[l][l + 1] = this->alphabetic_tree_root[l][l] = l;
        }
    }

    void initialize_root_context(const int32_t * root_frequencies)
    {
        int32_t unique_symbols = 0, total_symbols = 1;

        this->current_segments.push_offset(0);

        for (int32_t c = 0; c < this->k; ++c)
        {
            if (root_frequencies[c] > 0)
            {
                this->contexts[unique_symbols].count  = root_frequencies[c];
                this->contexts[unique_symbols].offset = total_symbols;
                this->contexts[unique_symbols].symbol = c;

                this->current_segments.push_offset(total_symbols);
                
                unique_symbols++; total_symbols += root_frequencies[c];
            }
        }

        m03_parser::normalize_context(&this->contexts[0], unique_symbols, total_symbols);
    }

    void parse_contexts()
    {
        while (this->current_segments.count > 0)
        {
            for (int32_t segment_start = 0; segment_start < this->current_segments.count;)
            {
                int32_t context_start   = this->current_segments.offsets[segment_start];
                int32_t context_end     = context_start + this->contexts[context_start].count;
                int32_t segment_end     = segment_start + 1;

                while (segment_end < this->current_segments.count && this->current_segments.offsets[segment_end] < context_end)
                {
                    segment_end++;
                }

                assert(context_end - context_start > 1);
                assert(segment_end - segment_start > 1);

                if (this->is_trivial_context(context_start))
                {
                    m03_parser::split_trivial_context(this->contexts, this->next_segments, &this->current_segments.offsets[segment_start], &this->current_segments.offsets[segment_end]);
                }
                else
                {
                    m03_parser::populate_context_frequencies(&this->contexts[context_start], &this->contexts[this->primary_index], &this->parent_frequencies[0]);
                    this->split_context_recursive(&this->current_segments.offsets[segment_start], &this->current_segments.offsets[segment_end], 0);
                }

                segment_start = segment_end;
            }

            this->next_segments.sort();
            this->current_segments.reset();

            std::swap(this->current_segments, this->next_segments);
        }
    }

    void split_context_recursive(const int32_t * offsets, const int32_t * offsets_end, int32_t level)
    {
        assert(offsets_end - offsets > 0);

        if (offsets_end - offsets == 1)
        {
            m03_parser::populate_next_segments(&this->contexts[offsets[0]], &this->contexts[this->primary_index], &this->parent_frequencies[0], this->next_segments);
            return;
        }

        if (this->is_trivial_context(offsets[0]))
        {
            m03_parser::split_trivial_context(this->contexts, this->next_segments, offsets, offsets_end);
            return;
        }

        if (offsets_end - offsets >= OPTIMAL_ABT_SMALL_THRESHOLD && offsets_end - offsets <= OPTIMAL_ABT_LARGE_THRESHOLD)
        {
            this->build_optimal_alphabetic_tree(offsets, offsets_end);
            this->traverse_alphabetic_tree(offsets, offsets_end, 0, (int32_t)(offsets_end - offsets) - 1, level);
            return;
        }

        const int32_t * offsets_pivot = (offsets_end - offsets) > 2 
            ? this->choose_context_pivot_using_heuristic(offsets, offsets_end)
            : &offsets[1];

        this->split_context_by_pivot(offsets[0], offsets_pivot[0], level, (offsets_pivot - offsets) == 1, (offsets_end - offsets_pivot) == 1);
        this->split_context_recursive(offsets, offsets_pivot, level + 1);
        this->split_context_recursive(offsets_pivot, offsets_end, level + 1);
    }

    void traverse_alphabetic_tree(const int32_t * offsets, const int32_t * offsets_end, int32_t l, int32_t r, int32_t level)
    {
        assert(l <= r);

        if (r - l < OPTIMAL_ABT_SMALL_THRESHOLD - 1)
        {
            split_context_recursive(&offsets[l], &offsets[r + 1], level);
            return;
        }

        if (l == r)
        {
            m03_parser::populate_next_segments(&this->contexts[offsets[l]], &this->contexts[this->primary_index], &this->parent_frequencies[0], this->next_segments);
            return;
        }

        if (this->is_trivial_context(offsets[l]))
        {
            m03_parser::split_trivial_context(this->contexts, this->next_segments, &offsets[l], &offsets[r + 1]);
            return;
        }

        int32_t offsets_pivot = this->alphabetic_tree_root[l][r];

        this->split_context_by_pivot(offsets[l], offsets[offsets_pivot + 1], level, (offsets_pivot - l) == 0, (r - offsets_pivot) == 1);
        this->traverse_alphabetic_tree(offsets, offsets_end, l, offsets_pivot, level + 1);
        this->traverse_alphabetic_tree(offsets, offsets_end, offsets_pivot + 1, r, level + 1);
    }

    const int32_t * choose_context_pivot_using_heuristic(const int32_t * offsets, const int32_t * offsets_end)
    {
        assert(offsets_end - offsets > 2);

        int32_t context_begin = offsets[0];
        int32_t context_end   = offsets[0] + this->contexts[offsets[0]].count;
        size_t  offsets_count = offsets_end - offsets;

        if (offsets_count == 3)
        {
            int64_t A = 1 + (int64_t)(offsets[1] ) - (int64_t)(context_begin);
            int64_t C = 1 + (int64_t)(context_end) - (int64_t)(offsets[2]);

            return C <= A ? &offsets[1] : &offsets[2];
        }
        else if (offsets_count == 4)
        {
            int64_t A = 1 + (int64_t)(offsets[1] ) - (int64_t)(context_begin);
            int64_t B = 1 + (int64_t)(offsets[2] ) - (int64_t)(offsets[1]);
            int64_t C = 1 + (int64_t)(offsets[3] ) - (int64_t)(offsets[2]);
            int64_t D = 1 + (int64_t)(context_end) - (int64_t)(offsets[3]);

            const int32_t * offset1 = &offsets[1]; int64_t cost1 = pivot_cost3(B, C, D);
            const int32_t * offset2 = &offsets[2]; int64_t cost2 = A + B + C + D;
            const int32_t * offset3 = &offsets[3]; int64_t cost3 = pivot_cost3(A, B, C);

            if (cost2 <= cost1) { offset1 = offset2; cost1 = cost2; }
            if (cost3 <  cost1) { offset1 = offset3; }

            return offset1;
        }
        else if (offsets_count == 5)
        {
            int64_t A = 1 + (int64_t)(offsets[1] ) - (int64_t)(context_begin);
            int64_t B = 1 + (int64_t)(offsets[2] ) - (int64_t)(offsets[1]);
            int64_t C = 1 + (int64_t)(offsets[3] ) - (int64_t)(offsets[2]);
            int64_t D = 1 + (int64_t)(offsets[4] ) - (int64_t)(offsets[3]);
            int64_t E = 1 + (int64_t)(context_end) - (int64_t)(offsets[4]);

            const int32_t * offset1 = &offsets[1]; int64_t cost1 = pivot_cost4(B, C, D, E);
            const int32_t * offset2 = &offsets[2]; int64_t cost2 = A + B + pivot_cost3(C, D, E);
            const int32_t * offset3 = &offsets[3]; int64_t cost3 = pivot_cost3(A, B, C) + D + E;
            const int32_t * offset4 = &offsets[4]; int64_t cost4 = pivot_cost4(A, B, C, D);

            if (cost2 <= cost1) { offset1 = offset2; cost1 = cost2; }
            if (cost3 <  cost1) { offset1 = offset3; cost1 = cost3; }
            if (cost4 <  cost1) { offset1 = offset4; }

            return offset1;
        }
        else if (offsets_count == 6)
        {
            int64_t A = 1 + (int64_t)(offsets[1] ) - (int64_t)(context_begin);
            int64_t B = 1 + (int64_t)(offsets[2] ) - (int64_t)(offsets[1]);
            int64_t C = 1 + (int64_t)(offsets[3] ) - (int64_t)(offsets[2]);
            int64_t D = 1 + (int64_t)(offsets[4] ) - (int64_t)(offsets[3]);
            int64_t E = 1 + (int64_t)(offsets[5] ) - (int64_t)(offsets[4]);
            int64_t F = 1 + (int64_t)(context_end) - (int64_t)(offsets[5]);

            const int32_t * offset1 = &offsets[1]; int64_t cost1 = pivot_cost5(B, C, D, E, F);
            const int32_t * offset2 = &offsets[2]; int64_t cost2 = A + B + pivot_cost4(C, D, E, F);
            const int32_t * offset3 = &offsets[3]; int64_t cost3 = pivot_cost3(A, B, C) + pivot_cost3(D, E, F);
            const int32_t * offset4 = &offsets[4]; int64_t cost4 = pivot_cost4(A, B, C, D) + E + F;
            const int32_t * offset5 = &offsets[5]; int64_t cost5 = pivot_cost5(A, B, C, D, E);

            if (cost2 <= cost1) { offset1 = offset2; cost1 = cost2; }
            if (cost3 <= cost1) { offset1 = offset3; cost1 = cost3; }
            if (cost4 <  cost1) { offset1 = offset4; cost1 = cost4; }
            if (cost5 <  cost1) { offset1 = offset5; }

            return offset1;
        }
        else
        {
            assert(offsets_count > OPTIMAL_ABT_LARGE_THRESHOLD);

            {
                for (int32_t segment_end = context_end, offsets_index = (int32_t)offsets_count - 1; offsets_index >= 0; --offsets_index)
                {
                    int32_t segment_start = offsets[offsets_index];

                    this->left_frequencies[offsets_index] = 1 + segment_end - segment_start; segment_end = segment_start;
                }

                hutucker_get_lengths(offsets_count, (unsigned long *)this->left_frequencies, this->hutucker_tmp);
            }

            {
                uint8_t path[64] = { 0 };
                for (int32_t offsets_index = 0, length = 0; offsets_index < offsets_count; ++offsets_index)
                {
                    for (; length < this->left_frequencies[offsets_index]; ++length) { path[length] = 0; }

                    length = this->left_frequencies[offsets_index]; if (path[0] == 1) { return &offsets[offsets_index]; }

                    for (int32_t k = length - 1; k >= 0; --k) { if (path[k] ^= 1) { break; } }
                }
            }

            return NULL;
        }
    }

    void build_optimal_alphabetic_tree(const int32_t * offsets, const int32_t * offsets_end)
    {
        ptrdiff_t offsets_count = (ptrdiff_t)(offsets_end - offsets);

        assert(offsets_count >= OPTIMAL_ABT_SMALL_THRESHOLD && offsets_count <= OPTIMAL_ABT_LARGE_THRESHOLD);

        this->alphabetic_tree_keys[offsets_count - 1] = 1 + offsets[0] + this->contexts[offsets[0]].count - offsets[offsets_count - 1];

        for (ptrdiff_t offsets_index = offsets_count - 2; offsets_index >= 0; --offsets_index)
        {
            this->alphabetic_tree_keys[offsets_index] = 1 + offsets[offsets_index + 1] - offsets[offsets_index];
            this->alphabetic_tree_cost[offsets_index][offsets_index + 1] = this->alphabetic_tree_weight[offsets_index] = this->alphabetic_tree_keys[offsets_index] + this->alphabetic_tree_keys[offsets_index + 1];
        }

        for (ptrdiff_t length = 3; length <= offsets_count; ++length)
        {
            for (ptrdiff_t l = 0, r = length - 1; r < offsets_count; ++l, ++r)
            {
                uint8_t best_root = this->alphabetic_tree_root[l][r - 1];
                int64_t best_cost = this->alphabetic_tree_cost[l][best_root] + this->alphabetic_tree_cost[best_root + 1][r];

                for (ptrdiff_t root = (ptrdiff_t)best_root + 1; root <= (ptrdiff_t)this->alphabetic_tree_root[l + 1][r]; ++root)
                {
                    int64_t cost = this->alphabetic_tree_cost[l][root] + this->alphabetic_tree_cost[root + 1][r];
                    if (cost < best_cost) { best_cost = cost; best_root = (uint8_t)root; }
                }

                this->alphabetic_tree_weight[l] += this->alphabetic_tree_keys[r];
                this->alphabetic_tree_cost[l][r] = best_cost + this->alphabetic_tree_weight[l];
                this->alphabetic_tree_root[l][r] = best_root;
            }
        }
    }

    void split_context_by_pivot(int32_t parent_context_offset, int32_t right_context_offset, int32_t level, int32_t left_leaf, int32_t right_leaf)
    {
        symbol_context * parent_context = &this->contexts[parent_context_offset];
        int32_t parent_interval_size    = parent_context[0].count;
        int32_t parent_unique_symbols   = 1;

        symbol_context * left_context   = &this->left_contexts[0];
        int32_t * left_frequencies      = &this->left_frequencies[0];
        int32_t left_interval_size      = right_context_offset - parent_context_offset;
        int32_t left_unique_symbols     = 0;

        int32_t right_interval_size     = parent_interval_size - left_interval_size;
        int32_t right_unique_symbols    = 0;

        if (this->mode == m03_mode::encoding)
        {
            if (left_interval_size <= parent_interval_size - left_interval_size)
            {
                int32_t parent_total_symbols = parent_interval_size;

                parent_total_symbols -= ((uint32_t)(this->primary_index - parent_context_offset) < (uint32_t)parent_total_symbols);

                while (parent_total_symbols > 1 && parent_context[parent_unique_symbols].count > 0)
                {
                    parent_total_symbols -= parent_context[parent_unique_symbols].count;
                    left_frequencies[parent_context[parent_unique_symbols].symbol] = 0;
                    parent_unique_symbols++;
                }

                assert(parent_total_symbols > 0); parent_context[0].count = parent_total_symbols;
                left_frequencies[parent_context[0].symbol] = 0;

                for (int32_t p = parent_context_offset; p < right_context_offset; ++p) { left_frequencies[L[p]]++; }

                left_frequencies[0] -= ((uint32_t)(this->primary_index - parent_context_offset) < (uint32_t)left_interval_size);
            }
            else
            {
                int32_t parent_total_symbols = parent_interval_size;

                parent_total_symbols -= ((uint32_t)(this->primary_index - parent_context_offset) < (uint32_t)parent_total_symbols);

                while (parent_total_symbols > 1 && parent_context[parent_unique_symbols].count > 0)
                {
                    parent_total_symbols -= parent_context[parent_unique_symbols].count;
                    left_frequencies[parent_context[parent_unique_symbols].symbol] = parent_context[parent_unique_symbols].count;
                    parent_unique_symbols++;
                }

                assert(parent_total_symbols > 0); parent_context[0].count = parent_total_symbols;
                left_frequencies[parent_context[0].symbol] = parent_total_symbols;

                for (int32_t p = right_context_offset; p < parent_context_offset + parent_interval_size; ++p) { left_frequencies[L[p]]--; }

                left_frequencies[0] += ((uint32_t)(this->primary_index - right_context_offset) < (uint32_t)right_interval_size);
            }
        }
        else
        {
            int32_t parent_total_symbols = parent_interval_size;

            parent_total_symbols -= ((uint32_t)(this->primary_index - parent_context_offset) < (uint32_t)parent_total_symbols);

            while (parent_total_symbols > 1 && parent_context[parent_unique_symbols].count > 0)
            {
                parent_total_symbols -= parent_context[parent_unique_symbols].count;
                parent_unique_symbols++;
            }

            assert(parent_total_symbols > 0); parent_context[0].count = parent_total_symbols;
        }

        int32_t  left_remaining     = left_interval_size;
        int32_t  right_remaining    = right_interval_size;

        left_remaining  -= ((uint32_t)(this->primary_index - parent_context_offset) < (uint32_t)left_interval_size );
        right_remaining -= ((uint32_t)(this->primary_index - right_context_offset ) < (uint32_t)right_interval_size);

        for (int32_t parent_symbol_index = 0; parent_symbol_index < parent_unique_symbols; ++parent_symbol_index)
        {
            symbol_t symbol = parent_context[parent_symbol_index].symbol;
            int32_t  total  = parent_context[parent_symbol_index].count;
            int32_t  count  = left_frequencies[symbol];
                
            if (total <= left_remaining + right_remaining - total)
            {
                count = left_remaining <= right_remaining
                    ?         this->predict(        count, total, left_remaining , right_remaining, parent_unique_symbols - parent_symbol_index, symbol, level, left_leaf)
                    : total - this->predict(total - count, total, right_remaining, left_remaining , parent_unique_symbols - parent_symbol_index, symbol, level, right_leaf);
            }
            else
            {
                total = left_remaining + right_remaining - total;
                count = left_remaining - count;

                count = left_remaining <= right_remaining
                    ?         this->predict(        count, total, left_remaining , right_remaining, parent_unique_symbols - parent_symbol_index, symbol, level, right_leaf)
                    : total - this->predict(total - count, total, right_remaining, left_remaining , parent_unique_symbols - parent_symbol_index, symbol, level, left_leaf);

                count = left_remaining - count;
                total = left_remaining + right_remaining - total;
            }

            left_remaining  = left_remaining  - count;
            right_remaining = right_remaining + count - total;

            if (count > 0)
            {
                left_context[left_unique_symbols].count     = count;
                left_context[left_unique_symbols].offset    = parent_context[parent_symbol_index].offset;
                left_context[left_unique_symbols].symbol    = symbol;

                parent_context[parent_symbol_index].count   -= count;
                parent_context[parent_symbol_index].offset  += count;

                left_unique_symbols++;
            }

            if (parent_context[parent_symbol_index].count > 0)
            {
                parent_context[right_unique_symbols] = parent_context[parent_symbol_index];
                right_unique_symbols++;
            }
        }

        {
            memmove(&this->contexts[right_context_offset], &parent_context[0], right_unique_symbols * sizeof(symbol_context));
            m03_parser::normalize_context(&this->contexts[right_context_offset], right_unique_symbols, right_interval_size);

            memcpy(&parent_context[0], &left_context[0], left_unique_symbols * sizeof(symbol_context));
            m03_parser::normalize_context(&parent_context[0], left_unique_symbols, left_interval_size);
        }
    }

    INLINE bool is_trivial_context(int32_t context_start)
    {
        return this->contexts[context_start + 1].count == 0 && ((uint32_t)(this->primary_index - context_start) >= (uint32_t)this->contexts[context_start].count);
    }

    static void split_trivial_context(symbol_context * contexts, offset_queue & queue, const int32_t * offsets, const int32_t * offsets_end)
    {
        int32_t context_start           = *offsets++;
        symbol_context parent_context   = contexts[context_start];

        for (; offsets < offsets_end;)
        {
            symbol_context * context    = &contexts[context_start];
            int32_t context_end         = *offsets++;
            int32_t context_size        = context_end - context_start;

            queue.push_offset(parent_context.offset);

            context[0].count    = context_size;              parent_context.count  -= context_size;
            context[0].offset   = parent_context.offset;     parent_context.offset += context_size;
            context[0].symbol   = parent_context.symbol;     if (context_size > 1) { context[1].count = 0; }
           
            context_start       = context_end;
        }

        queue.push_offset(parent_context.offset);

        contexts[context_start] = parent_context; if (contexts[context_start].count > 1) { contexts[context_start + 1].count = 0; }
    }

    static void populate_context_frequencies(symbol_context * context, symbol_context * primary_index_context, int32_t * frequencies)
    {
        int32_t total_symbols   = context[0].count;
        int32_t unique_symbols  = 1;

        total_symbols -= ((uint32_t)(primary_index_context - context) < (uint32_t)total_symbols);

        while (total_symbols > 1 && context[unique_symbols].count > 0)
        {
            frequencies[context[unique_symbols].symbol] = context[unique_symbols].count;
            total_symbols -= context[unique_symbols].count; unique_symbols++;
        }

        assert(total_symbols > 0); frequencies[context[0].symbol] = total_symbols;
    }

    static void populate_next_segments(symbol_context * context, symbol_context * primary_index_context, int32_t * frequencies, offset_queue & queue)
    {
        int32_t total_symbols   = context[0].count;
        int32_t unique_symbols  = 1;

        total_symbols -= ((uint32_t)(primary_index_context - context) < (uint32_t)total_symbols);

        while (total_symbols > 1 && context[unique_symbols].count > 0)
        {
            if (frequencies[context[unique_symbols].symbol] != context[unique_symbols].count)
            {
                queue.push_offset(context[unique_symbols].offset);
            }

            total_symbols -= context[unique_symbols].count; unique_symbols++;
        }

        if (total_symbols > 0 && frequencies[context[0].symbol] != total_symbols)
        {
            queue.push_offset(context[0].offset);
        }
    }

    static void normalize_context(symbol_context * context, int32_t unique_symbols, int32_t total_symbols)
    {
        if (unique_symbols > 1)
        {
            for (int32_t i = 1; i < unique_symbols; ++i)
            {
                symbol_context temp = context[i];

                int32_t j = i;
                while (j > 0 && (context[j - 1].count < temp.count || (context[j - 1].count == temp.count && context[j - 1].symbol > temp.symbol)))
                {
                    context[j] = context[j - 1]; j--;
                }

                context[j] = temp;
            }

            {
                symbol_context * contexts_start  = &context[std::max(0, unique_symbols - 6)];
                symbol_context * contexts_end    = &context[unique_symbols - 1];

                while (contexts_start < contexts_end) { std::swap(*contexts_start++, *contexts_end--); }
            }
        }

        assert(total_symbols > 0); context[0].count = total_symbols; if (unique_symbols < total_symbols) { context[unique_symbols].count = 0; }
    }

    INLINE static int64_t pivot_cost3(int64_t A, int64_t B, int64_t C)
    {
        return A + B + C + B + std::min(A, C);
    }

    INLINE static int64_t pivot_cost4(int64_t A, int64_t B, int64_t C, int64_t D)
    {
        return A + B + C + D + std::min(A + B + C + D, std::min(pivot_cost3(A, B, C), pivot_cost3(B, C, D)));
    }

    INLINE static int64_t pivot_cost5(int64_t A, int64_t B, int64_t C, int64_t D, int64_t E)
    {
        return A + B + C + D + E + std::min(std::min(pivot_cost4(B, C, D, E), A + B + pivot_cost3(C, D, E)), std::min(pivot_cost3(A, B, C) + D + E, pivot_cost4(A, B, C, D)));
    }
};

#pragma warning( pop )
