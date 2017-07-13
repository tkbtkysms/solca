/* common_functions.hpp
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


#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream> 
#include "constant_numbers.hpp"
#ifdef __SSE4_2__
#include <xmmintrin.h>
#endif

namespace solca_comp{

  class CFunc{
  public:
    template <typename T>
    inline 
    static
    void ResizeVec(std::vector<T> &vec,
		   const uint64_t kLen) {
      vec.resize(kLen,T());
      std::vector<T> (vec).swap(vec); 
    }


    inline static std::pair<uint64_t, uint64_t >GetLargerPrime(const uint64_t kNum){
      for(uint64_t primes_index = 0; primes_index < kPrimeSize; ++primes_index){
	if(kPrimes[primes_index] >= kNum){
	  return std::make_pair(primes_index, kPrimes[primes_index]);
	}
      }
      return std::make_pair(kPrimeSize - 1, kPrimes[kPrimeSize - 1]);
    }
  
    inline static uint64_t ComputeHashVal(const uint64_t kHashSize,
					  const uint64_t kVar) {
      return kVar % kHashSize;
    }
  
    inline static uint32_t MSB(uint64_t bits){
      if(bits == 0){
	return 0;
      }
#ifdef __SSE4_2__
      return 63 - __builtin_clzll(bits);
#else
      uint64_t tmp1, tmp2;
      if((tmp1 = (bits >> 32))){ // msb >= 32
	if((tmp2 = (tmp1 >> 16))){// msb >= 48
	  if((tmp1 = (tmp2 >> 8))){ // msb >= 56 
	    return kMSB_TBL[tmp1] + 56;
	  }
	  else{ // 48 <= msb < 56
	    return kMSB_TBL[tmp2] + 48;
	  }
	}
	else{// 32 <= msb < 48
	  if((tmp2 = (tmp1 >> 8))){ //40 <= msb < 48
	    return kMSB_TBL[tmp2] + 40;
	  }
	  else{//32 <= msb < 40
	    return kMSB_TBL[tmp1] + 32;
	  }
	}
      }
      else{ // msb < 32
	if((tmp1 = (bits >> 16))){ // 16 <= msb < 32
	  if((tmp2 = (tmp1 >> 8))){ // 24 <= msb < 32
	    return kMSB_TBL[tmp2] + 24;
	  }
	  else{ //16 <= msb < 24
	    return kMSB_TBL[tmp1] + 16;
	  }
	}
	else{ // msb < 16
	  if((tmp1 = (bits >> 8))){ // 8 <= msb < 16
	    return kMSB_TBL[tmp1] + 8;
	  }
	  else{ //0 <= msb < 8 
	    return kMSB_TBL[bits];
	  }
	}
      }
#endif
    }
  
    inline static uint32_t LSB(uint64_t bits){
      if(bits == 0){
	return 0;
      }
#ifdef __SSE4_2__
      return __builtin_ctzll(bits);
#else
      return kLSB_TBL[((bits& -bits)*kdeBruijnNum64) >> 58];
#endif
    }
  
    inline static uint64_t PopCnt(uint64_t bits){
#ifdef __SSE4_2__
      return __builtin_popcountll(bits);
#else
      bits = bits - (bits >> 1 & 0x5555555555555555ull);
      bits = (bits & 0x3333333333333333ull) + (bits >> 2 & 0x3333333333333333ull);
      bits = (bits + (bits >> 4)) & 0x0f0f0f0f0f0f0f0full;
      bits = bits * 0x0101010101010101ull;
      return bits >> 56;
#endif
    }
  
    inline static uint64_t ComputeBPD(const uint64_t kMaxVal) {
      return MSB(kMaxVal) + 1;
    }
  };

}
#endif //COMMON_FUNCTIONS_HPP
