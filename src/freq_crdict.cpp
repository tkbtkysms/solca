/* freq_crdict.cpp
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

#include "freq_crdict.hpp"

using namespace std;

void FreqCRDict::Init(){

  num_frules_ = 0;
  insert_ht_pos_ = 0;
  num_not_used_ = hash_size_;
  num_bucket_ = 1;
  
  CFunc::ResizeVec(var_, hash_size_);
  CFunc::ResizeVec(left_, hash_size_);
  CFunc::ResizeVec(right_, hash_size_);
  CFunc::ResizeVec(freq_, hash_size_);
  CFunc::ResizeVec(hash_table_, hash_size_);
  CFunc::ResizeVec(next_, hash_size_);
  CFunc::ResizeVec(not_used_, hash_size_);

  for(size_t i = 0; i < hash_size_; i++){
    var_[i] = kDummyCode;
    left_[i] = kDummyCode;
    right_[i] = kDummyCode;
    freq_[i] = 0;
    hash_table_[i] = hash_size_;
    next_[i] = hash_size_;
    not_used_[i] = hash_size_ - i - 1;
  }

}

uint64_t FreqCRDict::GetVar(const uint64_t kLeft,
			    const uint64_t kRight){
  
  insert_ht_pos_ = CFunc::ComputeHashVal(hash_size_,(kLeft + 3)*kRight);
  uint64_t pos = hash_table_[insert_ht_pos_];
  
  while(pos != hash_size_){
    if(left_[pos] == kLeft 
       && right_[pos] == kRight){
      freq_[pos]++;
      return var_[pos];
    }
    pos = next_[pos];
  }

  return kDummyCode;
}

void FreqCRDict::PrintCRDict(){
  
  for(size_t i = 0; i < hash_size_; i++){
    std::cout << i << " " << var_[i] << " " << left_[i] << " " << right_[i] << " " << next_[i] << endl;
  }
  
  cout << "hash" << endl;
  for(size_t i = 0; i < hash_size_; i++){
    cout << i << " " << hash_table_[i] << endl;
  }
}


void FreqCRDict::AddRule(const uint64_t kVar,
			 const uint64_t kLeft,
			 const uint64_t kRight){

  uint32_t insert_pos = not_used_[--num_not_used_];
  InsertToHash(insert_pos);

  var_[insert_pos] = kVar;
  left_[insert_pos] = kLeft;
  right_[insert_pos] = kRight;
  freq_[insert_pos] = 1;
  
  num_frules_++;
 
  if(num_frules_ == hash_size_){
    DeleteNotFreqRules();
    num_bucket_++;
  }
}

void FreqCRDict::InsertToHash(const uint64_t kInsertPos){
  uint64_t tmp = hash_table_[insert_ht_pos_];
  hash_table_[insert_ht_pos_] = kInsertPos;
  next_[kInsertPos] = tmp;
}

void FreqCRDict::DeleteNotFreqRules(){
  while(num_frules_ == hash_size_){
    for(size_t i = 0; i < hash_size_; i++){
      if(--freq_[i] == 0){
	DeleteFromHash(i);
	not_used_[num_not_used_++] = i;
	num_frules_--;
      }
    }
  }
}

void FreqCRDict::DeleteFromHash(const uint64_t kPos){
  
  uint64_t hash_pos = CFunc::ComputeHashVal(hash_size_,
					    (left_[kPos] + 3)*right_[kPos]);

  uint64_t pos = hash_table_[hash_pos];
  if(pos == kPos){
    hash_table_[hash_pos] = next_[pos];
    next_[pos] = hash_size_;
    var_[pos] = kDummyCode;
    left_[pos] = kDummyCode;
    right_[pos] = kDummyCode;
  }
  else{
    uint64_t prev_pos = pos;
    pos = next_[pos];
    while(pos != hash_size_){
      if(pos == kPos){
	next_[prev_pos] = next_[pos];
	next_[pos] = hash_size_;
	var_[pos] = kDummyCode;
	left_[pos] = kDummyCode;
	right_[pos] = kDummyCode;
	break;
      }
      prev_pos = pos;
      pos = next_[pos];
    }
  }
}

uint64_t FreqCRDict::ByteSize() const{
  return sizeof(FreqCRDict) + 
    4*hash_size_*sizeof(uint64_t) +
    3*hash_size_*sizeof(uint32_t);
}
