# bsc-m03

The bsc-m03 is experimental block sorting compressor based on M03 context aware compression algorithm invented by Michael Maniscalco:
* Michael Maniscalco *M03: A solution for context based blocksort (BWT) compression*, 2004
* Jurgen Abel *Post BWT stages of the Burrows-Wheeler compression algorithm*, 2010

Copyright (c) 2021 Ilya Grebnov <ilya.grebnov@gmail.com>

## License
The libsais is released under the [GNU General Public License](LICENSE "GNU General Public License")

## Changes
* 2021-12-03 : Version 0.1.0
  * Initial public release of the bsc-m03.

# Benchmarks

### Calgary Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bib | 111261 | 25143 | 1.808 |
| book1 | 768771 | 208157 | 2.166 |
| book2 | 610856 | 141591 | 1.854 |
| geo | 102400 | 52797 | 4.125 |
| news | 377109 | 108387 | 2.299 |
| obj1 | 21504 | 9901 | 3.683 |
| obj2 | 246814 | 69689 | 2.259 |
| paper1 | 53161 | 15384 | 2.315 |
| paper2 | 82199 | 23161 | 2.254 |
| pic | 513216 | 44920 | 0.700 |
| progc | 39611 | 11525 | 2.328 |
| progl | 71646 | 13921 | 1.554 |
| progp | 49379 | 9530 | 1.544 |
| trans | 93695 | 15759 | 1.346 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 39310 | 2.068 |
| asyoulik.txt | 125179 | 36585 | 2.338 |
| cp.html | 24603 | 7042 | 2.290 |
| fields.c | 11150 | 2748 | 1.972 |
| grammar.lsp | 3721 | 1142 | 2.455 |
| kennedy.xls | 1029744 | 58440 | 0.454 |
| lcet10.txt | 426754 | 96730 | 1.813 |
| plrabn12.txt | 481861 | 131617 | 2.185 |
| ptt5 | 513216 | 44920 | 0.700 |
| sum | 38240 | 11599 | 2.427 |
| xargs.1 | 4227 | 1618 | 3.062 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 708602 | 1.401 |
| E.coli | 4638690 | 1137915 | 1.962 |
| world192.txt | 2473400 | 384776 | 1.245 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2220939 | 1.743 |
| mozilla | 51220480 | 15831237 | 2.473 |
| mr | 9970564 | 2169223 | 1.741 |
| nci | 33553445 | 1148550 | 0.274 |
| ooffice | 6152192 | 2542258 | 3.306 |
| osdb | 10085684 | 2251471 | 1.786 |
| reymont | 6627202 | 972461 | 1.174 |
| samba | 21606400 | 3881872 | 1.437 |
| sao | 7251944 | 4672656 | 5.155 |
| webster | 41458703 | 6318267 | 1.219 |
| xml | 5345280 | 369196 | 0.553 |
| x-ray | 8474240 | 3697722 | 3.491 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7262753 | 1.681 |
| etext99 | 105277340 | 21730495 | 1.651 |
| gcc-3.0.tar | 86630400 | 10306097 | 0.952 |
| howto | 39422105 | 7662880 | 1.555 |
| jdk13c | 69728899 | 2692938 | 0.309 |
| linux-2.4.5.tar | 116254720 | 16773180 | 1.154 |
| rctail96 | 114711151 | 9949692 | 0.694 |
| rfc | 116421901 | 15192366 | 1.044 |
| sprot34.dat | 109617186 | 17534134 | 1.280 |
| w3c2 | 104201579 | 5800775 | 0.445 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 825162 | 7.836 |
| AcroRd32.exe | 3870784 | 1582677 | 3.271 |
| english.dic | 465211 | 148582 | 2.555 |
| FlashMX.pdf | 4526946 | 3735179 | 6.601 |
| FP.LOG | 20617071 | 514554 | 0.200 |
| MSO97.DLL | 3782416 | 1904460 | 4.028 |
| ohs.doc | 4168192 | 817718 | 1.569 |
| rafale.bmp | 4149414 | 750437 | 1.447 |
| vcfiu.hlp | 4121418 | 620358 | 1.204 |
| world95.txt | 2988578 | 452271 | 1.211 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20529360 | 1.642 |
| enwik9 | 1000000000 | 162084133 | 1.297 |
