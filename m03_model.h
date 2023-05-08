/*--

This file is a part of bsc-m03 project.

    Copyright (c) 2021-2023 Ilya Grebnov <ilya.grebnov@gmail.com>

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

enum m03_mode { encoding, decoding };

template <m03_mode mode> class m03_model
{
protected:
    void initialize_model(RangeCoder * coder)
    {
        this->coder = coder;

        this->memset_uint16(this->T1_model        , 1, sizeof(this->T1_model));

        this->memset_uint16(this->T2_model_m0     , 1, sizeof(this->T2_model_m0));
        this->memset_uint16(this->T2_model_m1     , 1, sizeof(this->T2_model_m1));

        this->memset_uint16(this->T3_model_m0     , 1, sizeof(this->T2_model_m0));
        this->memset_uint16(this->T3_model_m1     , 1, sizeof(this->T3_model_m1));
        this->memset_uint16(this->T3_model_m2     , 1, sizeof(this->T3_model_m2));

        this->memset_uint16(this->Tx_model_m0     , 1, sizeof(this->Tx_model_m0));
        this->memset_uint16(this->Tx_model_m1     , 1, sizeof(this->Tx_model_m1));
        this->memset_uint16(this->Tx_model_m2     , 1, sizeof(this->Tx_model_m2));
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

    int32_t predict(int32_t count, int32_t total, int32_t left_remaining, int32_t right_remaining, int32_t symbols_remaining, int32_t context)
    {
        int32_t inferred_right = std::max(total - left_remaining, 0); total -= inferred_right; right_remaining -= inferred_right << 1;

        assert(left_remaining > 0); assert(right_remaining > 0); assert(total <= left_remaining); assert(left_remaining <= right_remaining);

        if (total <= 3)
        {
            int32_t state = 0;
            state += 1 * (context);
            state += 32 * (std::min((int32_t)symbols_remaining - 2, 7));
            state += 256 * (std::min((int32_t)bit_scan_reverse(inferred_right + 1), 3));
            state += 1024 * (left_remaining + right_remaining + inferred_right == symbols_remaining);
            state += 2048 * (left_remaining == total);
            state += 4096 * (((int64_t)left_remaining * 11) / ((int64_t)right_remaining));

            if (total == 1)
            {
                ptrdiff_t bucket = m03_T1_model_m0_state_table[state];

                uint16_t * RESTRICT predictor = &this->T1_model[bucket][0];
                if (predictor[0] + predictor[1] > m03_T1_model_m0_scale_table[bucket])
                {
                    predictor[0] = (predictor[0] + 1) >> 1;
                    predictor[1] = (predictor[1] + 1) >> 1;
                }

                if (mode == m03_mode::encoding)
                {
                    this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                }
                else
                {
                    count = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                    this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                }

                predictor[count]++;
            }
            else if (total == 2)
            {
                int32_t pivot = (count == 0) | (count == 2);

                {
                    ptrdiff_t bucket = m03_T2_model_m0_state_table[state];

                    uint16_t * RESTRICT predictor = &this->T2_model_m0[bucket][0];
                    if (predictor[0] + predictor[1] > m03_T2_model_m0_scale_table[bucket])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                    }

                    if (mode == m03_mode::encoding)
                    {
                        this->coder->Encode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                    }
                    else
                    {
                        pivot = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                        this->coder->Decode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                    }

                    predictor[pivot]++;
                }

                if (pivot)
                {
                    count = count > 0;

                    {
                        ptrdiff_t bucket = m03_T2_model_m1_state_table[state];

                        uint16_t * RESTRICT predictor = &this->T2_model_m1[bucket][0];
                        if (predictor[0] + predictor[1] > m03_T2_model_m1_scale_table[bucket])
                        {
                            predictor[0] = (predictor[0] + 1) >> 1;
                            predictor[1] = (predictor[1] + 1) >> 1;
                        }

                        if (mode == m03_mode::encoding)
                        {
                            this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }
                        else
                        {
                            count = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                            this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }

                        predictor[count]++;
                    }

                    count = count ? total : count;
                }
                else
                {
                    count = 1;
                }
            }
            else
            {
                int32_t pivot = (count == 0) | (count == 3);

                {
                    ptrdiff_t bucket = m03_T3_model_m0_state_table[state];

                    uint16_t * RESTRICT predictor = &this->T3_model_m0[bucket][0];
                    if (predictor[0] + predictor[1] > m03_T3_model_m0_scale_table[bucket])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                    }

                    if (mode == m03_mode::encoding)
                    {
                        this->coder->Encode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                    }
                    else
                    {
                        pivot = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                        this->coder->Decode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                    }

                    predictor[pivot]++;
                }

                if (pivot)
                {
                    count = count > 0;

                    {
                        ptrdiff_t bucket = m03_T3_model_m1_state_table[state];

                        uint16_t * RESTRICT predictor = &this->T3_model_m1[bucket][0];
                        if (predictor[0] + predictor[1] > m03_T3_model_m1_scale_table[bucket])
                        {
                            predictor[0] = (predictor[0] + 1) >> 1;
                            predictor[1] = (predictor[1] + 1) >> 1;
                        }

                        if (mode == m03_mode::encoding)
                        {
                            this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }
                        else
                        {
                            count = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                            this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }

                        predictor[count]++;
                    }

                    count = count ? total : count;
                }
                else
                {
                    count = count - 1;

                    {
                        ptrdiff_t bucket = m03_T3_model_m2_state_table[state];

                        uint16_t * RESTRICT predictor = &this->T3_model_m2[bucket][0];
                        if (predictor[0] + predictor[1] > m03_T3_model_m2_scale_table[bucket])
                        {
                            predictor[0] = (predictor[0] + 1) >> 1;
                            predictor[1] = (predictor[1] + 1) >> 1;
                        }

                        if (mode == m03_mode::encoding)
                        {
                            this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }
                        else
                        {
                            count = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                            this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                        }

                        predictor[count]++;
                    }

                    count = count + 1;
                }
            }
        }
        else
        {
            int32_t state = 0;
            state += 1 * (std::min((int32_t)bit_scan_reverse(total - 3), 7));
            state += 8 * (context);
            state += 256 * (std::min((int32_t)bit_scan_reverse(symbols_remaining - 1), 3));
            state += 1024 * (left_remaining == total);
            state += 2048 * (inferred_right > 0);
            state += 4096 * (((int64_t)left_remaining * 11) / ((int64_t)right_remaining));

            int32_t pivot = (count == 0) | (count == total);

            {
                ptrdiff_t bucket = m03_Tx_model_m0_state_table[state];

                uint16_t * RESTRICT predictor = &this->Tx_model_m0[bucket][0];
                if (predictor[0] + predictor[1] > m03_Tx_model_m0_scale_table[bucket])
                {
                    predictor[0] = (predictor[0] + 1) >> 1;
                    predictor[1] = (predictor[1] + 1) >> 1;
                }

                if (mode == m03_mode::encoding)
                {
                    this->coder->Encode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                }
                else
                {
                    pivot = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                    this->coder->Decode(pivot ? predictor[0] : 0, predictor[pivot], predictor[0] + predictor[1]);
                }

                predictor[pivot]++;
            }

            if (pivot)
            {
                count = count > 0;

                {
                    ptrdiff_t bucket = m03_Tx_model_m1_state_table[state];

                    uint16_t * RESTRICT predictor = &this->Tx_model_m1[bucket][0];
                    if (predictor[0] + predictor[1] > m03_Tx_model_m1_scale_table[bucket])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                    }

                    if (mode == m03_mode::encoding)
                    {
                        this->coder->Encode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                    }
                    else
                    {
                        count = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                        this->coder->Decode(count ? predictor[0] : 0, predictor[count], predictor[0] + predictor[1]);
                    }

                    predictor[count]++;
                }

                count = count ? total : count;
            }
            else
            {
                state = 0;
                state += 1 * (std::min(total - 4, 15));
                state += 16 * (context & 3);
                state += 64 * (((int64_t)left_remaining * 7) / ((int64_t)right_remaining));
                state += 512 * (std::min((int32_t)bit_scan_reverse(symbols_remaining - 1), 3));
                state += 2048 * (inferred_right >= total);

                int32_t min = 1, max = total - 1; context = 1;
                while (min != max && context < 16)
                {
                    ptrdiff_t bucket = m03_Tx_model_m2_state_table[state * 16 + context];

                    uint16_t * RESTRICT predictor = &this->Tx_model_m2[bucket][0];
                    if (predictor[0] + predictor[1] > m03_Tx_model_m2_scale_table[bucket])
                    {
                        predictor[0] = (predictor[0] + 1) >> 1;
                        predictor[1] = (predictor[1] + 1) >> 1;
                    }

                    int32_t median = min + ((max - min + 1) >> 1), bit = count >= median;

                    if (mode == m03_mode::encoding)
                    {
                        this->coder->Encode(bit ? predictor[0] : 0, predictor[bit], predictor[0] + predictor[1]);
                    }
                    else
                    {
                        bit = this->coder->GetCumFreq(predictor[0] + predictor[1]) >= predictor[0];
                        this->coder->Decode(bit ? predictor[0] : 0, predictor[bit], predictor[0] + predictor[1]);
                    }

                    predictor[bit]++; context += context + bit; min = bit ? median : min; max = bit ? max : median - 1;
                }

                count = mode == m03_mode::encoding
                    ? this->coder->EncodeValue(min, count, max)
                    : this->coder->DecodeValue(min, max);
            }
        }

        return count;
    }
    
private:
    RangeCoder *    coder;

    uint16_t        T1_model[128][2];

    uint16_t        T2_model_m0[48][2];
    uint16_t        T2_model_m1[80][2];

    uint16_t        T3_model_m0[48][2];
    uint16_t        T3_model_m1[64][2];
    uint16_t        T3_model_m2[32][2];

    uint16_t        Tx_model_m0[64][2];
    uint16_t        Tx_model_m1[80][2];
    uint16_t        Tx_model_m2[256][2];

    void memset_uint16(void * RESTRICT dst, uint16_t v, size_t size)
    {
        for (size_t i = 0; i < size / 2; ++i) { ((uint16_t *)dst)[i] = v; }
    }
};

#pragma warning( pop )
