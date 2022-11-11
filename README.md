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
| bib | 111261 | 24656 | 1.773 |
| book1 | 768771 | 204395 | 2.127 |
| book2 | 610856 | 139566 | 1.828 |
| geo | 102400 | 52580 | 4.108 |
| news | 377109 | 106395 | 2.257 |
| obj1 | 21504 | 9795 | 3.644 |
| obj2 | 246814 | 68414 | 2.218 |
| paper1 | 53161 | 15048 | 2.265 |
| paper2 | 82199 | 22687 | 2.208 |
| pic | 513216 | 44620 | 0.696 |
| progc | 39611 | 11320 | 2.286 |
| progl | 71646 | 13610 | 1.520 |
| progp | 49379 | 9316 | 1.509 |
| trans | 93695 | 15446 | 1.319 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 38667 | 2.034 |
| asyoulik.txt | 125179 | 36019 | 2.302 |
| cp.html | 24603 | 6915 | 2.249 |
| fields.c | 11150 | 2695 | 1.934 |
| grammar.lsp | 3721 | 1130 | 2.429 |
| kennedy.xls | 1029744 | 56568 | 0.439 |
| lcet10.txt | 426754 | 95240 | 1.785 |
| plrabn12.txt | 481861 | 130068 | 2.159 |
| ptt5 | 513216 | 44620 | 0.696 |
| sum | 38240 | 11479 | 2.401 |
| xargs.1 | 4227 | 1585 | 3.000 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 701049 | 1.386 |
| E.coli | 4638690 | 1126463 | 1.943 |
| world192.txt | 2473400 | 378508 | 1.224 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2203859 | 1.730 |
| mozilla | 51220480 | 15630325 | 2.441 |
| mr | 9970564 | 2158802 | 1.732 |
| nci | 33553445 | 1130423 | 0.270 |
| ooffice | 6152192 | 2511633 | 3.266 |
| osdb | 10085684 | 2221807 | 1.762 |
| reymont | 6627202 | 962152 | 1.161 |
| samba | 21606400 | 3816749 | 1.413 |
| sao | 7251944 | 4651078 | 5.131 |
| webster | 41458703 | 6267572 | 1.209 |
| xml | 5345280 | 362358 | 0.542 |
| x-ray | 8474240 | 3681801 | 3.476 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7206590 | 1.668 |
| etext99 | 105277340 | 21508150 | 1.634 |
| gcc-3.0.tar | 86630400 | 10131247 | 0.936 |
| howto | 39422105 | 7556359 | 1.533 |
| jdk13c | 69728899 | 2638786 | 0.303 |
| linux-2.4.5.tar | 116254720 | 16489301 | 1.135 |
| rctail96 | 114711151 | 9788959 | 0.683 |
| rfc | 116421901 | 14967795 | 1.029 |
| sprot34.dat | 109617186 | 17259191 | 1.260 |
| w3c2 | 104201579 | 5666677 | 0.435 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 823496 | 7.820 |
| AcroRd32.exe | 3870784 | 1560548 | 3.225 |
| english.dic | 465211 | 145707 | 2.506 |
| FlashMX.pdf | 4526946 | 3717253 | 6.569 |
| FP.LOG | 20617071 | 505982 | 0.196 |
| MSO97.DLL | 3782416 | 1882533 | 3.982 |
| ohs.doc | 4168192 | 805796 | 1.547 |
| rafale.bmp | 4149414 | 743544 | 1.434 |
| vcfiu.hlp | 4121418 | 608769 | 1.182 |
| world95.txt | 2988578 | 445466 | 1.192 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20339773 | 1.627 |
| enwik9 | 1000000000 | 160616907 | 1.285 |
