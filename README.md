# bsc-m03

The bsc-m03 is experimental block sorting compressor based on M03 context aware compression algorithm invented by Michael Maniscalco:
* Michael Maniscalco *M03: A solution for context based blocksort (BWT) compression*, 2004
* Jurgen Abel *Post BWT stages of the Burrows-Wheeler compression algorithm*, 2010

Copyright (c) 2021 Ilya Grebnov <ilya.grebnov@gmail.com>

## License
The libsais is released under the [GNU General Public License](LICENSE "GNU General Public License")

## Changes
* 2021-12-07 : Version 0.1.1 - 0.1.2
  * Slightly improved compression using symbols history.
* 2021-12-03 : Version 0.1.0
  * Initial public release of the bsc-m03.

# Benchmarks

### Calgary Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bib | 111261 | 25090 | 1.804 |
| book1 | 768771 | 207896 | 2.163 |
| book2 | 610856 | 141204 | 1.849 |
| geo | 102400 | 52821 | 4.127 |
| news | 377109 | 107940 | 2.290 |
| obj1 | 21504 | 9903 | 3.684 |
| obj2 | 246814 | 69338 | 2.247 |
| paper1 | 53161 | 15327 | 2.307 |
| paper2 | 82199 | 23090 | 2.247 |
| pic | 513216 | 44960 | 0.701 |
| progc | 39611 | 11522 | 2.327 |
| progl | 71646 | 13886 | 1.551 |
| progp | 49379 | 9512 | 1.541 |
| trans | 93695 | 15738 | 1.344 |

### Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| alice29.txt | 152089 | 39239 | 2.064 |
| asyoulik.txt | 125179 | 36500 | 2.333 |
| cp.html | 24603 | 7045 | 2.291 |
| fields.c | 11150 | 2751 | 1.974 |
| grammar.lsp | 3721 | 1146 | 2.464 |
| kennedy.xls | 1029744 | 58981 | 0.458 |
| lcet10.txt | 426754 | 96489 | 1.809 |
| plrabn12.txt | 481861 | 131455 | 2.182 |
| ptt5 | 513216 | 44960 | 0.701 |
| sum | 38240 | 11634 | 2.434 |
| xargs.1 | 4227 | 1619 | 3.064 |

### Large Canterbury Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| bible.txt | 4047392 | 707595 | 1.399 |
| E.coli | 4638690 | 1138016 | 1.963 |
| world192.txt | 2473400 | 383714 | 1.241 |

### Silesia Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| dickens | 10192446 | 2217969 | 1.741 |
| mozilla | 51220480 | 15783932 | 2.465 |
| mr | 9970564 | 2168743 | 1.740 |
| nci | 33553445 | 1147263 | 0.274 |
| ooffice | 6152192 | 2533659 | 3.295 |
| osdb | 10085684 | 2250926 | 1.785 |
| reymont | 6627202 | 969844 | 1.171 |
| samba | 21606400 | 3867735 | 1.432 |
| sao | 7251944 | 4671964 | 5.154 |
| webster | 41458703 | 6308597 | 1.217 |
| xml | 5345280 | 367777 | 0.550 |
| x-ray | 8474240 | 3698602 | 3.492 |

### Manzini Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| chr22.dna | 34553758 | 7262258 | 1.681 |
| etext99 | 105277340 | 21702753 | 1.649 |
| gcc-3.0.tar | 86630400 | 10262222 | 0.948 |
| howto | 39422105 | 7634423 | 1.549 |
| jdk13c | 69728899 | 2680040 | 0.307 |
| linux-2.4.5.tar | 116254720 | 16698531 | 1.149 |
| rctail96 | 114711151 | 9917087 | 0.692 |
| rfc | 116421901 | 15140037 | 1.040 |
| sprot34.dat | 109617186 | 17470714 | 1.275 |
| w3c2 | 104201579 | 5765329 | 0.443 |

### Maximum Compression Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| A10.jpg | 842468 | 825194 | 7.836 |
| AcroRd32.exe | 3870784 | 1575980 | 3.257 |
| english.dic | 465211 | 148615 | 2.556 |
| FlashMX.pdf | 4526946 | 3732982 | 6.597 |
| FP.LOG | 20617071 | 513540 | 0.199 |
| MSO97.DLL | 3782416 | 1897216 | 4.013 |
| ohs.doc | 4168192 | 814824 | 1.564 |
| rafale.bmp | 4149414 | 750466 | 1.447 |
| vcfiu.hlp | 4121418 | 617241 | 1.198 |
| world95.txt | 2988578 | 451042 | 1.207 |

### Large Text Compression Benchmark Corpus ###
| File name | Input size (bytes) | Output size (bytes) | Bits per symbol |
|:---------------:|:-----------:|:------------:|:-------:|
| enwik8 | 100000000 | 20486072 | 1.639 |
| enwik9 | 1000000000 | 161794295 | 1.294 |
