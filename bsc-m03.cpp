/*--

This file is a part of bsc-m03 project.

    Copyright (c) 2021 Ilya Grebnov <ilya.grebnov@gmail.com>

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

static int32_t compress_memory_block(uint8_t * buffer, int32_t block_size, int32_t symbol_size)
{
    if (block_size % symbol_size != 0)
    {
        fprintf(stderr, "\nError: Block size of %d bytes is not a multiple of symbol width!\n", block_size);
        return -2;
    }

    int32_t indexes[32] = { -1 };
    int32_t comressed_size = -1;
    int32_t block_symbols = block_size / symbol_size;
    int32_t r = next_power_of_2(std::max(block_symbols / 16, 1048576));

    if (int32_t * libsais_temp = (int32_t *)malloc(block_symbols * sizeof(int32_t)))
    {
        int32_t result = symbol_size == 1
            ? libsais_bwt_aux(buffer, buffer, libsais_temp, block_symbols, 0, root_frequencies, r, indexes)
            : libsais16_bwt_aux((uint16_t *)buffer, (uint16_t *)buffer, libsais_temp, block_symbols, 0, root_frequencies, r, indexes);

        free(libsais_temp);

        if (result == 0)
        {
            if (uint16_t * L = (uint16_t *)malloc(((size_t)block_symbols + 1) * sizeof(uint16_t)))
            {
                if (m03_parser * parser = (m03_parser *)malloc(sizeof(m03_parser)))
                {
                    {
                        int32_t primary_index = indexes[0];

                        if (symbol_size == 1)
                        {
                            for (int32_t p = 0; p < primary_index; ++p)             { L[p + 0] = ((uint16_t)buffer[p]); }
                            for (int32_t p = primary_index; p < block_symbols; ++p) { L[p + 1] = ((uint16_t)buffer[p]); }
                        }
                        else
                        {
                            for (int32_t p = 0; p < primary_index; ++p)             { L[p + 0] = ((uint16_t *)buffer)[p]; }
                            for (int32_t p = primary_index; p < block_symbols; ++p) { L[p + 1] = ((uint16_t *)buffer)[p]; }
                        }

                        L[primary_index] = 0;
                    }

                    RangeCoder coder;
                    coder.InitEncoder(buffer, block_size);
                    coder.EncodeValue(1, symbol_size, 2);

                    for (int32_t t = 0; t <= (block_symbols - 1) / r; ++t)
                    {
                        coder.EncodeValue(1, indexes[t], block_symbols);
                    }

                    if (parser->initialize(L, block_symbols + 1, indexes[0], root_frequencies, symbol_size == 1 ? 256 : 256 * 256, &coder, m03_mode::encoding))
                    {
                        parser->run();
                        parser->destroy();

                        comressed_size = coder.FinishEncoder();
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

    return comressed_size;
}

static int32_t decompress_memory_block(uint8_t * buffer, int32_t comressed_size, int32_t block_size)
{
    RangeCoder coder;
    coder.InitDecoder(buffer);
    int32_t symbol_size = coder.DecodeValue(1, 2);

    int32_t indexes[32] = { -1 };
    int32_t primary_index = -1;
    int32_t decomressed_size = -1;
    int32_t block_symbols = block_size / symbol_size;
    int32_t r = next_power_of_2(std::max(block_symbols / 16, 1048576));

    for (int32_t t = 0; t <= (block_symbols - 1) / r; ++t)
    {
        indexes[t] = coder.DecodeValue(1, block_symbols);
    }

    if (uint16_t * L = (uint16_t *)malloc(((size_t)block_symbols + 1) * sizeof(uint16_t)))
    {
        if (m03_parser * parser = (m03_parser *)malloc(sizeof(m03_parser)))
        {
            if (parser->initialize(L, block_symbols + 1, indexes[0], root_frequencies, symbol_size == 1 ? 256 : 256 * 256, &coder, m03_mode::decoding))
            {
                parser->run();
                parser->destroy();

                {
                    primary_index = indexes[0];

                    if (symbol_size == 1)
                    {
                        for (int32_t p = 0; p < primary_index; ++p)             { buffer[p] = (uint8_t)L[p + 0]; }
                        for (int32_t p = primary_index; p < block_symbols; ++p) { buffer[p] = (uint8_t)L[p + 1]; }
                    }
                    else
                    {
                        for (int32_t p = 0; p < primary_index; ++p)             { ((uint16_t *)buffer)[p] = L[p + 0]; }
                        for (int32_t p = primary_index; p < block_symbols; ++p) { ((uint16_t *)buffer)[p] = L[p + 1]; }
                    }
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

    if (primary_index > 0)
    {
        if (int32_t * libsais_temp = (int32_t *)malloc(((size_t)block_symbols + 1) * sizeof(int32_t)))
        {
            int32_t result = symbol_size == 1
                ? libsais_unbwt_aux(buffer, buffer, libsais_temp, block_symbols, root_frequencies, r, indexes)
                : libsais16_unbwt_aux((uint16_t *)buffer, (uint16_t *)buffer, libsais_temp, block_symbols, root_frequencies, r, indexes);

            if (result == 0)
            {
                decomressed_size = block_size;
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

    return decomressed_size;
}

static int compress_file(const char * input_file_name, const char * output_file_name, int32_t max_block_size, int32_t symbol_size)
{
    clock_t start_time = clock();
    if (FILE * input_file = fopen(input_file_name, "rb"))
    {
        if (FILE * output_file = fopen(output_file_name, "wb"))
        {
            fseek(input_file, 0, SEEK_END); int64_t remaining_size = _ftelli64(input_file); rewind(input_file);

            if (uint8_t * buffer = (uint8_t *)malloc(std::min(remaining_size, (int64_t)max_block_size) * sizeof(uint8_t)))
            {
                int64_t input_bytes = 0, output_bytes = 0;

                while (remaining_size > 0)
                {
                    fprintf(stdout, "\rCompressing %.55s(%02d%%)", input_file_name, (int)((input_bytes * 100) / (input_bytes + remaining_size)));

                    int32_t block_size = (int32_t)std::min(remaining_size, (int64_t)max_block_size);

                    if (fread(buffer, sizeof(uint8_t), block_size, input_file) != block_size)
                    {
                        fprintf(stderr, "\nError: Unable to read input file!\n");
                        break;
                    }

                    int32_t comressed_size = compress_memory_block(buffer, block_size, symbol_size);
                    if (comressed_size <= 0) { break; }

                    if (fwrite(&block_size, sizeof(uint8_t), sizeof(block_size), output_file) != sizeof(block_size))
                    {
                        fprintf(stderr, "\nError: Unable to write output file!\n");
                        break;
                    }

                    if (fwrite(&comressed_size, sizeof(uint8_t), sizeof(comressed_size), output_file) != sizeof(comressed_size))
                    {
                        fprintf(stderr, "\nError: Unable to write output file!\n");
                        break;
                    }

                    if (fwrite(buffer, sizeof(uint8_t), comressed_size, output_file) != comressed_size)
                    {
                        fprintf(stderr, "\nError: Unable to write output file\n");
                        break;
                    }

                    remaining_size  -= block_size;
                    input_bytes     += block_size;
                    output_bytes    += sizeof(block_size) + sizeof(comressed_size) + comressed_size;
                }

                if (remaining_size == 0)
                {
                    fprintf(stdout, "\r%.55s compressed from %lld into %lld in %.3f seconds (%.3f bps).\n", input_file_name, input_bytes, output_bytes, ((double)clock() - start_time) / CLOCKS_PER_SEC, (8.0 * symbol_size * output_bytes) / input_bytes);
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
                fseek(input_file, 0, SEEK_END); int64_t remaining_size = _ftelli64(input_file); rewind(input_file);

                if (uint8_t * buffer = (uint8_t *)malloc(max_block_size * sizeof(uint8_t)))
                {
                    int64_t input_bytes = 0, output_bytes = 0;

                    while (remaining_size > 0)
                    {
                        fprintf(stdout, "\rDecompressing %.55s(%02d%%)", input_file_name, (int)((input_bytes * 100) / (input_bytes + remaining_size)));

                        int32_t block_size, comressed_size;
                        if (fread(&block_size, sizeof(uint8_t), sizeof(block_size), input_file) != sizeof(block_size))
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        if (fread(&comressed_size, sizeof(uint8_t), sizeof(comressed_size), input_file) != sizeof(comressed_size))
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        if (block_size > max_block_size || comressed_size > max_block_size)
                        {
                            fprintf(stderr, "\nError: The compressed data is corrupted!\n");
                            break;
                        }

                        if (fread(buffer, sizeof(uint8_t), comressed_size, input_file) != comressed_size)
                        {
                            fprintf(stderr, "\nError: Unable to read input file!\n");
                            break;
                        }

                        int32_t decomressed_size = decompress_memory_block(buffer, comressed_size, block_size);
                        if (decomressed_size != block_size) { break; }

                        if (fwrite(buffer, sizeof(uint8_t), decomressed_size, output_file) != decomressed_size)
                        {
                            fprintf(stderr, "\nError: Unable to write output file\n");
                            break;
                        }

                        remaining_size  -= sizeof(block_size) + sizeof(comressed_size) + comressed_size;
                        input_bytes     += sizeof(block_size) + sizeof(comressed_size) + comressed_size;
                        output_bytes    += decomressed_size;
                    }

                    if (remaining_size == 0)
                    {
                        fprintf(stdout, "\r%.55s decompressed from %lld into %lld in %.3f seconds.\n", input_file_name, input_bytes, output_bytes, ((double)clock() - start_time) / CLOCKS_PER_SEC);
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
    fprintf(stdout, "Usage: bsc-m03 <e|d> input-file output-file <options>\n");
    fprintf(stdout, "  -b<size> Block size in bytes, default 128MB (memory usage is ~15x).\n");
    fprintf(stdout, "  -w<8|16> Symbol width in bits.\n");

    return 0;
}

int main(int argc, const char * argv[])
{
    fprintf(stdout, "bsc-m03 is experimental block sorting compressor. Version 0.1.0 (3 December 2021).\n");
    fprintf(stdout, "Copyright (c) 2021 Ilya Grebnov <Ilya.Grebnov@gmail.com>. ABSOLUTELY NO WARRANTY.\n");
    fprintf(stdout, "This program is based on (at least) the work of Michael Maniscalco and Atsushi Komiya.\n\n");

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
                if (max_block_size <= 0) { return print_usage(); }

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
        case 'c':
        case 'C':
        case 'e':
        case 'E':
        {
            return compress_file(argv[2], argv[3], max_block_size, symbol_width / 8);
        }

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