/*-----------------------------------------------------------*/
/* Block Sorting, Lossless Data Compression Library.         */
/* Range coder                                               */
/*-----------------------------------------------------------*/

/*--

This file is a part of bsc and/or libbsc, a program and a library for
lossless, block-sorting data compression.

   Copyright (c) 2009-2021 Ilya Grebnov <ilya.grebnov@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

Please see the file LICENSE for full copyright information.

See also the bsc and libbsc web site:
  http://libbsc.com/ for more information.

--*/

/*--

NOTICE: This file has been modified for use in the bsc-m03 project.

--*/

#ifndef _LIBBSC_CODER_RANGECODER_H
#define _LIBBSC_CODER_RANGECODER_H

#include "platform.h"

class RangeCoder
{

private:

    union ari
    {
        struct u
        {
            unsigned int low32;
            unsigned int carry;
        } u;
        unsigned long long low;
    } ari;

    unsigned int ari_code;
    unsigned int ari_ffnum;
    unsigned int ari_cache;
    unsigned int ari_range;

    const unsigned char * RESTRICT ari_input;
          unsigned char * RESTRICT ari_output;
          unsigned char * RESTRICT ari_outputEOB;
          unsigned char * RESTRICT ari_outputStart;

    INLINE void OutputByte(unsigned char s)
    {
        *ari_output++ = s;
    };

    INLINE unsigned char InputByte()
    {
        return *ari_input++;
    };

    NOINLINE unsigned int ShiftLow()
    {
        if (ari.u.low32 < 0xff000000U || ari.u.carry)
        {
            OutputByte(ari_cache + ari.u.carry);
            if (ari_ffnum)
            {
                unsigned char s = ari.u.carry - 1;
                do { OutputByte(s); } while (--ari_ffnum);
            }
            ari_cache = ari.u.low32 >> 24; ari.u.carry = 0;
        } 
        else
        {
            ari_ffnum++;
        }
        
        ari.u.low32 <<= 8; return ari_range << 8;
    }

public:

    INLINE void InitEncoder(unsigned char * output, int outputSize)
    {
        ari_outputStart = output;
        ari_output      = output;
        ari_outputEOB   = output + outputSize - 16;
        ari.low         = 0;
        ari_ffnum       = 0;
        ari_cache       = 0;
        ari_range       = 0xffffffff;
    };

    INLINE int FinishEncoder()
    {
        ShiftLow(); ShiftLow(); ShiftLow(); ShiftLow(); ShiftLow();
        return (int)(ari_output - ari_outputStart);
    }

    INLINE void Encode(unsigned int cum_freq, unsigned int sym_freq, unsigned int total_freq)
    {
        unsigned int range = ari_range / total_freq;
        ari.low += (unsigned long long)cum_freq * range; ari_range = sym_freq * range;

        while (ari_range < 0x1000000) { ari_range = ShiftLow(); }
    }

    template <int P = 12> INLINE unsigned int EncodeBit(unsigned int bit, int probability)
    {
        unsigned int range = (((unsigned long long)ari_range) * probability) >> P;
        ari.low   = ari.low + ((~bit + 1u) & range);
        ari_range = range   + ((~bit + 1u) & (ari_range - range - range));

        while (ari_range < 0x1000000) { ari_range = ShiftLow(); }

        return bit;
    }

    INLINE unsigned int EncodeValue(unsigned int min, unsigned int value, unsigned int max)
    {
        assert(min <= value && value <= max);

        while (max - min >= 0x10000)
        {
            unsigned int median = min + ((max - min) >> 1);
            if (value > median)
            {
                EncodeBit<1>(1, 1);
                min = median + 1;
            }
            else
            {
                EncodeBit<1>(0, 1);
                max = median;
            }
        }

        if (min != max)
        {
            Encode(value - min, 1, max - min + 1);
        }

        return value;
    }

    INLINE void InitDecoder(const unsigned char * input)
    {
        ari_input = input;
        ari_code  = 0;
        ari_range = 0xffffffff;
        ari_code  = (ari_code << 8) | InputByte();
        ari_code  = (ari_code << 8) | InputByte();
        ari_code  = (ari_code << 8) | InputByte();
        ari_code  = (ari_code << 8) | InputByte();
        ari_code  = (ari_code << 8) | InputByte();
    };

    INLINE unsigned int GetCumFreq(unsigned int total_freq)
    {
        while (ari_range < 0x1000000)
        {
            ari_range <<= 8; ari_code = (ari_code << 8) | InputByte();
        }

        return ari_code / (ari_range / total_freq);
    }

    INLINE void Decode(unsigned int cum_freq, unsigned int sym_freq, unsigned int total_freq)
    {
        unsigned int range = ari_range / total_freq;
        ari_code -= cum_freq * range; ari_range = sym_freq * range;
    }

    template <int P = 12> INLINE int DecodeBit(int probability)
    {
        while (ari_range < 0x1000000)
        {
            ari_range <<= 8; ari_code = (ari_code << 8) | InputByte();
        }

        unsigned int range = (((unsigned long long)ari_range) * probability) >> P;
        int bit = ari_code >= range;

        ari_range = bit ? ari_range - range : range;
        ari_code  = bit ? ari_code  - range : ari_code;

        return bit;
    }

    INLINE unsigned int DecodeValue(unsigned int min, unsigned int max)
    {
        assert(min <= max);

        while (max - min >= 0x10000)
        {
            unsigned int median = min + ((max - min) >> 1);
            if (DecodeBit<1>(1))
            {
                min = median + 1;
            }
            else
            {
                max = median;
            }
        }

        if (min != max)
        {
            unsigned int cum_freq = GetCumFreq(max - min + 1);
            Decode(cum_freq, 1, max - min + 1); min += cum_freq;
        }

        return min;
    }
};

#endif

/*-----------------------------------------------------------*/
/* End                                          rangecoder.h */
/*-----------------------------------------------------------*/
