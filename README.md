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
|FOLCA|||3.82|
|FOLCA+|302.426|235.78|3.82|
|SOLCA|532.23|60.45|3.82| 
|SOLCA+CRD|183.66|82.45|3.82|
|LZD|26.94|589.76|5.92|
|Re-Pair||||

##  core utils (196MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||6.04|
|FOLCA+|152.09|206.46|6.04|
|SOLCA|301.90|39.08|6.04|
|SOLCA+CRD|82.98|61.09|6.04|
|LZD|11.95|334.04|8.96|
|Re-Pair||||

## einstein.de.txt (89MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||0.38|
|FOLCA+|57.48|6.75|0.38|
|SOLCA|103.61|1.36|0.38|
|SOLCA+CRD|14.77|23.36|0.38|
|LZD|2.07|46.28|0.66|
|Re-Pair||||

## einstein.en.txt (446MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||0.20|
|FOLCA+|295.61|14.85|0.20|
|SOLCA|548.43|3.29|0.20|
|SOLCA+CRD|78.63|25.29|0.20|
|LZD|9.79051|205.06|0.28|
|Re-Pair||||
## Escherichia_coli (108MB) 

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||14.14|
|FOLCA+|79.28|226.43|14.14|
|SOLCA|173.70|56.58|14.14|
|SOLCA+CRD|67.42|78.58|14.14|
|LZD|14.72|403.53|20.98|
|Re-Pair||||

## influenza (148MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||5.44|
|FOLCA+|96.77|117.49|5.44|
|SOLCA|196.95|22.92|5.44|
|SOLCA+CRD|41.84|44.92|5.44|
|LZD|8.39|215.49|6.47|
|Re-Pair||||

## kernel (247MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||2.55|
|FOLCA+|185.50|106.76|2.55|
|SOLCA|336.59|21.32|2.55|
|SOLCA+CRD|102.55|43.32|2.55|
|LZD|9.46|257.53|4.20|
|Re-Pair||||

## para (410MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||5.21|
|FOLCA+|286.90|261.97|5.21|
|SOLCA|522.65|72.46|5.21|
|SOLCA+CRD|196.63|94.46|5.21|
|LZD|30.92|693.93|8.19|
|Re-Pair||||

## world_leaders (45MB)

|compressor|compression time (sec)| memory consumption (MB)|compression ratio|
|---|---|---|---|
|FOLCA|||4.26|
|FOLCA+|28.51|29.74|4.26|
|SOLCA|60.15|6.54|4.26|
|SOLCA+CRD|10.46|28.54|4.26|
|LZD|1.60|53.52|5.41|
|Re-Pair||||
