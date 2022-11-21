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
| bib | 111261 | 24550 | 1.765 |
| book1 | 768771 | 203954 | 2.122 |
| book2 | 610856 | 139181 | 1.823 |
| geo | 102400 | 52482 | 4.100 |
| news | 377109 | 105915 | 2.247 |
| obj1 | 21504 | 9779 | 3.638 |
| obj2 | 246814 | 68229 | 2.212 |
| paper1 | 53161 | 15010 | 2.259 |
| paper2 | 82199 | 22641 | 2.204 |
| pic | 513216 | 44587 | 0.695 |
| progc | 39611 | 11303 | 2.283 |
| progl | 71646 | 13565 | 1.515 |
| progp | 49379 | 9291 | 1.505 |
| trans | 93695 | 15399 | 1.315 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 38622 | 2.032 |
| asyoulik.txt | 125179 | 35959 | 2.298 |
| cp.html | 24603 | 6883 | 2.238 |
| fields.c | 11150 | 2704 | 1.940 |
| grammar.lsp | 3721 | 1130 | 2.429 |
| kennedy.xls | 1029744 | 56697 | 0.440 |
| lcet10.txt | 426754 | 95012 | 1.781 |
| plrabn12.txt | 481861 | 129960 | 2.158 |
| ptt5 | 513216 | 44587 | 0.695 |
| sum | 38240 | 11453 | 2.396 |
| xargs.1 | 4227 | 1587 | 3.004 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 699609 | 1.383 |
| E.coli | 4638690 | 1125573 | 1.941 |
| world192.txt | 2473400 | 377228 | 1.220 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2201665 | 1.728 |
| mozilla | 51220480 | 15588180 | 2.435 |
| mr | 9970564 | 2157681 | 1.731 |
| nci | 33553445 | 1127979 | 0.269 |
| ooffice | 6152192 | 2506008 | 3.259 |
| osdb | 10085684 | 2218043 | 1.759 |
| reymont | 6627202 | 961063 | 1.160 |
| samba | 21606400 | 3802811 | 1.408 |
| sao | 7251944 | 4649665 | 5.129 |
| webster | 41458703 | 6259839 | 1.208 |
| xml | 5345280 | 359928 | 0.539 |
| x-ray | 8474240 | 3682048 | 3.476 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7199372 | 1.667 |
| etext99 | 105277340 | 21444578 | 1.630 |
| gcc-3.0.tar | 86630400 | 10083390 | 0.931 |
| howto | 39422105 | 7521856 | 1.526 |
| jdk13c | 69728899 | 2625797 | 0.301 |
| linux-2.4.5.tar | 116254720 | 16410352 | 1.129 |
| rctail96 | 114711151 | 9732849 | 0.679 |
| rfc | 116421901 | 14912324 | 1.025 |
| sprot34.dat | 109617186 | 17175727 | 1.254 |
| w3c2 | 104201579 | 5629500 | 0.432 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 823451 | 7.819 |
| AcroRd32.exe | 3870784 | 1557055 | 3.218 |
| english.dic | 465211 | 145156 | 2.496 |
| FlashMX.pdf | 4526946 | 3712598 | 6.561 |
| FP.LOG | 20617071 | 504344 | 0.196 |
| MSO97.DLL | 3782416 | 1878874 | 3.974 |
| ohs.doc | 4168192 | 803197 | 1.542 |
| rafale.bmp | 4149414 | 743978 | 1.434 |
| vcfiu.hlp | 4121418 | 606466 | 1.177 |
| world95.txt | 2988578 | 443815 | 1.188 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20293393 | 1.623 |
| enwik9 | 1000000000 | 160258936 | 1.282 |

### Pizza & Chilli Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dblp.xml | 296135874 | 21993088 | 0.594 |
| dna | 403927746 | 86344131 | 1.710 |
| english.1024MB | 1073741824 | 194009933 | 1.445 |
| pitches | 55832855 | 16998266 | 2.436 |
| proteins | 1184051855 | 304161163 | 2.055 |
| sources | 210866607 | 29848691 | 1.132 |

### Pizza & Chilli Repetitive Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| cere | 461286644 | 8566781 | 0.149 |
| coreutils | 205281778 | 4314630 | 0.168 |
| einstein.de.txt | 92758441 | 132953 | 0.011 |
| einstein.en.txt | 467626544 | 336959 | 0.006 |
| Escherichia_Coli | 112689515 | 7878020 | 0.559 |
| influenza | 154808555 | 1743038 | 0.090 |
| kernel | 257961616 | 2973476 | 0.092 |
| para | 429265758 | 10678863 | 0.199 |
| world_leaders | 46968181 | 519391 | 0.088 |
| fib41 | 267914296 | 83 | 0.000 |
| rs.13 | 216747218 | 87 | 0.000 |
| tm29 | 268435456 | 160 | 0.000 |
