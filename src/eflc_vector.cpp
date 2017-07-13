/* eflc_vector.cpp
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

#include "eflc_vector.hpp"

using namespace std;

namespace solca_comp{

  void EFLCVector::Init(const uint64_t kReservedCodes, 
			const uint64_t kMaxVal){
    code_bits_ = CFunc::MSB(kMaxVal) + 1;
    num_codes_ = 0;
    reserved_codes_ = kReservedCodes;
    flc_vec_.ResizeAndInit(kReservedCodes,
			   code_bits_);
  }

  void EFLCVector::Resize(const uint64_t kReservedCodes, 
			  const uint64_t kMaxVal){
    uint64_t prev_code_bits = code_bits_;
  
    code_bits_ = CFunc::MSB(kMaxVal) + 1;
    if(code_bits_ == 0){
      code_bits_ = 1;
    }
    reserved_codes_ = kReservedCodes;
    flc_vec_.Resize(kReservedCodes,
		    code_bits_);
    for(size_t i = num_codes_ - 1; i !=  kDummyCode;i--){
      Set(i, Get(i,prev_code_bits));
    }
  }

  void EFLCVector::Resize2(const uint64_t kReservedCodes, 
			   const uint64_t kMaxVal){
    code_bits_ = CFunc::MSB(kMaxVal) + 1;
    reserved_codes_ = kReservedCodes;
    num_codes_ = kReservedCodes;
    flc_vec_.Resize(kReservedCodes,
		    code_bits_);
  }

  void EFLCVector::Clear(){
    num_codes_ = 0;
  }
  
  void EFLCVector::Delete(){
    reserved_codes_ = 0;
    code_bits_ = 0;
    num_codes_ = 0;
    flc_vec_.Delete();
  }

  void EFLCVector::PushBack(const uint64_t kCode){
    flc_vec_.PushBack(num_codes_++,
		      kCode,
		      code_bits_);
  }

  void EFLCVector::Set(const uint64_t kPos, 
		       const uint64_t kCode){
    flc_vec_.Set(kPos,
		 kCode,
		 code_bits_);
  }

  uint64_t EFLCVector::Get(const uint64_t kPos,
			   const uint8_t kCodeBits){
    return flc_vec_.Get(kPos,
			kCodeBits);
  }

  uint64_t EFLCVector::operator[](const uint64_t kPos) const{
    return flc_vec_.Get(kPos,
			code_bits_);
  }

  uint64_t EFLCVector::NumCodes() const{
    return num_codes_;
  }

  uint64_t EFLCVector::ReservedCodes(){
    return reserved_codes_;
  }

  uint8_t EFLCVector::CodeBits(){
    return code_bits_;
  }

  uint64_t EFLCVector::NumReservedBlocks(){
    return ((reserved_codes_ * code_bits_)  >> kLgBlockSize) + 1;
  }

  uint64_t EFLCVector::NumBlocks(){
    return ((num_codes_ * code_bits_) >> kLgBlockSize) + 1;
  }

  uint64_t EFLCVector::ByteSize() const{
    return flc_vec_.ByteSize(reserved_codes_,
			     code_bits_)
      + sizeof(EFLCVector); 
  }

  void EFLCVector::Save(ofstream &ofs){
    ofs.write((char*)&num_codes_, sizeof(num_codes_));
    ofs.write((char*)&reserved_codes_, sizeof(reserved_codes_));
    ofs.write((char*)&code_bits_, sizeof(code_bits_));
    flc_vec_.Save(ofs,
		  num_codes_,
		  code_bits_);
  }

  void EFLCVector::Load(ifstream &ifs){
    ifs.read((char*)&num_codes_, sizeof(num_codes_));
    ifs.read((char*)&reserved_codes_, sizeof(reserved_codes_));
    ifs.read((char*)&code_bits_, sizeof(code_bits_));
    flc_vec_.Load(ifs,
		  num_codes_,
		  code_bits_);
  }

}//namespace solca_comp
