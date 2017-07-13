/* flc_vector.hpp
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

#ifndef FLC_VECTOR_HPP
#define FLC_VECTOR_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "constant_numbers.hpp"
#include "common_functions.hpp"

namespace solca_comp{
  
  class FLCVector{//fixed length code vector for 1-64 bits.
  private:
    std::vector<uint64_t> codes_;

  public:
    FLCVector() : codes_(){};
    ~FLCVector(){};

    //Operations for the space of codes_.
    void Init(const uint64_t kNumCodes, 
	      const uint8_t  kCodeBits);
    void Clear(const uint64_t kNumCodes, 
	       const uint64_t kCodeBits);
    void Delete();
    void Resize(const uint64_t kNumCodes,
		const uint8_t  kCodeBits);
    void ResizeAndInit(const uint64_t kNumCodes,
		       const uint8_t  kCodeBits);

    //Computation for each space.
    uint64_t NumBits(const uint64_t kNumCodes,
		     const uint8_t kCodeBits);
    uint64_t NumBlocks(const uint64_t kNumCodes,
		       const uint8_t kCodeBits) const;
    uint64_t ByteSize(const uint64_t kNumCodes,
		      const uint8_t kCodeBits) const;

    //Save/load from/to file.
    void Save(std::ostream &ofs,
	      const uint64_t kLength,
	      const uint8_t  kCodeBits);
    void Load(std::istream &ifs,
	      const uint64_t kLength,
	      const uint8_t  kCodeBits);
  
    //setters and getters
    inline void PushBack(const uint64_t kLength,
			 const uint64_t kCode,
			 const uint8_t  kCodeBtis);
    inline void Set(const uint64_t kPos, 
		    const uint64_t kCode,
		    const uint8_t  kCodeBits);
    inline uint64_t Get(const uint64_t kPos,
			const uint64_t kCodeBits) const;
    inline void SetBits(const uint64_t kBitPos, 
			const uint64_t kCode,
			const uint8_t  kCodeBits);
    inline uint64_t GetBits(const uint64_t kBitPos,
			    const uint64_t kCodeBits) const;
    inline void Swap(const uint64_t kPos1,
		     const uint64_t kPos2,
		     const uint8_t kCodeBits);
  }; //class FLCVector

  //inline implementations

  inline void FLCVector::PushBack(const uint64_t kPos,
				  const uint64_t kCode,
				  const uint8_t  kCodeBits){
    uint64_t bit_pos = kPos*kCodeBits;
    uint64_t block_pos = bit_pos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (bit_pos & (uint64_t)(kBlockSize - 1)) - kCodeBits;
    uint64_t bit_num_ones = (1 << kCodeBits) -1;

    if((shift + kCodeBits) == kBlockSize){
      codes_.push_back(0);
    }

    if(shift >= 0){
      codes_[block_pos] &= ~(bit_num_ones << shift);
      codes_[block_pos] |= (kCode << shift); 
    }
    else{
      shift = -shift;
      codes_[block_pos] &= ~(bit_num_ones >> shift);
      codes_[block_pos] |= (kCode >> shift);
      codes_.push_back(0);
      shift = kBlockSize - shift;
      codes_[block_pos + 1] &= ~(bit_num_ones << shift);
      codes_[block_pos + 1] |= (kCode << shift);
    }
  }
  
  inline void FLCVector::Set(const uint64_t kPos, 
			     const uint64_t kCode,
			     const uint8_t  kCodeBits){
    uint64_t bit_pos = kPos*kCodeBits;
    uint64_t block_pos = bit_pos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (bit_pos & (uint64_t)(kBlockSize - 1)) - kCodeBits;
    uint64_t bit_num_ones = (1 << kCodeBits) -1;
    if(shift == 0 && kCodeBits == kBlockSize){
      codes_[block_pos] = kCode;
    }
    else if(shift >= 0){
      codes_[block_pos] &= ~(bit_num_ones << shift);
      codes_[block_pos] |= (kCode << shift); 
    }
    else{
      shift = -shift;
      codes_[block_pos] &= ~(bit_num_ones >> shift);
      codes_[block_pos] |= (kCode >> shift);
      shift = kBlockSize - shift;
      codes_[block_pos + 1] &= ~(bit_num_ones << shift);
      codes_[block_pos + 1] |= (kCode << shift);
    }
  }
  inline void FLCVector::SetBits(const uint64_t kBitPos, 
				 const uint64_t kCode,
				 const uint8_t  kCodeBits){
    uint64_t block_pos = kBitPos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (kBitPos & (uint64_t)(kBlockSize - 1)) - kCodeBits;
    uint64_t bit_num_ones = (1 << kCodeBits) -1;

    if(shift == 0 && kCodeBits == kBlockSize){
      codes_[block_pos] = kCode;
    }
    else if(shift >= 0){
      codes_[block_pos] &= ~(bit_num_ones << shift);
      codes_[block_pos] |= (kCode << shift); 
    }
    else{
      shift = -shift;
      codes_[block_pos] &= ~(bit_num_ones >> shift);
      codes_[block_pos] |= (kCode >> shift);
      shift = kBlockSize - shift;
      codes_[block_pos + 1] &= ~(bit_num_ones << shift);
      codes_[block_pos + 1] |= (kCode << shift);
    }
  }

  inline uint64_t FLCVector::Get(const uint64_t kPos,
				 const uint64_t kCodeBits) const {
    uint64_t bit_pos = kPos*kCodeBits;
    uint64_t block_pos = bit_pos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (bit_pos & (uint64_t)(kBlockSize - 1)) - kCodeBits;
    uint64_t bit_num_ones = (1 << kCodeBits) -1;

    if(shift == 0 && kCodeBits == kBlockSize){
      return codes_[block_pos];
    }
    else if (shift >= 0){
      return (codes_[block_pos] & (bit_num_ones << shift)) >> shift;
    }
    else{
      uint64_t code = 0;
      shift = -shift;
      code = (codes_[block_pos] & (bit_num_ones >> shift)) << shift;
      shift = kBlockSize - shift;
      code += (codes_[block_pos + 1] & (bit_num_ones << shift)) >> shift;
      return code;
    }
  }

  inline uint64_t FLCVector::GetBits(const uint64_t kBitPos,
				     const uint64_t kCodeBits) const {
    uint64_t block_pos = kBitPos >> kLgBlockSize;
    int8_t  shift = kBlockSize - (kBitPos & (uint64_t)(kBlockSize - 1)) - kCodeBits;
    uint64_t bit_num_ones = (1 << kCodeBits) -1;

    if(shift == 0 && kCodeBits == kBlockSize){
      return codes_[block_pos];
    }
    else if(shift >= 0){
      return (codes_[block_pos] & (bit_num_ones << shift)) >> shift;
    }
    else{
      uint64_t code = 0;
      shift = -shift;
      code = (codes_[block_pos] & (bit_num_ones >> shift)) << shift;
      shift = kBlockSize - shift;
      code += (codes_[block_pos + 1] & (bit_num_ones << shift)) >> shift;
      return code;
    }
  }
  
  inline void FLCVector::Swap(const uint64_t kPos1,
			      const uint64_t kPos2,
			      const uint8_t kCodeBits){
    uint64_t tmp_code = Get(kPos1, kCodeBits);
    Set(kPos1,Get(kPos2,kCodeBits), kCodeBits);
    Set(kPos2,tmp_code, kCodeBits);
  }

  //inline implementations
}
#endif //ADYFFLCARRAY_HPP
