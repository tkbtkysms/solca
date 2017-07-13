/* eflc_vector.hpp
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

#ifndef EFLC_VECTOR_HPP
#define EFLC_VECTOR_HPP

#include <cmath>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include "flc_vector.hpp"
#include "constant_numbers.hpp"
#include "common_functions.hpp"


namespace solca_comp{
  //extended fixed length code array.
  //FLCVector + additional informations
  //(# of codes, # of reserved codes, bits per code)

  class EFLCVector{      
  private:
    FLCVector flc_vec_;
    uint64_t num_codes_;
    uint64_t reserved_codes_;
    uint8_t code_bits_;

  public:
    EFLCVector() : flc_vec_(),		
		   num_codes_(),
		   reserved_codes_(),
		   code_bits_(){}
    ~EFLCVector(){}

    //initialization/resize/clear/deletion of variables
    void Init(const uint64_t kReservedCodes,
	      const uint64_t kMaxVal);
    void Resize(const uint64_t kReservedCodes,
		const uint64_t kMaxVal);
    void Resize2(const uint64_t kReservedCodes,
		 const uint64_t kMaxVal);
    void Clear();
    void Delete();
  
    //setters and getters
    void PushBack(const uint64_t kCode);
    void Set(const uint64_t kPos, 
	     const uint64_t kCode);
    uint64_t Get(const uint64_t kPos,
		 const uint8_t kCodeBits);
    uint64_t operator[](const uint64_t kPos) const;
    uint64_t NumCodes() const;
    uint64_t ReservedCodes();
    uint8_t CodeBits();
  
    //saving/loading from/to input/output file
    void Save(std::ofstream &ofs);
    void Load(std::ifstream &ifs);

    //computation for each space
    uint64_t NumReservedBlocks();
    uint64_t NumBlocks();
    uint64_t ByteSize() const;

  };// class EFLCVector

}//namespace solca_comp
#endif //EFLC_VECTOR_HPP
