/* decomp_suc_poslp.cpp
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

#include "decomp_suc_poslp.hpp"

using namespace std;

namespace solca_comp {

  uint64_t DSucPOSLP::Left(const uint64_t kVar){
    if(kVar < kAlphabetSize){
      return kVar;
    }
    const uint64_t kSFBTPos = sfbt_.LeftChild(sfbt_.InSelect(Var2SFBTVar(kVar) + 1));

    if (sfbt_.IsLeaf(kSFBTPos)) {
      return leave_.Get(sfbt_.LeafRank(kSFBTPos) - 1,
			sfbt_.InRank(kSFBTPos) + kAlphabetSize);
    }
    return SFBTVar2Var(sfbt_.InRank(kSFBTPos) - 1);
  }

  uint64_t DSucPOSLP::Right(const uint64_t kVar){
    if(kVar < kAlphabetSize){
      return kVar;
    }
    const uint64_t kSFBTPos = sfbt_.RightChild(sfbt_.InSelect(Var2SFBTVar(kVar) + 1));
    if (sfbt_.IsLeaf(kSFBTPos)) {
      return leave_.Get(sfbt_.LeafRank(kSFBTPos) - 1,
			sfbt_.InRank(kSFBTPos) + kAlphabetSize);
    }
    return SFBTVar2Var(sfbt_.InRank(kSFBTPos) - 1);
  }

  void DSucPOSLP::RepeatDecompress(const uint64_t kVar,
				   ofstream &ofs){
    if(kVar < kAlphabetSize){
      ofs << (char)kVar;
    }
    else{
      RepeatDecompress(Left(kVar),
		       ofs);
      RepeatDecompress(Right(kVar),
		       ofs);
    }
  }

  void DSucPOSLP::Load(ifstream &ifs){
    sfbt_.Load(ifs);
    FLCVector input;
    FLCVector tmp_input;
    uint64_t bit_pos = 0;
    uint64_t leaf_bits = 9;
    uint64_t max_code = 1 << leaf_bits;
    uint64_t tmp_leaf = 0;
    uint64_t leaf_rank = 0;
    uint64_t inner_rank = kAlphabetSize;
    leave_.Init(9);
    
    input.Load(ifs, 1, kBlockSize);
    input.Resize(2, kBlockSize);
    for(uint64_t i = 0; i < sfbt_.Length(); i++){
      if(sfbt_.Get(i) == kOP){
	if((bit_pos + leaf_bits) >= kBlockSize){
	  tmp_input.Load(ifs, 1, kBlockSize);
	  input.SetBits(kBlockSize, tmp_input.GetBits(0,kBlockSize), kBlockSize);
	  tmp_leaf = input.GetBits(bit_pos, leaf_bits);
	  input.SetBits(0, tmp_input.GetBits(0, kBlockSize), kBlockSize);
	  tmp_input.SetBits(0,0,kBlockSize);
	  bit_pos = (bit_pos + leaf_bits) - kBlockSize;
	}
	else{
	  tmp_leaf = input.GetBits(bit_pos, leaf_bits);
	  bit_pos += leaf_bits;
	}
	leave_.PushBack(tmp_leaf, inner_rank);
	leaf_rank++;
      }
      else{
	inner_rank++;
	if(inner_rank >= max_code){
	  leaf_bits++;
	  max_code = 1 << leaf_bits;
	}
      }
    }
    for(uint64_t i = kAlphabetSize; i < sfbt_.InRank(sfbt_.Length() - 1) + kAlphabetSize - 1;i++){

    }
  }

  uint64_t DSucPOSLP::Decompress(const string& kInputFileName,
				 const string& kOutputFileName){
    ifstream ifs(kInputFileName.c_str());
    ofstream ofs(kOutputFileName.c_str());
  
    Load(ifs);
    ifs.close();
    RepeatDecompress(sfbt_.InRank(sfbt_.Length() - 1) + kAlphabetSize  - 2,
		     ofs);
    ofs.close();
    return 1;
  }

}//solca_comp
