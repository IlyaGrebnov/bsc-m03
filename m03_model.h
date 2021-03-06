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

#include "m03_tables.h"

#pragma warning( push )
#pragma warning( disable : 6385 )
#pragma warning( disable : 6386 )

#define SYMBOL_HISTORY_MAX_DEPTH (16)

enum class m03_mode : int { encoding = 0, decoding = 1, };

class m03_model
{
protected:
    m03_mode mode;

    void initialize_model(RangeCoder * coder, m03_mode mode)
    {
        this->coder = coder;
        this->mode  = mode;

        this->memset_uint16(this->T1_model        , 1, sizeof(this->T1_model));
        this->memset_uint16(this->T2_model        , 1, sizeof(this->T2_model));
        this->memset_uint16(this->Ternary_model   , 1, sizeof(this->Ternary_model));
        this->memset_uint16(this->Tree_model      , 1, sizeof(this->Tree_model));
    }

    void encode_root_frequencies(const int32_t * root_frequencies, int32_t k, int32_t n)
    {
        int64_t  bit_freq[33];
        int64_t  bit_freq_sum[33];

        {
            int64_t remaining_min = n, remaining_max = n, remaining_count = k;

            memset(bit_freq, 0, sizeof(bit_freq));

            for (ptrdiff_t p = 0; p < k; ++p)
            {
                bit_freq[bit_scan_reverse(root_frequencies[p] + 1)]++;
            }

            for (ptrdiff_t bit = 0; bit <= 32 && remaining_count > 0; ++bit)
            {
                int64_t min_value   = (1ll << (bit + 0)) - 1;
                int64_t max_value   = (1ll << (bit + 1)) - 2;
                int64_t min         = std::max(remaining_count - (remaining_max / (max_value + 1)), (int64_t)0);
                int64_t max         = remaining_count * max_value < remaining_min ? remaining_count - 1 : remaining_count;

                this->coder->EncodeValue((unsigned int)min, (unsigned int)bit_freq[bit], (unsigned int)max);

                remaining_min       -= bit_freq[bit] * max_value;
                remaining_max       -= bit_freq[bit] * min_value;
                remaining_count     -= bit_freq[bit];
            }
        }

        {
            int64_t bit_sum = 0, remaining_min = 0, remaining_max = 0, remaining_total = n;

            for (ptrdiff_t bit = 32; bit >= 0; --bit)
            {
                int64_t min_value   = (1ll << (bit + 0)) - 1;
                int64_t max_value   = (1ll << (bit + 1)) - 2;

                bit_freq_sum[bit] = bit_sum; bit_sum += bit_freq[bit];

                remaining_min       += min_value * bit_freq[bit];
                remaining_max       += max_value * bit_freq[bit];
            }

            for (ptrdiff_t p = 0; p < k; ++p)
            {
                int32_t bit = bit_scan_reverse(root_frequencies[p] + 1);

                {
                    for (ptrdiff_t b = 0; b < bit; ++b)
                    {
                        if (bit_freq[b] > 0)
                        {
                            this->coder->Encode((unsigned int)bit_freq[b], (unsigned int)bit_freq_sum[b], (unsigned int)(bit_freq[b] + bit_freq_sum[b]));
                        }

                        assert(bit_freq_sum[b] > 0); bit_freq_sum[b]--;
                    }

                    if (bit_freq_sum[bit] > 0)
                    {
                        this->coder->Encode(0, (unsigned int)bit_freq[bit], (unsigned int)(bit_freq[bit] + bit_freq_sum[bit]));
                    }

                    assert(bit_freq[bit] > 0); bit_freq[bit]--;
                }

                {
                    int64_t min_value   = (1ll << (bit + 0)) - 1;
                    int64_t max_value   = (1ll << (bit + 1)) - 2;
                    remaining_min       -= min_value;
                    remaining_max       -= max_value;
                    int64_t min         = std::max(min_value, remaining_total - remaining_max);
                    int64_t max         = std::min(max_value, remaining_total - remaining_min);

                    this->coder->EncodeValue((unsigned int)min, (unsigned int)root_frequencies[p], (unsigned int)max);

                    remaining_total     -= root_frequencies[p];
                }
            }
        }
    }

