# solca
This program is a space-optimal version of a grammar compression called FOLCA[1].  
The detail will be published in ESA2017.  
This program uses [RSDic](https://code.google.com/archive/p/rsdic/) as a part of the data structure.

# download and compile
    $ git clone https://github.com/tkbtkysms/solca.git  
    $ cd solca/src/  
    $ make

# execution
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
