# bsc-m03

The bsc-m03 is experimental block sorting compressor based on M03 context aware compression algorithm invented by Michael Maniscalco:
* Michael Maniscalco *M03: A solution for context based blocksort (BWT) compression*, 2004
* Jurgen Abel *Post BWT stages of the Burrows-Wheeler compression algorithm*, 2010

Copyright (c) 2021-2022 Ilya Grebnov <ilya.grebnov@gmail.com>

## License
The bsc-m03 is released under the [GNU General Public License](LICENSE "GNU General Public License")

## Changes
* 2022-01-08 : Version 0.2.1
  * Replaced std::stable_sort with ska_sort.
* 2022-01-05 : Version 0.2
  * Improved compression.
  * Reduced memory usage from 15x to 13x.
* 2021-12-07 : Version 0.1.1 - 0.1.2
  * Slightly improved compression using symbols history.
* 2021-12-03 : Version 0.1.0
  * Initial public release of the bsc-m03.

# Benchmarks

### Calgary Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bib | 111261 | 24832 | 1.785 |
| book1 | 768771 | 206247 | 2.146 |
| book2 | 610856 | 140103 | 1.835 |
| geo | 102400 | 52597 | 4.109 |
| news | 377109 | 107049 | 2.271 |
| obj1 | 21504 | 9863 | 3.669 |
| obj2 | 246814 | 68833 | 2.231 |
| paper1 | 53161 | 15145 | 2.279 |
| paper2 | 82199 | 22824 | 2.221 |
| pic | 513216 | 44694 | 0.697 |
| progc | 39611 | 11390 | 2.300 |
| progl | 71646 | 13689 | 1.529 |
| progp | 49379 | 9376 | 1.519 |
| trans | 93695 | 15550 | 1.328 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 38841 | 2.043 |
| asyoulik.txt | 125179 | 36149 | 2.310 |
| cp.html | 24603 | 6969 | 2.266 |
| fields.c | 11150 | 2712 | 1.946 |
| grammar.lsp | 3721 | 1138 | 2.447 |
| kennedy.xls | 1029744 | 56929 | 0.442 |
| lcet10.txt | 426754 | 95628 | 1.793 |
| plrabn12.txt | 481861 | 130437 | 2.166 |
| ptt5 | 513216 | 44694 | 0.697 |
| sum | 38240 | 11539 | 2.414 |
| xargs.1 | 4227 | 1603 | 3.034 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 703933 | 1.391 |
| E.coli | 4638690 | 1129304 | 1.948 |
| world192.txt | 2473400 | 381247 | 1.233 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2208219 | 1.733 |
| mozilla | 51220480 | 15704019 | 2.453 |
| mr | 9970564 | 2160359 | 1.733 |
| nci | 33553445 | 1137038 | 0.271 |
| ooffice | 6152192 | 2522972 | 3.281 |
| osdb | 10085684 | 2230920 | 1.770 |
| reymont | 6627202 | 964011 | 1.164 |
| samba | 21606400 | 3839503 | 1.422 |
| sao | 7251944 | 4656134 | 5.136 |
| webster | 41458703 | 6279969 | 1.212 |
| xml | 5345280 | 364952 | 0.546 |
| x-ray | 8474240 | 3685642 | 3.479 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7227116 | 1.673 |
| etext99 | 105277340 | 21586520 | 1.640 |
| gcc-3.0.tar | 86630400 | 10198397 | 0.942 |
| howto | 39422105 | 7594162 | 1.541 |
| jdk13c | 69728899 | 2659297 | 0.305 |
| linux-2.4.5.tar | 116254720 | 16599153 | 1.142 |
| rctail96 | 114711151 | 9852234 | 0.687 |
| rfc | 116421901 | 15047359 | 1.034 |
| sprot34.dat | 109617186 | 17382679 | 1.269 |
| w3c2 | 104201579 | 5717299 | 0.439 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 823856 | 7.823 |
| AcroRd32.exe | 3870784 | 1568677 | 3.242 |
| english.dic | 465211 | 147280 | 2.533 |
| FlashMX.pdf | 4526946 | 3721859 | 6.577 |
| FP.LOG | 20617071 | 508327 | 0.197 |
| MSO97.DLL | 3782416 | 1890558 | 3.999 |
| ohs.doc | 4168192 | 810011 | 1.555 |
| rafale.bmp | 4149414 | 745966 | 1.438 |
| vcfiu.hlp | 4121418 | 613304 | 1.190 |
| world95.txt | 2988578 | 448323 | 1.200 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20398312 | 1.632 |
| enwik9 | 1000000000 | 161062976 | 1.289 |
