/* bit_vector.hpp
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

#include <vector>
#include <fstream>
#include "constant_numbers.hpp"
#include "common_functions.hpp"

namespace solca_comp{

  class BitVector{
  private:
    std::vector<uint64_t> bits_;
    uint64_t num_bits_;
    uint64_t reserved_bits_;
  public:
    BitVector() : bits_(),
		  num_bits_(),
		  reserved_bits_(){}
    ~BitVector(){}

    //initialization and resize
    void Init(const uint64_t kReservedBits);
    void Resize(const uint64_t kReservedBits);
    void Delete();
  
    //setters
    inline void SetBit(const uint64_t kPos,
		       const bool kBit);
    inline void PushBack(const bool kBit);
    //getters
    inline bool GetBit(const uint64_t kPos);
    inline uint64_t GetBits(const uint64_t kPos,
			    const uint8_t kNumBits);
    inline uint64_t GetBlock(const uint64_t kBlockPos);
    inline bool operator[](const uint64_t kPos);

    void Save(std::ofstream &ofs,
	      const uint64_t kNumBits);
  };//class BitVector

  //inline implementations
  inline void BitVector::PushBack(const bool kBit)
  {
    if(num_bits_ == reserved_bits_){
      bits_.push_back(0);
      reserved_bits_ += kBlockSize;
    }
    SetBit(num_bits_++,
	   kBit);
  }

  inline void BitVector::SetBit(const uint64_t kPos,
				const bool kBit)
  {
    uint64_t block_pos = kPos >> kLgBlockSize;
    uint64_t local_pos = kPos & (kBlockSize - 1);
    if(kBit){
      bits_[block_pos] |= (1LLU << (kBlockSize - 1 - local_pos));
    }
    else{
      bits_[block_pos] &= (~(1LLU << (kBlockSize - 1 - local_pos)));
    }
  }

  inline bool BitVector::GetBit(const uint64_t kPos)
  {
    const uint64_t shift = kBlockSize - (kPos & (kBlockSize - 1)) - 1;
    return (bits_[kPos >> kLgBlockSize] >> shift) & 1;
  }

  inline bool BitVector::operator[](const uint64_t kPos){
    return GetBit(kPos);
  }

  inline uint64_t BitVector::GetBits(const uint64_t kPos,
				     const uint8_t kNumBits)
  {
    uint64_t block_pos = kPos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (kPos & (uint64_t)(kBlockSize - 1)) - kNumBits;
    uint64_t bit_num_ones = (1 << kNumBits) -1;

    if(shift >= 0){
      return (bits_[block_pos] & (bit_num_ones << shift)) >> shift;
    }
    else{
      uint64_t bits = 0;
      shift = -shift;
      bits = (bits_[block_pos] & (bit_num_ones >> shift)) << shift;
      shift = kBlockSize - shift;
      bits += (bits_[block_pos + 1] & (bit_num_ones << shift)) >> shift;
      return bits;
    }
  }

  inline uint64_t BitVector::GetBlock(const uint64_t kBlockPos){
    return bits_[kBlockPos];
  }

}//namespace solca_comp
