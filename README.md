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
We will show the following experiments in near future.
The following compressors correspond to our ESA2017's paper.

## cere (440MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

##  core utils (196MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## einstein.de.txt (89MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## einstein.en.txt (446MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||
## Escherichia_coli (108MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## influenza (148MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## kernel (247MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## para (410MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||

## world_leaders (45MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA||||
|FOLCA+||||
|SOLCA||||
|SOLCA+CRD||||
|LZD||||
|Re-Pair||||
