/* flc_vector.cpp
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

#include "flc_vector.hpp"

using namespace std;


void FLCVector::Init(const uint64_t kNumCodes,
		     const uint8_t  kCodeBits){
  if(kNumCodes != 0){
    CFunc::ResizeVec(codes_,
		     NumBlocks(kNumCodes, kCodeBits));
    for(size_t i = 0; i < NumBlocks(kNumCodes, kCodeBits);i++){
      codes_[i] = 0;
    }
  }
  else{
    codes_.clear();
  }
}

void FLCVector::Clear(const uint64_t kNumCodes, 
		      const uint64_t kCodeBits){
  for(size_t i = 0; i < kNumCodes;i++){
    Set(i,
	0,
	kCodeBits);
  }
}

void FLCVector::Delete(){
  std::vector<uint64_t> ().swap(codes_);
}


void FLCVector::Resize(const uint64_t kNumCodes,
		       const uint8_t  kCodeBits){
  CFunc::ResizeVec(codes_,
		   NumBlocks(kNumCodes, kCodeBits));
}

void FLCVector::ResizeAndInit(const uint64_t kNumCodes,
			      const uint8_t  kCodeBits){
  Resize(kNumCodes, kCodeBits);
  for(size_t i = 0; i < NumBlocks(kNumCodes, kCodeBits);i++){
    codes_[i] = 0;
  }
}

uint64_t FLCVector::NumBits(const uint64_t kNumCodes,
			    const uint8_t kCodeBits){
  return NumBlocks(kNumCodes, kCodeBits) * kBlockSize;
}

uint64_t FLCVector::NumBlocks(const uint64_t kNumCodes,
			      const uint8_t kCodeBits) const{
  if(kNumCodes == 0){
    return 0;
  }
  else{
    uint64_t bit_len = kNumCodes*kCodeBits;
    if((bit_len & (kBlockSize - 1)) == 0){      
       return ((kNumCodes * kCodeBits) >> kLgBlockSize);
    }
    else{
       return ((kNumCodes * kCodeBits) >> kLgBlockSize) + 1;
    }
  }
}

uint64_t FLCVector::ByteSize(const uint64_t kNumCodes,
			     const uint8_t kCodeBits) const {
    return sizeof(FLCVector)
      + sizeof(uint64_t)*NumBlocks(kNumCodes, kCodeBits);
}

void FLCVector::Save(ostream &ofs,
		     const uint64_t kNumCodes,
		     const uint8_t kCodeBits){
  ofs.write((char*)&codes_[0],
	    NumBlocks(kNumCodes,kCodeBits)*sizeof(uint64_t));
}

void FLCVector::Load(istream &ifs,
		     const uint64_t kNumCodes,
		     const uint8_t  kCodeBits){
  codes_.resize(NumBlocks(kNumCodes, kCodeBits));
  ifs.read((char*)&codes_[0], 
	   NumBlocks(kNumCodes, kCodeBits) * sizeof(uint64_t));
}

