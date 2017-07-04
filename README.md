# solca
This program is a space-optimal version of grammar compression called FOLCA[1].  
The detail will be published in ESA2017.  
This program uses [RSDic](https://code.google.com/archive/p/rsdic/) as a part of the data structure.

# download and compile
    $ git clone https://github.com/tkbtkysms/solca.git  
    $ cd solca/src/  
    $ make

# executions
## compression
    $ ./compress --input_file=string --output_file=string [options] ...   
     options:  
       -i, --input_file     input file name (string)  
       -o, --output_file    output file name (string)  
       -e, --erase_br       erase line break (bool [=0])  
       -?, --help           print this message  

## decompression
    $ ./decompress --compressed_file=string --output_file=string [options] ...   
     options:  
       -i, --compressed_file    compressed file name (string)  
       -o, --output_file        output file name (string)  
       -?, --help               print this message  
  
# references
[1] Shirou Maruyama, Yasuo Tabei, Hiroshi Sakamoto, Kunihiko Sadakane:  
Fully-Online Grammar Compression. SPIRE 2013: 218-229


# additional experiments for [repetitive texts of Pizza & chili corpus](http://pizzachili.dcc.uchile.cl/repcorpus/real/)
We will show the following experiments in the near future.
The following compressors correspond to our ESA2017's paper.

## cere (440MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|532.23|60.45|| 
|SOLCA+CRD|183.66|82.45||
|LZD|26.94|589.76||
|Re-Pair||||

##  core utils (196MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|301.90|39.08||
|SOLCA+CRD|82.98|61.09||
|LZD|11.95|334.04||
|Re-Pair||||

## einstein.de.txt (89MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|103.61|1.36||
|SOLCA+CRD|14.77|23.36||
|LZD|2.07|46.28||
|Re-Pair||||

## einstein.en.txt (446MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|548.43|3.29||
|SOLCA+CRD|78.63|25.29||
|LZD|9.79051|205.06||
|Re-Pair||||
## Escherichia_coli (108MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|173.70|56.58||
|SOLCA+CRD|67.42|78.58||
|LZD|14.72|403.53||
|Re-Pair||||

## influenza (148MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|196.95|22.92||
|SOLCA+CRD|41.84|44.92||
|LZD|8.39|215.49||
|Re-Pair||||

## kernel (247MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|336.59|21.32||
|SOLCA+CRD|102.55|43.32||
|LZD|9.46|257.53||
|Re-Pair||||

## para (410MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|522.65|72.46||
|SOLCA+CRD|196.63|94.46||
|LZD|30.92|693.93||
|Re-Pair||||

## world_leaders (45MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA|60.15|6.54||
|SOLCA+CRD|10.46|28.54||
|LZD|1.60|53.52||
|Re-Pair||||