    void decode_root_frequencies(int32_t * root_frequencies, int32_t k, int32_t n)
    {
        int64_t  bit_freq[33];
        int64_t  bit_freq_sum[33];

        {
            int64_t remaining_min = n, remaining_max = n, remaining_count = k;

            memset(bit_freq, 0, sizeof(bit_freq));

            for (ptrdiff_t bit = 0; bit <= 32 && remaining_count > 0; ++bit)
            {
                int64_t min_value   = (1ll << (bit + 0)) - 1;
                int64_t max_value   = (1ll << (bit + 1)) - 2;
                int64_t min         = std::max(remaining_count - (remaining_max / (max_value + 1)), (int64_t)0);
                int64_t max         = remaining_count * max_value < remaining_min ? remaining_count - 1 : remaining_count;

                bit_freq[bit]       = this->coder->DecodeValue((unsigned int)min, (unsigned int)max);

                remaining_min       -= bit_freq[bit] * max_value;
                remaining_max       -= bit_freq[bit] * min_value;
                remaining_count     -= bit_freq[bit];
            }
        }

        {
            int64_t bit_sum = 0, remaining_min = 0, remaining_max = 0, remaining_total = n;

            for (ptrdiff_t bit = 32; bit >= 0; --bit)
            {
                int64_t min_value   = (1ll << (bit + 0)) - 1;
                int64_t max_value   = (1ll << (bit + 1)) - 2;

                bit_freq_sum[bit] = bit_sum; bit_sum += bit_freq[bit];

                remaining_min       += min_value * bit_freq[bit];
                remaining_max       += max_value * bit_freq[bit];
            }

            for (ptrdiff_t p = 0; p < k; ++p)
            {
                int32_t bit = 0;

                while (bit_freq_sum[bit] > 0)
                {
                    if (bit_freq[bit] > 0)
                    {
                        unsigned int cum_freq = this->coder->GetCumFreq((unsigned int)(bit_freq[bit] + bit_freq_sum[bit]));
                        if (cum_freq < bit_freq[bit])
                        {
                            this->coder->Decode(0, (unsigned int)bit_freq[bit], (unsigned int)(bit_freq[bit] + bit_freq_sum[bit]));
                            break;
                        }
                        else
                        {
                            this->coder->Decode((unsigned int)bit_freq[bit], (unsigned int)bit_freq_sum[bit], (unsigned int)(bit_freq[bit] + bit_freq_sum[bit]));;
                        }
                    }

                    bit_freq_sum[bit]--; bit++;
                }

                assert(bit_freq[bit] > 0); bit_freq[bit]--;

                {
                    int64_t min_value   = (1ll << (bit + 0)) - 1;
                    int64_t max_value   = (1ll << (bit + 1)) - 2;
                    remaining_min       -= min_value;
                    remaining_max       -= max_value;
                    int64_t min         = std::max(min_value, remaining_total - remaining_max);
                    int64_t max         = std::min(max_value, remaining_total - remaining_min);

                    root_frequencies[p] = this->coder->DecodeValue((unsigned int)min, (unsigned int)max);

                    remaining_total     -= root_frequencies[p];
                }
            }
        }
    }

