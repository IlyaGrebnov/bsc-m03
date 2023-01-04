# bsc-m03

The bsc-m03 is experimental block sorting compressor based on M03 context aware compression algorithm invented by Michael Maniscalco:
* Michael Maniscalco *M03: A solution for context based blocksort (BWT) compression*, 2004
* Jurgen Abel *Post BWT stages of the Burrows-Wheeler compression algorithm*, 2010

Moreover, the bsc-m03 compressor is a practical implementation of *Compression via Substring Enumeration* for byte-oriented sources:
* Danny Dube, Vincent Beaudoin *Lossless Data Compression via Substring Enumeration*, 2010
* Takahiro Ota, Hiroyoshi Morita, Akiko Manada *Compression by Substring Enumeration with a Finite Alphabet Using Sorting*, 2018

Copyright (c) 2021-2022 Ilya Grebnov <ilya.grebnov@gmail.com>

## License
The bsc-m03 is released under the [GNU General Public License](LICENSE "GNU General Public License")

## Changes
* 2022-11-27 : Version 0.5.0
  * Compression ratio improvements.
* 2022-11-20 : Version 0.4.0
  * Compression ratio improvements.
* 2022-11-10 : Version 0.3.0
  * Compression ratio improvements.
* 2022-01-08 : Version 0.2.1
  * Performance improvements.
* 2022-01-05 : Version 0.2
  * Memory usage improvements.
  * Compression ratio improvements.
* 2021-12-07 : Version 0.1.1 - 0.1.2
  * Minor compression ratio improvements.
* 2021-12-03 : Version 0.1.0
  * Initial public release of the bsc-m03.

# Benchmarks

### Calgary Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bib | 111261 | 24479 | 1.760 |
| book1 | 768771 | 203745 | 2.120 |
| book2 | 610856 | 138870 | 1.819 |
| geo | 102400 | 52465 | 4.099 |
| news | 377109 | 105621 | 2.241 |
| obj1 | 21504 | 9775 | 3.637 |
| obj2 | 246814 | 68003 | 2.204 |
| paper1 | 53161 | 14957 | 2.251 |
| paper2 | 82199 | 22594 | 2.199 |
| pic | 513216 | 44424 | 0.692 |
| progc | 39611 | 11257 | 2.274 |
| progl | 71646 | 13512 | 1.509 |
| progp | 49379 | 9248 | 1.498 |
| trans | 93695 | 15310 | 1.307 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 38562 | 2.028 |
| asyoulik.txt | 125179 | 35889 | 2.294 |
| cp.html | 24603 | 6872 | 2.235 |
| fields.c | 11150 | 2685 | 1.926 |
| grammar.lsp | 3721 | 1120 | 2.408 |
| kennedy.xls | 1029744 | 57440 | 0.446 |
| lcet10.txt | 426754 | 94823 | 1.778 |
| plrabn12.txt | 481861 | 129770 | 2.154 |
| ptt5 | 513216 | 44424 | 0.692 |
| sum | 38240 | 11426 | 2.390 |
| xargs.1 | 4227 | 1585 | 3.000 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 698395 | 1.380 |
| E.coli | 4638690 | 1126125 | 1.942 |
| world192.txt | 2473400 | 376173 | 1.217 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2199344 | 1.726 |
| mozilla | 51220480 | 15589159 | 2.435 |
| mr | 9970564 | 2156826 | 1.731 |
| nci | 33553445 | 1126386 | 0.269 |
| ooffice | 6152192 | 2503991 | 3.256 |
| osdb | 10085684 | 2223002 | 1.763 |
| reymont | 6627202 | 958772 | 1.157 |
| samba | 21606400 | 3794300 | 1.405 |
| sao | 7251944 | 4649723 | 5.129 |
| webster | 41458703 | 6253627 | 1.207 |
| xml | 5345280 | 357958 | 0.536 |
| x-ray | 8474240 | 3681388 | 3.475 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7206269 | 1.668 |
| etext99 | 105277340 | 21422251 | 1.628 |
| gcc-3.0.tar | 86630400 | 10046880 | 0.928 |
| howto | 39422105 | 7504315 | 1.523 |
| jdk13c | 69728899 | 2612434 | 0.300 |
| linux-2.4.5.tar | 116254720 | 16351863 | 1.125 |
| rctail96 | 114711151 | 9707347 | 0.677 |
| rfc | 116421901 | 14871775 | 1.022 |
| sprot34.dat | 109617186 | 17157222 | 1.252 |
| w3c2 | 104201579 | 5598687 | 0.430 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 823533 | 7.820 |
| AcroRd32.exe | 3870784 | 1555832 | 3.216 |
| english.dic | 465211 | 145096 | 2.495 |
| FlashMX.pdf | 4526946 | 3712716 | 6.561 |
| FP.LOG | 20617071 | 502648 | 0.195 |
| MSO97.DLL | 3782416 | 1878076 | 3.972 |
| ohs.doc | 4168192 | 803171 | 1.542 |
| rafale.bmp | 4149414 | 745470 | 1.437 |
| vcfiu.hlp | 4121418 | 604165 | 1.173 |
| world95.txt | 2988578 | 442271 | 1.184 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20263925 | 1.621 |
| enwik9 | 1000000000 | 160018905 | 1.280 |

### Pizza & Chilli Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dblp.xml | 296135874 | 21926695 | 0.592 |
| dna | 403927746 | 86414423 | 1.711 |
| english.1024MB | 1073741824 | 193810792 | 1.444 |
| pitches | 55832855 | 16984071 | 2.434 |
| proteins | 1184051855 | 304486803 | 2.057 |
| sources | 210866607 | 29749020 | 1.129 |

### Pizza & Chilli Repetitive Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| cere | 461286644 | 8576879 | 0.149 |
| coreutils | 205281778 | 4293243 | 0.167 |
| einstein.de.txt | 92758441 | 132286 | 0.011 |
| einstein.en.txt | 467626544 | 336029 | 0.006 |
| Escherichia_Coli | 112689515 | 7928044 | 0.563 |
| influenza | 154808555 | 1760692 | 0.091 |
| kernel | 257961616 | 2955825 | 0.092 |
| para | 429265758 | 10730998 | 0.200 |
| world_leaders | 46968181 | 518220 | 0.088 |
| fib41 | 267914296 | 83 | 0.000 |
| rs.13 | 216747218 | 86 | 0.000 |
| tm29 | 268435456 | 158 | 0.000 |
