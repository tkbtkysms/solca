/* online_bp.hpp
MIT License

Copyright (c) 2017 Tomohiro I

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

#ifndef ONLINE_BP_HPP_
#define ONLINE_BP_HPP_

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "common_functions.hpp"
#include "obp_constants.hpp"
#include "bit_vector.hpp"

//balanced parenthes supporting the append operation.

class OnlineBP{
private:

  size_t n_; //the length of BP
  size_t size_; // the bytes of onlineBP

  size_t N_; // the reserved length of BP
  BitVector BP_;

  std::vector<uint8_t> exA_; // excess array
  // min tree
  std::vector<std::vector<uint8_t> > minA_;// min array of each level
  std::vector<uint64_t> minNextA_;
  std::vector<uint64_t> minPrevA_;
  //outer rank/select
  std::vector<uint8_t> outerRankLA_;
  std::vector<uint64_t> outerRankHA_;
  std::vector<uint64_t> outerSelHA_;
  
  uint32_t last_ex_; // the last value of excess
  uint64_t curOuterRank_; 
  
public:
  OnlineBP(): n_(),
	      size_(),
	      N_(),
	      BP_(),
	      exA_(),
	      minA_(),
	      minNextA_(),
	      minPrevA_(),
	      outerRankLA_(),
	      outerRankHA_(),
	      outerSelHA_(),
	      last_ex_(),
	      curOuterRank_(){}
  ~OnlineBP(){}

  //Initilization and free of space
  void Init();
  void Free();

  //accessors
  size_t Length();
  size_t Size() const; // in bytes
  int Alpha();
  int L();
  uint32_t Get(uint64_t i);
  uint64_t GetBits(uint64_t i,
		   uint32_t d);
  
  //setters
  void Push(const int c);


  //rank/select operations
  uint64_t Rank(uint64_t i,
		int c);
  uint64_t Select(uint64_t i,
		  int c);
  uint64_t OuterRank(uint64_t i);
  uint64_t OuterSelect(uint64_t i);

  //back/forword search
  uint64_t Bwd(const uint64_t i,
	       int32_t d);
  uint64_t Fwd(const uint64_t i,
	       int32_t d);
  
  //Save and Load
  void Save(std::ofstream &ofs);  
  void Load(std::ifstream &ifs);

  
private:
  uint32_t Depth(uint64_t i);
  //rank/select operations for middle/mini blocks
  inline uint64_t FwdSel64(uint64_t x, uint64_t &i);
  uint64_t FwdSel(uint64_t block_pos,
		  uint64_t i);
  uint64_t FwdSel_0(uint64_t block_pos,
		    uint64_t i);
  inline uint64_t FwdCnt(uint64_t block_pos,
			 uint64_t local_pos);
  inline uint64_t FwdSelOuter64(uint64_t x,
				uint64_t y,			     
				uint64_t &i);
  uint64_t FwdSelOuter(uint64_t block_pos,
		       uint64_t i);
  inline uint64_t FwdCntOuter(uint64_t block_pos,
			      uint64_t i);
  
  //sub routines of forword serch and back word search
  uint64_t BwdWords(const uint64_t block_pos,
		    const uint32_t end_pos,
		    const uint32_t num_words,
		    int32_t &d);
  uint64_t FwdWords(uint64_t block_pos,
		    const uint32_t beg_pos,
		    const uint32_t num_words,
		    int32_t &d);
 
  
}; //class Onlinebp


// inline implementations 

inline uint64_t OnlineBP::FwdSel64(uint64_t x,
				   uint64_t &i)
{
  for (int j = 0; j < 8; ++j) {
    const uint_fast8_t w = x >> 56;
    const uint64_t c = kCNT_TBL[w];
    if (i <= c) {
      return j*8 + kSel8Tbl[((i-1)<<8) + w];
    }
    i -= c;
    x <<= 8;
  }
  return 64;
}

inline uint64_t OnlineBP::FwdCnt(uint64_t block_pos,
				 uint64_t local_pos)
{
  uint64_t ret = 0;
  while(local_pos >= kMidBlock.size) {
    ret += CFunc::PopCnt(BP_.GetBlock(block_pos++));
    local_pos -= kMidBlock.size;
  }
  ret += CFunc::PopCnt((BP_.GetBlock(block_pos) >> (kMidBlock.size - local_pos - 1)));
  return ret;
}

inline uint64_t OnlineBP::FwdSelOuter64(uint64_t x,
					uint64_t y,
					uint64_t &i)
{
  for (int j = 0; j < 7; ++j) {
    const uint32_t w = x >> 54;
    const uint64_t c = kOuter8Tbl[w];
    if (i <= c) {
      return j*8 + kOuter8Tbl[(i<<10) + w];
    }
    i -= c;
    x <<= 8;
  }
  const uint32_t w = (x >> 54) | (y >> 62);
  const uint64_t c = kOuter8Tbl[w];
  if (i <= c) {
    return 56 + kOuter8Tbl[(i<<10) + w];
  }
  i -= c;
  return 64;
}


inline uint64_t OnlineBP::FwdCntOuter(uint64_t block_pos,
				      uint64_t i)
{
  uint64_t ret = 0;
  uint64_t x = 0;
  uint32_t w = 0;
  const uint64_t t = i/8 + 1;
  for (uint32_t j = 0; j < t; ++j) {
    if (j % 8 == 0) {
      x = BP_.GetBlock(block_pos++);
      w = x >> 54;
      x <<= 8;
      x |= BP_.GetBlock(block_pos) >> 56;
    } else {
      w = x >> 54;
      x <<= 8;
    }
    if (j == t-1) {
      w |= 0x7f >> (i % 8);
    }
    ret += kOuter8Tbl[w];
  }
  return ret;
}

#endif // ONLINE_BP_HPP_
