/* constant_numbers.hpp
MIT License

Copyright (c) 2017 Yoshimasa Takabatake

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CONSTANT_NUMBERS_HPP
#define CONSTANT_NUMBERS_HPP

#include <vector>
#include <cstdint>
#include <cmath>

namespace solca_comp{

  static const uint64_t kDummyCode =  0xFFFFFFFFFFFFFFFFULL;

  static const uint64_t kAlphabetSize = 256;

  static const uint64_t kOne = 0x0000000000000001ULL;

  static const bool kOP = true;

  static const bool kCP = false;

  static const uint64_t kPrimeSize = 28;

  static const uint64_t kBlockSize = 64;

  static const uint64_t kLgBlockSize = 6;

  static const uint64_t kInitADYGMRLength = 10;

  static const uint64_t kdeBruijnNum64 = 0x03F566ED27179461UL;

  static const uint64_t kPrimes[] = {
    /* 0*/  8 + 3,
    /* 1*/  16 + 3,
    /* 2*/  32 + 5,
    /* 3*/  64 + 3,
    /* 4*/  128 + 3,
    /* 5*/  256 + 27,
    /* 6*/  512 + 9,
    /* 7*/  1024 + 9,
    /* 8*/  2048 + 5,
    /* 9*/  4096 + 3,
    /*10*/  8192 + 27,
    /*11*/  16384 + 43,
    /*12*/  32768 + 3,
    /*13*/  65536 + 45,
    /*14*/  131072 + 29,
    /*15*/  262144 + 3,
    /*16*/  524288 + 21, 
    /*17*/  1048576 + 7,
    /*18*/  2097152 + 17,
    /*19*/  4194304 + 15,
    /*20*/  8388608 + 9,
    /*21*/  16777216 + 43,
    /*22*/  33554432 + 35,
    /*23*/  67108864 + 15,
    /*24*/  134217728 + 29,
    /*25*/  268435456 + 3,
    /*26*/  536870912 + 11,
    /*27*/  1073741824 + 85,
  };

  static const uint8_t kCNT_TBL[256] =
    {
      0, 1, 1, 2, 1, 2, 2, 3,
      1, 2, 2, 3, 2, 3, 3, 4,
      1, 2, 2, 3, 2, 3, 3, 4,
      2, 3, 3, 4, 3, 4, 4, 5,
      1, 2, 2, 3, 2, 3, 3, 4,
      2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      1, 2, 2, 3, 2, 3, 3, 4,
      2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6,
      4, 5, 5, 6, 5, 6, 6, 7,
      1, 2, 2, 3, 2, 3, 3, 4,
      2, 3, 3, 4, 3, 4, 4, 5,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6,
      4, 5, 5, 6, 5, 6, 6, 7,
      2, 3, 3, 4, 3, 4, 4, 5,
      3, 4, 4, 5, 4, 5, 5, 6,
      3, 4, 4, 5, 4, 5, 5, 6,
      4, 5, 5, 6, 5, 6, 6, 7,
      3, 4, 4, 5, 4, 5, 5, 6,
      4, 5, 5, 6, 5, 6, 6, 7,
      4, 5, 5, 6, 5, 6, 6, 7,
      5, 6, 6, 7, 6, 7, 7, 8
    };

  static const uint8_t kLSB_TBL[64] =
    { 0, 1,59, 2,60,40,54, 3,
      61,32,49,41,55,19,35, 4,
      62,52,30,33,50,12,14,42,
      56,16,27,20,36,23,44, 5,
      63,58,39,53,31,48,18,34,
      51,29,11,13,15,26,22,43,
      57,38,47,17,28,10,25,21,
      37,46, 9,24,45, 8, 7, 6
    };

  static const uint8_t kMSB_TBL[256] =
    { 0, 0, 1, 1, 2, 2, 2, 2,
      3, 3, 3, 3, 3, 3, 3, 3,
      4, 4, 4, 4, 4, 4, 4, 4,
      4, 4, 4, 4, 4, 4, 4, 4,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7
    };

}
#endif //CONSTANT_NUMBERS_HPP
