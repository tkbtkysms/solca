/* inner_leaf.cpp
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

#include "inner_leaf.hpp"

using namespace std;

namespace comp {

  void InnerLeaf::Init(const uint64_t kInitBitLen) {
    init_bit_len_ = bit_len_ = kInitBitLen;
    num_elems_ = 0;
    vector<FLCVector>().swap(elems_);
    num_each_bit_elems_.clear();
    num_each_bit_elems_.push_back(0);
  }
  
  uint64_t InnerLeaf::Get(const uint64_t kIndex,
			  const uint64_t kMax) const {
    int max_bit_len =  CFunc::MSB(kMax) + 1;
    uint64_t elems_index  = max_bit_len - init_bit_len_;
    uint64_t offset = kIndex - num_each_bit_elems_[elems_index];
    return elems_[elems_index].Get(offset, max_bit_len);
  }
  
  void InnerLeaf::PushBack(const uint64_t kVal,
			   const uint64_t kMax) {
    int max_bit_len =  CFunc::MSB(kMax) + 1;
    uint64_t elems_index  = max_bit_len - init_bit_len_;
    if (bit_len_ <= max_bit_len) {
      elems_.push_back(FLCVector());
      elems_.back().Init(0, ++bit_len_);
      num_each_bit_elems_.push_back(num_elems_);
    }
    elems_[elems_index].PushBack(num_elems_ - num_each_bit_elems_[elems_index], 
				 kVal,
				 max_bit_len);
    ++num_elems_;
    num_each_bit_elems_[elems_index + 1]++;
  }

  uint64_t InnerLeaf::NumElems() const {
    return num_elems_;
  }

  void InnerLeaf::Clear() {
    vector<FLCVector>().swap(elems_);
    init_bit_len_ = 0;
    bit_len_ = 0;
    num_elems_ = 0;
  }

  uint64_t InnerLeaf::Space() const {
    uint64_t elems_space = 0;
    for (uint64_t i = 0; i != (uint64_t)(bit_len_ - init_bit_len_); ++i) 
      elems_space += elems_[i].ByteSize(num_each_bit_elems_[i+1] - num_each_bit_elems_[i],
					i + init_bit_len_);
    return elems_space + sizeof(InnerLeaf);
  }

  double InnerLeaf::CompRatio() const {
    uint64_t nspace = (bit_len_ * num_elems_ + 7) / 8;
    return static_cast<double>(Space()) / 
      static_cast<double>(nspace) * 100.0;
  }

  void InnerLeaf::Write(std::ostream& os) {
     
    os.write((char*)&init_bit_len_,
	     sizeof(init_bit_len_));
    os.write((char*)&bit_len_,
	     sizeof(bit_len_));
    os.write((char*)&num_elems_,
	     sizeof(num_elems_));
    os.write((char*)&num_each_bit_elems_[0],
	     sizeof(num_each_bit_elems_[0])*(bit_len_ - init_bit_len_ + 1));
    uint64_t pre_num = 0;
    for(uint64_t i = 0; i < (uint64_t)(bit_len_ - init_bit_len_); i++){
      elems_[i].Save(os,
		     num_each_bit_elems_[i + 1] - pre_num,
		     i + init_bit_len_);
      pre_num = num_each_bit_elems_[i + 1];
    }
  }

  void InnerLeaf::Read(std::istream& is) {
    is.read((char*)&init_bit_len_,
	    sizeof(init_bit_len_));
    is.read((char*)&bit_len_,
	    sizeof(bit_len_));
    is.read((char*)&num_elems_,
	    sizeof(num_elems_));
    num_each_bit_elems_.resize(bit_len_ - init_bit_len_ +  1);
    vector<uint64_t> (num_each_bit_elems_).swap(num_each_bit_elems_);
    is.read((char*)&num_each_bit_elems_[0],
	    sizeof(num_each_bit_elems_[0])*(bit_len_ - init_bit_len_ + 1));
    elems_.resize(bit_len_ - init_bit_len_);
    vector<FLCVector> (elems_).swap(elems_);
    uint64_t pre_num = 0;
    for(uint64_t i = 0; i < (uint64_t)(bit_len_ - init_bit_len_); i++){
      elems_[i].Load(is,
		     num_each_bit_elems_[i + 1] - pre_num,
		     i + init_bit_len_);
      pre_num = num_each_bit_elems_[i + 1];
    }
  }

} // namespace comp


