/* bit_vector.cpp
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

#include "bit_vector.hpp"

using namespace std;

void BitVector::Init(const uint64_t kReservedBits){

  uint64_t num_blocks = (kReservedBits >> kLgBlockSize);;
  if((kReservedBits & (kBlockSize - 1))){
    num_blocks++;
  }
  CFunc::ResizeVec(bits_,
		   num_blocks);
  reserved_bits_ = num_blocks * kBlockSize;
  num_bits_ = 0;
}

void BitVector::Resize(const uint64_t kReservedBits){
  uint64_t num_blocks = (kReservedBits >> kLgBlockSize);;
  if((kReservedBits & (kBlockSize - 1))){
    num_blocks++;
  }
  CFunc::ResizeVec(bits_,
		   num_blocks);
  reserved_bits_ = num_blocks * kBlockSize;
}

void BitVector::Delete(){
  vector<uint64_t> ().swap(bits_);
  num_bits_ = 0;
  reserved_bits_ = 0;
}

void BitVector::Save(ofstream &ofs,
		     const uint64_t kNumBits){
  uint64_t num_blocks = (kNumBits >> kLgBlockSize);;
  if((kNumBits & (kBlockSize - 1))){
    num_blocks++;
  }
  ofs.write((char*)&bits_[0], sizeof(bits_[0]) * num_blocks);
}
