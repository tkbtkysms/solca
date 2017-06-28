/* nrs_dic.cpp
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

#include "nrs_dic.hpp"


using namespace std;

void NRSDic::Init(const uint64_t kLength){
  avail_max_var_ = 512;
  max_var_ = 0; 
  data_.Init(kLength, avail_max_var_); 
  for(size_t i = 0; i < kPrimeSize; i++){
    if(kPrimes[i] >= kLength){
      primes_index_ = i;
      hash_size_ = kPrimes[i];
      break;
    }
    else if(i == kPrimeSize - 1){
      primes_index_ = i;
      hash_size_ = kPrimes[i];
    }
  }
  bits_per_hash_ = CFunc::MSB(kLength) + 1;
  hash_table_.Init(hash_size_,kLength);
  hash_list_.Init(hash_size_,kLength);
  num_var_.Init(kLength,kLength);
  CFunc::ResizeVec(var_pos_,kLength);
  for(size_t i = 0; i < kLength; i++){
    var_pos_[i].Init(0, bits_per_hash_);
  }
}

uint64_t NRSDic::PushBack(const uint64_t kVar){
  if(max_var_ < kVar){
    max_var_ = kVar;
  }
  if(avail_max_var_ < kVar){
    while(avail_max_var_ < kVar){
      avail_max_var_ = avail_max_var_ << 1;
    }
    DataResize(data_.ReservedCodes());
  }
  data_.PushBack(kVar);
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, kVar);
  uint64_t key1 = hash_table_[hash_val];
  if(key1 == 0){
    AddTable(hash_val);
  }
  else{
    if(data_[var_pos_[key1 - 1].Get(0,bits_per_hash_)] == kVar){
      AddTable(hash_val);
    }
    else{
      uint64_t tmp;
      while((tmp = hash_list_[key1 - 1]) != 0){
	if(data_[var_pos_[tmp - 1].Get(0, bits_per_hash_)] == kVar){
	  break;
	}
	key1 = tmp;
      }
      AddList(key1 - 1);
    }
  }
  return data_.NumCodes();
}

//private
void NRSDic::AddTable(const uint64_t kHashVal){
  uint64_t index = hash_table_[kHashVal];
  if(index == 0){
    index = num_var_.NumCodes() + 1;
    hash_table_.Set(kHashVal, index);
    num_var_.PushBack(0);
  }
  uint64_t num_var = num_var_[index - 1];
  var_pos_[index - 1].PushBack(num_var, data_.NumCodes()-1, bits_per_hash_);
  num_var_.Set(index - 1, ++num_var);
}

//private
void NRSDic::AddList(const uint64_t kHashVal){
  uint64_t index = hash_list_[kHashVal];
  if(index == 0){
    index = num_var_.NumCodes() + 1;
    hash_list_.Set(kHashVal, index);
    num_var_.PushBack(0);
  }
  uint64_t num_var = num_var_[index - 1];
  var_pos_[index - 1].PushBack(num_var, data_.NumCodes()-1, bits_per_hash_);
  num_var_.Set(index - 1, ++num_var);
}

uint64_t NRSDic::Select(const uint64_t kVar,
			FLCVector      &result){
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_,kVar);
  uint64_t key1 = hash_table_[hash_val];
  if(key1 == 0){
    return 0;
  }
  else{
    if(data_[var_pos_[key1 - 1].Get(0, bits_per_hash_)] == kVar){
	result = var_pos_[key1 - 1];
	return num_var_[key1 - 1];
    }
    else{
      uint64_t tmp;
      while((tmp  = hash_list_[key1 - 1]) != 0){
	if(data_[var_pos_[tmp - 1].Get(0, bits_per_hash_)] == kVar){
	  result = var_pos_[tmp - 1];
	  return num_var_[tmp - 1];
	}
	key1 = tmp;
      }
      return 0;
    }
  }
}

uint64_t NRSDic::Access(const uint64_t kIndex){
  return data_[kIndex];
}

void NRSDic::Clear(){
  for(size_t i = 0; i < data_.NumCodes(); i++){
    uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, 
				       data_[i]);
    uint64_t key1 = hash_table_[hash_val];
    if(key1 != 0){
      uint64_t tmp;
      while((tmp = hash_list_[key1 - 1]) != 0){
	hash_list_.Set(key1 - 1, 0);
	key1 = tmp;
      }
      hash_table_.Set(hash_val, 0);
    }
  }
  for(size_t i = 0; i < num_var_.NumCodes();i++){
    var_pos_[i].Clear(num_var_[i],bits_per_hash_);
  }
  var_pos_.clear();
  num_var_.Clear();
  data_.Clear();
}

void NRSDic::CheckClear(){
  for(size_t i = 0; i < hash_size_;i++){
    if(hash_list_[i] != 0){
      cout << "list error" << endl;
      cout << i << " " << hash_list_[i] << endl;
    }
    if(hash_table_[i] != 0){
      cout << "table error" << endl;
    }
  }
}


void NRSDic::Delete(){
  hash_table_.Delete();
  hash_list_.Delete();
  num_var_.Delete();
  vector<FLCVector> ().swap(var_pos_);
  data_.Delete();
}

uint64_t NRSDic::ByteSize() const{
    uint64_t byte_size = 0;
  for(size_t i = 0; i < data_.NumCodes(); i++){
      byte_size += var_pos_[i].ByteSize(num_var_[i], bits_per_hash_);
  }
  byte_size +=  (hash_table_.ByteSize()
		+ hash_list_.ByteSize()
		+ num_var_.ByteSize()
		+ data_.ByteSize()
		+ sizeof(NRSDic));
  return byte_size;
}

void NRSDic::DataResize(const uint64_t kLen){
  data_.Resize(kLen, avail_max_var_);
}
    
void NRSDic::Resize(const uint64_t kLen){
  DataResize(kLen);
  bits_per_hash_ = CFunc::MSB(kLen) + 1;
  if(kLen > hash_size_){
    hash_size_= kPrimes[++primes_index_];
  }
  hash_table_.Resize(hash_size_, kLen);
  hash_list_.Resize(hash_size_, kLen);
  num_var_.Init(kLen,kLen);
  CFunc::ResizeVec(var_pos_,kLen);
  for(size_t i = 0; i < kLen; i++){
    var_pos_[i].Init(0, bits_per_hash_);
  }
}

uint64_t NRSDic::BPH(){
  return bits_per_hash_;
}

uint64_t NRSDic::Num(){
  return data_.NumCodes();
}

uint64_t NRSDic::MaxVar(){
  return max_var_;
}

void     NRSDic::Save(ofstream &ofs){

  data_.Save(ofs);
  ofs.write((char*)&hash_size_, sizeof(hash_size_));
  ofs.write((char*)&primes_index_, sizeof(primes_index_));
  ofs.write((char*)&bits_per_hash_, sizeof(bits_per_hash_));
  ofs.write((char*)&avail_max_var_, sizeof(avail_max_var_));
  ofs.write((char*)&max_var_, sizeof(max_var_));
  hash_table_.Save(ofs);
  hash_list_.Save(ofs);
  num_var_.Save(ofs);
  for(size_t i = 0; i < num_var_.NumCodes(); i++){
    var_pos_[i].Save(ofs,
		     num_var_[i],
		     bits_per_hash_);
  }

}

void NRSDic::Load(ifstream &ifs){

  data_.Load(ifs);
  ifs.read((char*)&hash_size_, sizeof(hash_size_));
  ifs.read((char*)&primes_index_, sizeof(primes_index_));
  ifs.read((char*)&bits_per_hash_, sizeof(bits_per_hash_));
  ifs.read((char*)&avail_max_var_, sizeof(avail_max_var_));
  ifs.read((char*)&max_var_, sizeof(max_var_));
  hash_table_.Load(ifs);
  hash_list_.Load(ifs);
  num_var_.Load(ifs);
  CFunc::ResizeVec(var_pos_, num_var_.NumCodes());
  for(size_t i = 0;i < num_var_.NumCodes();i++){
    var_pos_[i].Load(ifs,
		     num_var_.NumCodes(),
		     bits_per_hash_);
  }

}

