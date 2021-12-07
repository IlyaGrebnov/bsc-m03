# bsc-m03

The bsc-m03 is experimental block sorting compressor based on M03 context aware compression algorithm invented by Michael Maniscalco:
* Michael Maniscalco *M03: A solution for context based blocksort (BWT) compression*, 2004
* Jurgen Abel *Post BWT stages of the Burrows-Wheeler compression algorithm*, 2010

Copyright (c) 2021 Ilya Grebnov <ilya.grebnov@gmail.com>

## License
The libsais is released under the [GNU General Public License](LICENSE "GNU General Public License")

## Changes
* 2021-12-07 : Version 0.1.1
  * Slightly improved compression using symbols history.
* 2021-12-03 : Version 0.1.0
  * Initial public release of the bsc-m03.

# Benchmarks

### Calgary Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bib | 111261 | 25098 | 1.805 |
| book1 | 768771 | 207930 | 2.164 |
| book2 | 610856 | 141245 | 1.850 |
| geo | 102400 | 52825 | 4.127 |
| news | 377109 | 107965 | 2.290 |
| obj1 | 21504 | 9904 | 3.685 |
| obj2 | 246814 | 69337 | 2.247 |
| paper1 | 53161 | 15330 | 2.307 |
| paper2 | 82199 | 23099 | 2.248 |
| pic | 513216 | 44961 | 0.701 |
| progc | 39611 | 11526 | 2.328 |
| progl | 71646 | 13892 | 1.551 |
| progp | 49379 | 9514 | 1.541 |
| trans | 93695 | 15739 | 1.344 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 39249 | 2.065 |
| asyoulik.txt | 125179 | 36508 | 2.333 |
| cp.html | 24603 | 7046 | 2.291 |
| fields.c | 11150 | 2752 | 1.975 |
| grammar.lsp | 3721 | 1148 | 2.468 |
| kennedy.xls | 1029744 | 58978 | 0.458 |
| lcet10.txt | 426754 | 96523 | 1.809 |
| plrabn12.txt | 481861 | 131473 | 2.183 |
| ptt5 | 513216 | 44961 | 0.701 |
| sum | 38240 | 11636 | 2.434 |
| xargs.1 | 4227 | 1620 | 3.066 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 707710 | 1.399 |
| E.coli | 4638690 | 1138016 | 1.963 |
| world192.txt | 2473400 | 383758 | 1.241 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2218186 | 1.741 |
| mozilla | 51220480 | 15784688 | 2.465 |
| mr | 9970564 | 2168769 | 1.740 |
| nci | 33553445 | 1147399 | 0.274 |
| ooffice | 6152192 | 2533840 | 3.295 |
| osdb | 10085684 | 2250910 | 1.785 |
| reymont | 6627202 | 970070 | 1.171 |
| samba | 21606400 | 3868421 | 1.432 |
| sao | 7251944 | 4671956 | 5.154 |
| webster | 41458703 | 6309084 | 1.217 |
| xml | 5345280 | 367771 | 0.550 |
| x-ray | 8474240 | 3698091 | 3.491 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7262254 | 1.681 |
| etext99 | 105277340 | 21704149 | 1.649 |
| gcc-3.0.tar | 86630400 | 10263588 | 0.948 |
| howto | 39422105 | 7635242 | 1.549 |
| jdk13c | 69728899 | 2680664 | 0.308 |
| linux-2.4.5.tar | 116254720 | 16701149 | 1.149 |
| rctail96 | 114711151 | 9918165 | 0.692 |
| rfc | 116421901 | 15141656 | 1.040 |
| sprot34.dat | 109617186 | 17473161 | 1.275 |
| w3c2 | 104201579 | 5766640 | 0.443 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 825193 | 7.836 |
| AcroRd32.exe | 3870784 | 1576102 | 3.257 |
| english.dic | 465211 | 148631 | 2.556 |
| FlashMX.pdf | 4526946 | 3732972 | 6.597 |
| FP.LOG | 20617071 | 513631 | 0.199 |
| MSO97.DLL | 3782416 | 1897323 | 4.013 |
| ohs.doc | 4168192 | 814842 | 1.564 |
| rafale.bmp | 4149414 | 750463 | 1.447 |
| vcfiu.hlp | 4121418 | 617351 | 1.198 |
| world95.txt | 2988578 | 451058 | 1.207 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20487507 | 1.639 |
| enwik9 | 1000000000 | 161805758 | 1.294 |