    int32_t predict(int32_t count, int32_t total, int32_t left_remaining, int32_t right_remaining, int32_t symbols_remaining, int32_t symbol, int32_t level, int32_t leaf_node)
    {
        level = std::min(level, SYMBOL_HISTORY_MAX_DEPTH - 1); this->Symbol_history[symbol][level] = left_remaining == 0;

        int32_t inferred_right = std::max(total - left_remaining, 0); total -= inferred_right;

        if (total > 0)
        {
            right_remaining -= inferred_right << 1;

            assert(right_remaining > 0); assert(total <= left_remaining); assert(left_remaining <= right_remaining);

            int32_t pivot_history =
                level > 1 ? this->Symbol_history[symbol][level - 1] | this->Symbol_history[symbol][level - 2] :
                level > 0 ? this->Symbol_history[symbol][level - 1] : 0;

            if (total <= 2)
            {
                int32_t state = 0;
                state += 1   * (left_remaining + right_remaining + inferred_right == symbols_remaining);
                state += 2   * (left_remaining == total);
                state += 4   * (pivot_history);
                state += 8   * (leaf_node);
                state += 16  * (std::min((int32_t)bit_scan_reverse(inferred_right + 1), 3));
                state += 64  * (std::min((int32_t)symbols_remaining - 2, 7));
                state += 512 * (((int64_t)left_remaining * 11) / ((int64_t)right_remaining));

                if (total == 1)
                {
                    state = m03_T1_model_state_table[state];

                    uint16_t * RESTRICT predictor = &this->T1_model[state][0];

                    if (predictor[0] + predictor[1] > m03_T1_model_scale_table[state])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                    }

                    if (this->mode == m03_mode::encoding)
                    {
                        this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                    }
                    else
                    {
                        unsigned int cum_freq = this->coder->GetCumFreq(predictor[0] + predictor[1]);

                        count = cum_freq >= predictor[0];
                        this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                    }

                    predictor[count]++; this->Symbol_history[symbol][level] = 1;
                }
                else
                {
                    state = m03_T2_model_state_table[state];

                    uint16_t * RESTRICT predictor = &this->T2_model[state][0];

                    if (predictor[0] + predictor[1] + predictor[2] > m03_T2_model_scale_table[state])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                        predictor[2] = (predictor[2] + 1) >> 1;
                    }

                    if (this->mode == m03_mode::encoding)
                    {
                        unsigned int cum_freq = count == 0 ? 0 : count == 1 ? predictor[0] : predictor[0] + predictor[1];
                        this->coder->Encode(cum_freq, predictor[count], predictor[0] + predictor[1] + predictor[2]);
                    }
                    else
                    {
                        unsigned int cum_freq = this->coder->GetCumFreq(predictor[0] + predictor[1] + predictor[2]);
                    
                        count       = (cum_freq >= predictor[0]) + (cum_freq >= (unsigned int)(predictor[0] + predictor[1]));
                        cum_freq    = count == 0 ? 0 : count == 1 ? predictor[0] : predictor[0] + predictor[1];

                        this->coder->Decode(cum_freq, predictor[count], predictor[0] + predictor[1] + predictor[2]);
                    }

                    predictor[count]++; this->Symbol_history[symbol][level] = count != 1;
                }
            }
            else
            {
                int32_t pivot = (count > 0) + (count == total);

                {
                    int32_t state = 0;
                    state += 1   * (std::min((int32_t)bit_scan_reverse(symbols_remaining - 1), 3));
                    state += 4   * (inferred_right > 0);
                    state += 8   * (left_remaining == total);
                    state += 16  * (leaf_node);
                    state += 32  * (pivot_history);
                    state += 64  * (std::min((int32_t)bit_scan_reverse(total - 2), 7));
                    state += 512 * (((int64_t)left_remaining * 11) / ((int64_t)right_remaining));
                       
                    state = m03_Ternary_model_state_table[state];

                    uint16_t * RESTRICT predictor = &this->Ternary_model[state][0];

                    if (predictor[0] + predictor[1] + predictor[2] > m03_Ternary_model_scale_table[state])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                        predictor[2] = (predictor[2] + 1) >> 1;
                    }

                    if (this->mode == m03_mode::encoding)
                    {
                        unsigned int cum_freq = pivot == 0 ? 0 : pivot == 1 ? predictor[0] : predictor[0] + predictor[1];
                        this->coder->Encode(cum_freq, predictor[pivot], predictor[0] + predictor[1] + predictor[2]);
                    }
                    else
                    {
                        unsigned int cum_freq = this->coder->GetCumFreq(predictor[0] + predictor[1] + predictor[2]);

                        pivot = (cum_freq >= predictor[0]) + (cum_freq >= (unsigned int)(predictor[0] + predictor[1]));
                        cum_freq = pivot == 0 ? 0 : pivot == 1 ? predictor[0] : predictor[0] + predictor[1];

                        this->coder->Decode(cum_freq, predictor[pivot], predictor[0] + predictor[1] + predictor[2]);
                    }

                    predictor[pivot]++; this->Symbol_history[symbol][level] = pivot != 1; if (pivot != 1) { count = pivot == 0 ? 0 : total; }
                }

                if (pivot == 1)
                {
                    int32_t state = 0;
                    state += 1 * (inferred_right >= total);
                    state += 2 * (pivot_history);
                    state += 4  * (std::min(total - 3, 15));
                    state += 64 * (((int64_t)left_remaining * 5) / ((int64_t)right_remaining));

                    int32_t min = 1, max = total - 1, context = 1;
                    while (min != max && context < 16)
                    {
                        ptrdiff_t bucket = m03_Tree_model_state_table[state * 16 + context];

                        uint16_t * RESTRICT predictor = &this->Tree_model[bucket][0];

                        if (predictor[0] + predictor[1] > m03_Tree_model_scale_table[bucket])
                        {
                            predictor[0] = (predictor[0] + 1) >> 1;
                            predictor[1] = (predictor[1] + 1) >> 1;
                        }

                        int32_t median = min + ((max - min + 1) >> 1), bit = count >= median;

                        if (this->mode == m03_mode::encoding)
                        {
                            this->coder->Encode(bit ? predictor[0] : 0, predictor[bit], predictor[0] + predictor[1]);
                        }
                        else
                        {
                            unsigned int cum_freq = this->coder->GetCumFreq(predictor[0] + predictor[1]);

                            bit = cum_freq >= predictor[0];
                            this->coder->Decode(bit ? predictor[0] : 0, predictor[bit], predictor[0] + predictor[1]);
                        }

                        predictor[bit]++; context += context + bit; min = bit ? median : min; max = bit ? max : median - 1;
                    }

                    count = this->mode == m03_mode::encoding
                        ? this->coder->EncodeValue(min, count, max)
                        : this->coder->DecodeValue(min, max);
                }
            }

            return count;
        }

        return 0;
    }
    
private:
    RangeCoder *    coder;

    uint16_t        T1_model[96][2];
    uint16_t        T2_model[96][4];
    uint16_t        Ternary_model[192][4];
    uint16_t        Tree_model[256][2];

    uint8_t         Symbol_history[MAX_ALPHABET_SIZE][SYMBOL_HISTORY_MAX_DEPTH];

    void memset_uint16(void * RESTRICT dst, uint16_t v, size_t size)
    {
        for (size_t i = 0; i < size / 2; ++i) { ((uint16_t *)dst)[i] = v; }
    }
};

#pragma warning( pop )
