/*-----------------------------------------------------------*/
/* Block Sorting, Lossless Data Compression Library.         */
/* Interface to platform specific functions and constants    */
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

#ifndef _LIBBSC_PLATFORM_H
#define _LIBBSC_PLATFORM_H

#ifdef _MSC_VER
    #define fseeko(stream, offset, origin) _fseeki64(stream, offset, origin)
    #define ftello(stream) _ftelli64(stream)
#endif

#if defined(_MSC_VER)
    #include <intrin.h>
#else
    #include <immintrin.h>
#endif

#if defined(__GNUC__)
    #define INLINE __inline__
#elif defined(_MSC_VER)
    #define INLINE __forceinline
#elif defined(__IBMC__)
    #define INLINE _Inline
#elif defined(__cplusplus)
    #define INLINE inline
#else
    #define INLINE /* */
#endif

#if defined(_MSC_VER)
    #define NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
    #define NOINLINE __attribute__ ((noinline))
#else
    #define NOINLINE /* */
#endif

#if defined(_MSC_VER)
    #define ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
    #define ALIGNED(x) __attribute__ ((aligned(x)))
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__CUDACC__)
    #define RESTRICT __restrict__
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
    #define RESTRICT __restrict
#else
    #define RESTRICT /* */
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define byteswap_uint64(x)    (__builtin_bswap64(x))
    #define bit_scan_reverse(x)   (__builtin_clz(x) ^ 31)
    #define bit_scan_forward(x)   (__builtin_ctz(x))
    #define bit_scan_forward64(x) (__builtin_ctzll(x))
#elif defined(_MSC_VER)
    #define byteswap_uint64(x)  (_byteswap_uint64(x))

    #pragma intrinsic(_BitScanReverse)
    #pragma intrinsic(_BitScanForward)

    static inline __forceinline unsigned long bit_scan_reverse(unsigned long x) 
    {
       unsigned long index;
       _BitScanReverse(&index, x);
       return index;
    }

    static inline __forceinline unsigned long bit_scan_forward(unsigned long x) 
    {
       unsigned long index;
       _BitScanForward(&index, x);
       return index;
    }
#endif

    static INLINE unsigned int next_power_of_2(unsigned int v)
    {
        v--;

        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;

        return v;
    }

#endif

/*-----------------------------------------------------------*/
/* End                                            platform.h */
/*-----------------------------------------------------------*/
