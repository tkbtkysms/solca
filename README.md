# solca
This program is a space-optimal version of grammar compression called FOLCA [1].  
The detail is showed in [A Space-Optimal Grammar Compression](http://drops.dagstuhl.de/opus/volltexte/2017/7864/) [2].  
This program uses [RSDic](https://code.google.com/archive/p/rsdic/) as a part of the data structure.

# download and compile
    $ git clone https://github.com/tkbtkysms/solca.git  
    $ cd solca/src/  
    $ make

# executions
## compression
```
$ ./compress --input_file=string --output_file=string [options] ... 
options:
  -i, --input_file        input file name (string)
  -o, --output_file       output file name (string)
  -n, --naive_encoding    output the naive encoding of solca's grammar (bool [=0])
  -e, --erase_br          erase line break (bool [=0])
  -p, --print_logs        print logs (bool [=0])
  -?, --help              print this message
```
 
## decompression
```
$ ./decompress --compressed_file=string --output_file=string [options] ... 
options:
  -i, --compressed_file    compressed file name (string)
  -o, --output_file        output file name (string)
  -n, --naive_encoding     decompress from the naive encoding of solca's grammar (bool [=0])
  -?, --help               print this message
```
# execution examples
## build from input_file the SOLCA's grammar in succinct space and output the succinct enconding of the SOLCA's grmmar (compressed file)
```
$ ./compress -i input_file -o compressed_file
```
## decompress from the succinct encoding of SOLCA's grammar (compressed_file)
```
$ ./decompress -i compressd_file -o decompressed_file
```
## build from input_file the SOLCA's grammar in succinct space and output the naive encoding of the SOLCA's grammar
The naive encoding is a standard 4 byte integer array storing X_{l_1}X_{r_1} ... X_{l_n}X_{r_n} where X_{l_i}  and X_{r_i} is the variable representing of righthand size of X_i -> X_{l_i}X_{r_i} and n is the number of non-terminals.
```
$ ./compress -i input_file -o compressed_file -n 1
```
## decompress from the naive representation of SOLCA's grammar
```
$ ../decompress -i compressed_file -o decompressed_file -n 1
```
# experiments for [repetitive texts of Pizza & chili corpus](http://pizzachili.dcc.uchile.cl/repcorpus/real/)
## comparison methods
FOLCA: [1].  
FOLCA+: FOLCA applying SOLCA's dynamic succinct tree.  
SOLCA: Seciton 3.3 of [2].  
SOLCA+CRD: Section 4 of [2] ← this implementation.   
LZD: the patricia tree space construction of https://github.com/kg86/lzd.  
Re-Pair: https://github.com/nicolaprezza/Re-Pair.  

## experimental environment
OS: CentOS 6.9  
CPU: Intel(R) Xeon(R) CPU E7-8837 @2.67GHz  
Memory: 1TB  
Compiler: gcc 6.2.1  

## eavaluation measure
CT: compression time  
WS: working space  
CR: compression (compressed size/ input size * 100)

## cere (440MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|428.65|168.24|3.82|
|FOLCA+|303.88|168.19|3.82|
|SOLCA|532.23|60.45|3.82| 
|SOLCA+CRD|183.66|82.45|3.82|
|LZD|26.94|589.76|5.92|
|Re-Pair|4452.92|2835.01|1.93|

##  core utils (196MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|213.56|140.96|6.04|
|FOLCA+|152.05|140.92|6.04|
|SOLCA|301.90|39.08|6.04|
|SOLCA+CRD|82.98|61.09|6.04|
|LZD|11.95|334.04|8.96|
|Re-Pair|1798.69|1175.77|2.77|

## einstein.de.txt (89MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|84.33|4.68|0.38|
|FOLCA+|59.51|4.68|0.38|
|SOLCA|103.61|1.36|0.38|
|SOLCA+CRD|14.77|23.36|0.38|
|LZD|2.07|46.28|0.66|
|Re-Pair|234.21|464.48|0.19|

## einstein.en.txt (446MB) 

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|428.11|10.60|0.20|
|FOLCA+|305.27|10.60|0.20|
|SOLCA|548.43|3.29|0.20|
|SOLCA+CRD|78.63|25.29|0.20|
|LZD|9.79|205.06|0.28|
|Re-Pair|1589.91|2417.14|0.10|

## Escherichia_coli (108MB) 

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|114.68|159.63|14.14|
|FOLCA+|81.31|159.58|14.14|
|SOLCA|173.70|56.58|14.14|
|SOLCA+CRD|67.42|78.58|14.14|
|LZD|14.72|403.53|20.98|
|Re-Pair|1209.61|701.31|8.37|

## influenza (148MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|140.62|83.66|5.44|
|FOLCA+|97.57|83.64|5.44|
|SOLCA|196.95|22.92|5.44|
|SOLCA+CRD|41.84|44.92|5.44|
|LZD|8.39|215.49|6.47|
|Re-Pair|578.32|960.61|2.67|

## kernel (247MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|265.48|73.79|2.55|
|FOLCA+|187.72|73.76|2.55|
|SOLCA|336.59|21.32|2.55|
|SOLCA+CRD|102.55|43.32|2.55|
|LZD|9.46|257.53|4.20|
|Re-Pair|1823.79|1374.12|1.23|

## para (410MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|419.36|192.01|5.21|
|FOLCA+|289.32|191.97|5.21|
|SOLCA|522.65|72.46|5.21|
|SOLCA+CRD|196.63|94.46|5.21|
|LZD|30.92|693.93|8.19|
|Re-Pair|5713.95|2639.39|2.74|

## world_leaders (45MB)

|compressor|CT (sec)|WS (MB)|CR (%)|
|---|---|---|---|
|FOLCA|41.16|21.24|4.26|
|FOLCA+|29.69|21.23|4.26|
|SOLCA|60.15|6.54|4.26|
|SOLCA+CRD|10.46|28.54|4.26|
|LZD|1.60|53.52|5.41|
|Re-Pair|103.0４|242.38|1.71|

# references
[1] Shirou Maruyama, Yasuo Tabei, Hiroshi Sakamoto, Kunihiko Sadakane:  
Fully-Online Grammar Compression. SPIRE 2013, 218-229.  

[2] Yoshimasa Takabatake, Tomohiro I, Hiroshi Sakamoto:  
A Space-Optimal Grammar Compression. ESA 2017: 67:1-67:15  

