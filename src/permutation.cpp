/* permutation.cpp
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
 
#include "permutation.hpp"

using namespace std;
using namespace rsdic;

void Permutation::Clear(){
  elems_.Clear();
  has_skip_elem_.Clear();
  skip_list_.Clear();
  hash_size_ = 0;
  hash_table_.Clear();
  hash_list_.Clear();
  tmp_index_.Clear();
  tmp_skip_list_.Clear();
}

void Permutation::Build(EFLCVector &elem){
  elems_ = elem;
  ReIndex();
}

void Permutation::Resize(const uint64_t kLength){
  elems_.Resize2(kLength,kLength);
}

void Permutation::SetElem(const uint64_t kIndex,
			  const uint64_t kVar){
  elems_.Set(kIndex, kVar);
}

uint64_t Permutation::GetElem(const uint64_t kIndex){
  return elems_[kIndex];
}

uint64_t Permutation::GetPreElem(const uint64_t kIndex){
  return elems_.Get(kIndex,pre_bpd_);
}

void Permutation::SetPreBPD(){
  pre_bpd_ = elems_.CodeBits();
}

void Permutation::ReIndex(){
  InitHash();
  has_skip_elem_.Clear();
  skip_list_.Clear();
  uint64_t length = elems_.ReservedCodes();
  uint64_t skip_length = CFunc::MSB(CFunc::MSB(length) + 1) + 1;
 
  vector<bool> is_check_elem;
  is_check_elem.resize(length);
  vector<bool> (is_check_elem).swap(is_check_elem);

  for(size_t i = 0;i < length; i++){
    is_check_elem[i] = false;
  }
  
  uint64_t skip_list_num = 0;
  for(size_t i = 0; i < length; i++){
    if(!is_check_elem[i]){
      uint64_t pos = i;
      uint64_t loop_length = 0;
      uint64_t skip_pos = i;
      uint64_t local_skip_num = 0;
      
      while(elems_[pos] != i){
	is_check_elem[pos] = true; 
	loop_length++;
	if(loop_length == skip_length){
	  loop_length = 0;
	  skip_list_num++;
	  local_skip_num++;
	  SetHash(pos, skip_pos);
	  skip_pos = pos;
	}
	pos = elems_[pos];
      }
      is_check_elem[pos] = true; 
      if(local_skip_num > 0){
	skip_list_num++;
        SetHash(i, skip_pos);
      }
    }   
  }
  skip_list_.Init(skip_list_num, length);
  
  uint64_t tmp1;
  has_skip_elem_.Clear();
  for(size_t i = 0; i < length; i++){
    if((tmp1 = GetHash(i)) != kDummyCode ){
      has_skip_elem_.PushBack(true);
 
      skip_list_.PushBack(tmp1);
    }
    else{
      has_skip_elem_.PushBack(false);
    }
  }
  has_skip_elem_.LastProcess();
  DeleteHash();
  vector<bool> ().swap(is_check_elem);
}

void Permutation::DeleteHash(){
  hash_table_.Delete();
  hash_list_.Delete();
  tmp_index_.Delete();
  tmp_skip_list_.Delete();
  hash_size_ = 0;
}

void Permutation::InitHash(){
  uint64_t length = elems_.ReservedCodes();
  uint64_t max_num_elem = length/(CFunc::MSB(CFunc::MSB(length) +1) + 1) + 1;
  
  for(size_t i = 0; i < kPrimeSize; i++){
    if(kPrimes[i] >= (max_num_elem << 1)){
      hash_size_ = kPrimes[i];
      break;
    }
    else if(i == kPrimeSize - 1){
      hash_size_ = kPrimes[i];
    }
  }
  hash_table_.Init(hash_size_, (max_num_elem << 1));
  hash_list_.Init(hash_size_, (max_num_elem << 1));
  tmp_index_.Init(max_num_elem, length);
  tmp_skip_list_.Init(max_num_elem, length); 
}

void Permutation::AddTable(const uint64_t kHashVal,
			   const uint64_t kIndex,
			   const uint64_t kSkipPos){
  tmp_index_.PushBack(kIndex);
  tmp_skip_list_.PushBack(kSkipPos);
  hash_table_.Set(kHashVal,tmp_index_.NumCodes());
}

void Permutation::AddList(const uint64_t kHashVal,
			  const uint64_t kIndex,
			  const uint64_t kSkipPos){
  tmp_index_.PushBack(kIndex);
  tmp_skip_list_.PushBack(kSkipPos);
  hash_list_.Set(kHashVal,tmp_index_.NumCodes());
}


void Permutation::SetHash(const uint64_t kIndex,
			  const uint64_t kSkipPos){
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, kIndex);
  uint64_t key1 = hash_table_[hash_val];
  if(key1 == 0){
    AddTable(hash_val, 
	     kIndex,
	     kSkipPos);
  }
  else{
    uint64_t tmp = 0;
    while((tmp = hash_list_[key1 - 1]) != 0){
      key1 = tmp;
    }
    AddList(key1 - 1,
	    kIndex,
	    kSkipPos);
  }
}

uint64_t Permutation::GetHash(const uint64_t kIndex){
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_,kIndex);
  uint64_t key1 = hash_table_[hash_val];
  if(key1 == 0){
    return kDummyCode;
  }
  else if (tmp_index_[key1 - 1] == kIndex){
    return tmp_skip_list_[key1 - 1];
  }
  else{
    uint64_t tmp;
    while((tmp = hash_list_[key1 - 1]) != 0){
      if(tmp_index_[tmp - 1] == kIndex){
	return tmp_skip_list_[tmp -1];
      }
      key1 = tmp;
    }
    return kDummyCode;
  }
}

uint64_t Permutation::Access(const uint64_t kPos){
  return elems_[kPos];
}

uint64_t Permutation::ReverseAccess(const uint64_t kPos){

  uint64_t pos = kPos;
  
  while(elems_[pos] != kPos){
    if(has_skip_elem_.GetBit(pos) ){
      pos = skip_list_[has_skip_elem_.Rank(pos + 1, 1) - 1];
      while(elems_[pos] != kPos){
	pos = elems_[pos];
      }
      break;
    }
    else{
      pos = elems_[pos];
    }
  }

  return pos;
}

uint64_t Permutation::Length(){
  return elems_.NumCodes();
}


void Permutation::Save(ofstream &ofs){
  elems_.Save(ofs);
  has_skip_elem_.Save(ofs);
  skip_list_.Save(ofs);
  ofs.write((char*)&pre_bpd_,  sizeof(pre_bpd_));
}

void Permutation::Load(ifstream &ifs){
  elems_.Load(ifs);
  has_skip_elem_.Load(ifs);
  skip_list_.Load(ifs);
  ifs.read((char*)&pre_bpd_, sizeof(pre_bpd_));
}

uint64_t Permutation::ByteSize() const{
  return elems_.ByteSize() 
    + skip_list_.ByteSize()
    + has_skip_elem_.GetUsageBytes()
    + hash_table_.ByteSize()
    + hash_list_.ByteSize()
    + tmp_index_.ByteSize()
    + tmp_skip_list_.ByteSize()
    + sizeof(Permutation);
}
