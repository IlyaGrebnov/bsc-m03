/*--

This file is a part of bsc-m03 project.

    Copyright (c) 2021-2024 Ilya Grebnov <ilya.grebnov@gmail.com>

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

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>

#include "libsais/libsais.h"
#include "libsais/libsais16.h"

#include "common/platform.h"
#include "common/rangecoder.h"

#define MAX_ALPHABET_SIZE (256 * 256)

#include "m03_parser.h"

#pragma warning( push )
#pragma warning( disable : 6385 )
#pragma warning( disable : 6386 )

int32_t root_frequencies[MAX_ALPHABET_SIZE + 1];

#if !defined(BSC_DECOMPRESSION_ONLY)

template <class symbol_t> static int32_t compress_memory_block(uint8_t * buffer, int32_t block_size)
{
    int32_t indexes[32]     = { -1 };
    int32_t compressed_size = -1;
    int32_t symbol_size     = (int32_t)sizeof(symbol_t);
    int32_t block_symbols   = block_size / symbol_size;
    int32_t r               = next_power_of_2(std::max(block_symbols / 16, 1048576));

    if (block_size % symbol_size != 0)
    {
        fprintf(stderr, "\nError: Block size of %d bytes is not a multiple of symbol width!\n", block_size);
        return -2;
    }

    if (int32_t * libsais_temp = (int32_t *)malloc(block_symbols * sizeof(int32_t)))
    {
        int32_t result = symbol_size == 1
            ? libsais_bwt_aux(buffer, buffer, libsais_temp, block_symbols, 0, root_frequencies, r, indexes)
            : libsais16_bwt_aux((uint16_t *)buffer, (uint16_t *)buffer, libsais_temp, block_symbols, 0, root_frequencies, r, indexes);

        free(libsais_temp);

        if (result == 0)
        {
            if (symbol_t * L = (symbol_t *)malloc(((size_t)block_symbols + 1) * sizeof(symbol_t)))
            {
                if (m03_parser<symbol_t, m03_mode::encoding> * parser = (m03_parser<symbol_t, m03_mode::encoding> *)malloc(sizeof(m03_parser<symbol_t, m03_mode::encoding>)))
                {
                    {
                        int32_t primary_index = indexes[0];

                        memcpy(&L[0]                , &((symbol_t *)buffer)[0]            , primary_index * sizeof(symbol_t));
                        memcpy(&L[primary_index + 1], &((symbol_t *)buffer)[primary_index], ((size_t)block_symbols - (size_t)primary_index) * sizeof(symbol_t));

                        L[primary_index] = 0;
                    }

                    RangeCoder coder;
                    coder.InitEncoder(buffer, block_size);
                    coder.EncodeValue(1, symbol_size, 2);

                    for (int32_t t = 0; t <= (block_symbols - 1) / r; ++t)
                    {
                        coder.EncodeValue(1, indexes[t], block_symbols);
                    }

                    if (parser->initialize(L, block_symbols + 1, indexes[0], root_frequencies, 1 << (8 * symbol_size), &coder))
                    {
                        parser->run();
                        parser->destroy();

                        compressed_size = coder.FinishEncoder();
                    }
                    else
                    {
                        fprintf(stderr, "\nError: Not enough memory!\n");
                    }

                    free(parser);
                }
                else
                {
                    fprintf(stderr, "\nError: Not enough memory!\n");
                }

                if (compressed_size >= block_size)
                {
                    compressed_size = -1;

                    if (int32_t * libsais_temp = (int32_t *)malloc(((size_t)block_symbols + 1) * sizeof(int32_t)))
                    {
                        {
                            int32_t primary_index = indexes[0];

                            memcpy(&((symbol_t *)buffer)[0]            , &L[0]                , primary_index * sizeof(symbol_t));
                            memcpy(&((symbol_t *)buffer)[primary_index], &L[primary_index + 1], ((size_t)block_symbols - (size_t)primary_index) * sizeof(symbol_t));
                        }

                        result = symbol_size == 1
                            ? libsais_unbwt_aux((uint8_t *)buffer, (uint8_t *)buffer, libsais_temp, block_symbols, root_frequencies, r, indexes)
                            : libsais16_unbwt_aux((uint16_t *)buffer, (uint16_t *)buffer, libsais_temp, block_symbols, root_frequencies, r, indexes);

                        if (result == 0)
                        {
                            compressed_size = block_size;
                        }
                        else
                        {
                            fprintf(stderr, "\nError: libsais_unbwt failed, please contact the author!\n");
                        }

                        free(libsais_temp);
                    }
                    else
                    {
                        fprintf(stderr, "\nError: Not enough memory!\n");
                    }
                }

                free(L);
            }
            else
            {
                fprintf(stderr, "\nError: Not enough memory!\n");
            }
        }
        else
        {
            fprintf(stderr, "\nError: libsais_bwt failed, please contact the author!\n");
        }
    }
    else
    {
        fprintf(stderr, "\nError: Not enough memory!\n");
    }

    return compressed_size;
}

#endif

template <class symbol_t> static int32_t decompress_burrows_wheeler_transform(RangeCoder * coder, int32_t primary_index, int32_t block_symbols, uint8_t * buffer)
{
    int32_t result          = -1;
    int32_t symbol_size     = (int32_t)sizeof(symbol_t);

    if (symbol_t * L = (symbol_t *)malloc(((size_t)block_symbols + 1) * sizeof(symbol_t)))
    {
        if (m03_parser<symbol_t, m03_mode::decoding> * parser = (m03_parser<symbol_t, m03_mode::decoding> *)malloc(sizeof(m03_parser<symbol_t, m03_mode::decoding>)))
        {
            if (parser->initialize(L, block_symbols + 1, primary_index, root_frequencies, 1 << (8 * symbol_size), coder))
            {
                parser->run();
                parser->destroy();

                {
                    memcpy(&((symbol_t *)buffer)[0]            , &L[0]                , primary_index * sizeof(symbol_t));
                    memcpy(&((symbol_t *)buffer)[primary_index], &L[primary_index + 1], ((size_t)block_symbols - (size_t)primary_index) * sizeof(symbol_t));

                    result = 0;
                }
            }
            else
            {
                fprintf(stderr, "\nError: Not enough memory!\n");
            }

            free(parser);
        }
        else
        {
            fprintf(stderr, "\nError: Not enough memory!\n");
        }

        free(L);
    }
    else
    {
        fprintf(stderr, "\nError: Not enough memory!\n");
    }

    return result;
}

static int32_t decompress_memory_block(uint8_t * buffer, int32_t compressed_size, int32_t block_size)
{
    RangeCoder coder;
    coder.InitDecoder(buffer);

    int32_t indexes[32]         = { -1 };
    int32_t decompressed_size   = -1;
    int32_t symbol_size         = coder.DecodeValue(1, 2);
    int32_t block_symbols       = block_size / symbol_size;
    int32_t r                   = next_power_of_2(std::max(block_symbols / 16, 1048576));

    for (int32_t t = 0; t <= (block_symbols - 1) / r; ++t)
    {
        indexes[t] = coder.DecodeValue(1, block_symbols);
    }

    int32_t result = symbol_size == 1
        ? decompress_burrows_wheeler_transform<uint8_t> (&coder, indexes[0], block_symbols, buffer)
        : decompress_burrows_wheeler_transform<uint16_t>(&coder, indexes[0], block_symbols, buffer);

    if (result == 0)
    {
        if (int32_t * libsais_temp = (int32_t *)malloc(((size_t)block_symbols + 1) * sizeof(int32_t)))
        {
            result = symbol_size == 1
                ? libsais_unbwt_aux(buffer, buffer, libsais_temp, block_symbols, root_frequencies, r, indexes)
                : libsais16_unbwt_aux((uint16_t *)buffer, (uint16_t *)buffer, libsais_temp, block_symbols, root_frequencies, r, indexes);

            if (result == 0)
            {
                decompressed_size = block_size;
            }
            else
            {
                fprintf(stderr, "\nError: libsais_unbwt failed, please contact the author!\n");
            }

            free(libsais_temp);
        }
        else
        {
            fprintf(stderr, "\nError: Not enough memory!\n");
        }
    }

    return decompressed_size;
}

#if !defined(BSC_DECOMPRESSION_ONLY)

static int compress_file(const char * input_file_name, const char * output_file_name, int32_t max_block_size, int32_t symbol_size)
{
    clock_t start_time = clock();
    if (FILE * input_file = fopen(input_file_name, "rb"))
    {
        if (FILE * output_file = fopen(output_file_name, "wb"))
        {
            fseeko(input_file, 0, SEEK_END); int64_t remaining_size = ftello(input_file); rewind(input_file);

            int64_t buffer_size = std::min(remaining_size, (int64_t)max_block_size) + 16384; buffer_size += buffer_size / 16;

            if (uint8_t * buffer = (uint8_t *)malloc(buffer_size * sizeof(uint8_t)))
            {
                int64_t input_bytes = 0, output_bytes = 0;

                while (remaining_size > 0)
                {
                    fprintf(stdout, "\rCompressing %.55s(%02d%%)", input_file_name, (int)((input_bytes * 100) / (input_bytes + remaining_size))); fflush(stdout);

                    int32_t block_size = (int32_t)std::min(remaining_size, (int64_t)max_block_size);

                    if (fread(buffer, sizeof(uint8_t), block_size, input_file) != block_size)
                    {
                        fprintf(stderr, "\nError: Unable to read input file!\n");
                        break;
                    }

                    int32_t compressed_size = symbol_size == 1 
                        ? compress_memory_block<uint8_t> (buffer, block_size) 
                        : compress_memory_block<uint16_t>(buffer, block_size);

                    if (compressed_size <= 0 || compressed_size > block_size)
                    {
                        fprintf(stderr, "\nError: compression failed, please contact the author!\n");
                        break;
                    }

                    if (fwrite(&block_size, sizeof(uint8_t), sizeof(block_size), output_file) != sizeof(block_size))
                    {
                        fprintf(stderr, "\nError: Unable to write output file!\n");
                        break;
                    }

                    if (fwrite(&compressed_size, sizeof(uint8_t), sizeof(compressed_size), output_file) != sizeof(compressed_size))
                    {
                        fprintf(stderr, "\nError: Unable to write output file!\n");
                        break;
                    }

                    if (fwrite(buffer, sizeof(uint8_t), compressed_size, output_file) != compressed_size)
                    {
                        fprintf(stderr, "\nError: Unable to write output file\n");
                        break;
                    }

                    remaining_size  -= block_size;
                    input_bytes     += block_size;
                    output_bytes    += sizeof(block_size) + sizeof(compressed_size) + compressed_size;
                }

                if (remaining_size == 0)
                {
                    fprintf(stdout, "\r%.55s compressed from %lld into %lld in %.3f seconds (%.3f bps).\n", input_file_name, (long long int)input_bytes, (long long int)output_bytes, ((double)clock() - start_time) / CLOCKS_PER_SEC, (8.0 * symbol_size * output_bytes) / input_bytes);
                }

                free(buffer);
            }
            else
            {
                fprintf(stderr, "Error: Not enough memory!\n");
            }

            fclose(output_file);
        }
        else
        {
            fprintf(stderr, "Error: Unable to open output file!\n");
        }

        fclose(input_file);
    }
    else
    {
        fprintf(stderr, "Error: Unable to open input file!\n");
    }

    return 0;
}

#endif

static int decompress_file(const char * input_file_name, const char * output_file_name)
{
    clock_t start_time = clock();
    if (FILE * input_file = fopen(input_file_name, "rb"))
    {
        if (FILE * output_file = fopen(output_file_name, "wb"))
        {
            int32_t max_block_size;
            if (fread(&max_block_size, sizeof(uint8_t), sizeof(max_block_size), input_file) == sizeof(max_block_size))
            {
                fseeko(input_file, 0, SEEK_END); int64_t remaining_size = ftello(input_file); rewind(input_file);

                if (uint8_t * buffer = (uint8_t *)malloc(max_block_size * sizeof(uint8_t)))
                {
                    int64_t input_bytes = 0, output_bytes = 0;

                    while (remaining_size > 0)
                    {
                        fprintf(stdout, "\rDecompressing %.55s(%02d%%)", input_file_name, (int)((input_bytes * 100) / (input_bytes + remaining_size))); fflush(stdout);

                        int32_t block_size, compressed_size;
                        if (fread(&block_size, sizeof(uint8_t), sizeof(block_size), input_file) != sizeof(block_size))
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        if (fread(&compressed_size, sizeof(uint8_t), sizeof(compressed_size), input_file) != sizeof(compressed_size))
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        if (block_size > max_block_size || compressed_size > block_size)
                        {
                            fprintf(stderr, "\nError: The compressed data is corrupted!\n");
                            break;
                        }

                        if (fread(buffer, sizeof(uint8_t), compressed_size, input_file) != compressed_size)
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        int32_t decompressed_size = compressed_size < block_size
                            ? decompress_memory_block(buffer, compressed_size, block_size)
                            : block_size;

                        if (decompressed_size != block_size)
                        {
                            fprintf(stderr, "\nError: The compressed data is corrupted!\n");
                            break;
                        }

                        if (fwrite(buffer, sizeof(uint8_t), decompressed_size, output_file) != decompressed_size)
                        {
                            fprintf(stderr, "\nError: Unable to write output file\n");
                            break;
                        }

                        remaining_size  -= sizeof(block_size) + sizeof(compressed_size) + compressed_size;
                        input_bytes     += sizeof(block_size) + sizeof(compressed_size) + compressed_size;
                        output_bytes    += decompressed_size;
                    }

                    if (remaining_size == 0)
                    {
                        fprintf(stdout, "\r%.55s decompressed from %lld into %lld in %.3f seconds.\n", input_file_name, (long long int)input_bytes, (long long int)output_bytes, ((double)clock() - start_time) / CLOCKS_PER_SEC);
                    }

                    free(buffer);
                }
                else
                {
                    fprintf(stderr, "Error: Not enough memory!\n");
                }
            }
            else
            {
                fprintf(stderr, "Error: Unable to read input file!\n");
            }

            fclose(output_file);
        }
        else
        {
            fprintf(stderr, "Error: Unable to open output file!\n");
        }

        fclose(input_file);
    }
    else
    {
        fprintf(stderr, "Error: Unable to open input file!\n");
    }

    return 0;
}

static int print_usage()
{
#if !defined(BSC_DECOMPRESSION_ONLY)
    fprintf(stdout, "Usage: bsc-m03 <e|d> input-file output-file <options>\n");
    fprintf(stdout, "  -b<size> Block size in bytes, default 128MB (memory usage is ~13x).\n");
    fprintf(stdout, "  -w<8|16> Symbol width in bits.\n");
#else
    fprintf(stdout, "Usage: bsc-m03 d input-file output-file\n");
#endif

    return 0;
}

int main(int argc, const char * argv[])
{
    fprintf(stdout, "bsc-m03 is experimental block sorting compressor. Version 0.5.5 (8 May 2024).\n");
    fprintf(stdout, "Copyright (c) 2021-2024 Ilya Grebnov <Ilya.Grebnov@gmail.com>. ABSOLUTELY NO WARRANTY.\n");
    fprintf(stdout, "This program is based on (at least) the work of Michael Maniscalco (see AUTHORS).\n\n");

    int32_t max_block_size  = 128 * 1024 * 1024;
    int32_t symbol_width    = 8;

    if (argc < 4 || strlen(argv[1]) != 1)
    {
        return print_usage();
    }

    for (int32_t i = 4; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            return print_usage();
        }

        switch (argv[i][1])
        {
            case 'b':
            {
                max_block_size = atoi(argv[i] + 2);
                if (max_block_size <= 0 || max_block_size > 2047 * 1024 * 1024) { return print_usage(); }

                break;
            }

            case 'w':
            {
                symbol_width = atoi(argv[i] + 2);
                if (symbol_width != 8 && symbol_width != 16) { return print_usage(); }

                break;
            }

            default:
            {
                return print_usage();
            }
        }
    }

    switch (argv[1][0])
    {
#if !defined(BSC_DECOMPRESSION_ONLY)
        case 'c':
        case 'C':
        case 'e':
        case 'E':
        {
            return compress_file(argv[2], argv[3], max_block_size, symbol_width / 8);
        }
#endif

        case 'd':
        case 'D':
        {
            if (argc != 4) { return print_usage(); }

            return decompress_file(argv[2], argv[3]);
        }

        default:
        {
            return print_usage();
        }
    }

    return 0;
}

#pragma warning( pop )
