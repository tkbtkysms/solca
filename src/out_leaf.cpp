/* out_leaf.cpp
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

#include "out_leaf.hpp"

using namespace std;
using namespace rsdic;

void OutLeaf::Init(){

  left_gb_.Clear();
  left_perm_.Clear();
  right_gb_.Clear();
  right_perm_.Clear();
  prefix_len_ = 0;
  
  suffix_max_len_ = kInitADYGMRLength;
  suf_left_.Init(suffix_max_len_);
  suf_right_.Init(suffix_max_len_);
  is_first_build_ = true;
  
  for(size_t i = 0; i < kPrimeSize; i++){
    if(kPrimes[i] >= suffix_max_len_){
      primes_index_ = i;
      hash_size_ = kPrimes[i];
      break;
    }
    else if(i == kPrimeSize - 1){
      primes_index_ = i;
      hash_size_ = kPrimes[i];
    }
  }
 
  hash_table_.Init(hash_size_, hash_size_);
  hash_list_.Init(hash_size_, hash_size_);
  
  max_byte_size_ = 0;
}

void OutLeaf::InitUHashTable(){
  
  u_hash_table_.Init(hash_size_, hash_size_);
  u_hash_list_.Init(hash_size_, hash_size_);
  pos_array_.resize(suffix_max_len_);
  bits_per_pos_ = CFunc::MSB(prefix_len_ + suffix_max_len_) + 1;
  num_pos_.Init(suffix_max_len_, suffix_max_len_);
}

void OutLeaf::DeleteUHashTable(){

  u_hash_table_.Delete();
  u_hash_list_.Delete();
  for(size_t i = 0; i < num_pos_.ReservedCodes();i++){
    pos_array_[i].Delete();
  }
  vector<FLCVector> ().swap(pos_array_);
  num_pos_.Delete();

}

uint64_t OutLeaf::ReverseAccessAndAdd(const uint64_t kVar1,
				      const uint64_t kVar2){
  uint64_t ret;
  if((ret = SearchRule(kVar1,kVar2)) == kDummyCode){
    suf_left_.PushBack(kVar1);
    suf_right_.PushBack(kVar2);  
    if(suf_left_.Num() == suffix_max_len_){
      if(is_first_build_){
	FirstBuildPrefix();
	is_first_build_ = false;
      }
      else{
	ReBuildPrefix(); 
      }
      prefix_len_ += suf_left_.Num();
      Resize();
    }
  }

  return ret;

}


uint64_t OutLeaf::ReverseAccess(const uint64_t kVar1,
				 const uint64_t kVar2){
  
  return SearchRule(kVar1,kVar2);

}

void OutLeaf::ClearSuffix(){
  suf_left_.Clear();
  suf_right_.Clear();
}

//private eb1 stores whether var exists in suf_left_. 
RSDic OutLeaf::BuildLeftEB(){
  
  uint64_t max_var1 = suf_left_.MaxVar();
 
  vector<bool> tmp_eb1;
 
  tmp_eb1.clear();
  tmp_eb1.resize(max_var1 + 1);
  
  for(size_t i = 0; i < max_var1 + 1; i++){
    tmp_eb1[i] = false;
  }

  for(size_t i = 0; i < suf_left_.Num();i++){
    tmp_eb1[suf_left_.Access(i)] = true;
  }

  RSDic eb1;
  eb1.Clear();
  for(size_t i = 0; i < max_var1 + 1;i++){
    eb1.PushBack(tmp_eb1[i]);
  }
  vector<bool>().swap(tmp_eb1);
  eb1.LastProcess();

  return eb1;
}

//private eb2 stores whether var exists in suf_right_. 
RSDic OutLeaf::BuildRightEB(){
  
  uint64_t max_var2 = suf_right_.MaxVar();
  vector<bool> tmp_eb2;

  tmp_eb2.clear();
  tmp_eb2.resize(max_var2 + 1);

  for(size_t i = 0; i < max_var2 + 1; i++){
    tmp_eb2[i] = false;
  }

  for(size_t i = 0; i < suf_right_.Num();i++){
    tmp_eb2[suf_right_.Access(i)] = true;
  }

  RSDic eb2;
  eb2.Clear();
  for(size_t i = 0; i < max_var2 + 1;i++){
    eb2.PushBack(tmp_eb2[i]);
  }
  vector<bool>().swap(tmp_eb2);
  eb2.LastProcess();
  return eb2;
}

//private
void OutLeaf::FirstBuildPrefix(){

  InitUHashTable();
  FirstBuildPreLeft();
  FirstBuildPreRight();

}

void OutLeaf::FirstBuildPreLeft(){
  
  RSDic eb1  = BuildLeftEB();
  uint64_t prev_var = 0, now_var = 0;
  uint64_t perm_index = 0;
  FLCVector tmp_poss;
  left_perm_.Resize(prefix_len_ + suf_left_.Num());

  left_gb_.Clear();
  for(size_t i = 0; i < eb1.Rank(eb1.num() , 1);i++){
    now_var = eb1.Select(i,true);
    for(size_t j = 0; j < (now_var-prev_var); j++){
      left_gb_.PushBack(false);
    }
    uint64_t num = suf_left_.Select(now_var,tmp_poss);
    uint64_t BPH = suf_left_.BPH();
    for(size_t j = 0; j < num; j++){
      left_gb_.PushBack(true);
      left_perm_.SetElem(perm_index++,
		     tmp_poss.Get(j,BPH) + prefix_len_);
      AddPos(perm_index - 1);
    }
    prev_var = now_var;
  }
  
  eb1.Clear();
  left_gb_.LastProcess();
  left_perm_.ReIndex();

}

void OutLeaf::AddPos(const uint64_t kPos){
  uint64_t var =  Right(left_perm_.Access(kPos));
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, var);
  uint64_t key1 = u_hash_table_[hash_val];
  if(key1 == 0){
    AddUTable(hash_val, kPos);
  }
  else{
    if(var == Right(left_perm_.Access(pos_array_[key1 - 1].Get(0,bits_per_pos_)))){
      AddUTable(hash_val, kPos);
    }
    else{
      uint64_t tmp;
      while((tmp = u_hash_list_[key1 - 1]) != 0){
	if(var == Right(left_perm_.Access(pos_array_[tmp - 1].Get(0,bits_per_pos_)))){
	  break;
	}
	key1 = tmp;
      }
      AddUList(key1 - 1, kPos);
    }
  } 
}

void OutLeaf::AddUTable(const uint64_t kIndex,
			const uint64_t kPos){
  
  uint64_t pos_index = u_hash_table_[kIndex];
  
  if(pos_index ==  0){
    pos_index = num_pos_.NumCodes() + 1;
    u_hash_table_.Set(kIndex, pos_index);
    num_pos_.PushBack(0);
  }
  
  uint64_t num_pos = num_pos_[pos_index - 1];
  
  pos_array_[pos_index - 1].PushBack(num_pos, kPos, bits_per_pos_);
  num_pos_.Set(pos_index - 1, ++num_pos);

}

void OutLeaf::AddUList(const uint64_t kIndex,
		       const uint64_t kPos){

  uint64_t pos_index = u_hash_list_[kIndex];
  if(pos_index == 0){
    pos_index = num_pos_.NumCodes() + 1;
    u_hash_list_.Set(kIndex, pos_index);
    num_pos_.PushBack(0);
  }

  uint64_t num_pos = num_pos_[pos_index - 1];

  pos_array_[pos_index - 1].PushBack(num_pos, kPos, bits_per_pos_);
  num_pos_.Set(pos_index - 1, ++num_pos);

}


void OutLeaf::FirstBuildPreRight(){

  RSDic eb2  = BuildRightEB();
  uint64_t prev_var = 0, now_var = 0;
  uint64_t perm_index = 0;
  FLCVector tmp_poss;
  right_perm_.Resize(prefix_len_ + suf_right_.Num());
  right_gb_.Clear();
  for(size_t i = 0; i < eb2.Rank(eb2.num() , 1);i++){
    now_var = eb2.Select(i,true);
    for(size_t j = 0; j < (now_var-prev_var); j++){
      right_gb_.PushBack(false);
    }
    uint64_t num = GetPos(now_var, tmp_poss);
    uint64_t BPH = bits_per_pos_;
    for(size_t j = 0; j < num; j++){
      right_gb_.PushBack(true);
      right_perm_.SetElem(perm_index++,
		     tmp_poss.Get(j,BPH) + prefix_len_);
    }
    prev_var = now_var;
  }
  
  uint64_t eb2_size = eb2.GetUsageBytes();
  
  eb2.Clear();
  right_gb_.LastProcess();
  uint64_t space = Space();
  DeleteUHashTable();
  right_perm_.ReIndex();
  
  max_byte_size_ = space 
    + eb2_size;
  
}

uint64_t OutLeaf::GetPos(const uint64_t kVar,
			 FLCVector   &result){
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, kVar);
  uint64_t key1 = u_hash_table_[hash_val];
  if(key1 == 0){

    return 0;
  }
  else{
    if(kVar == Right(left_perm_.Access(pos_array_[key1 - 1].Get(0,bits_per_pos_)))){
      result = pos_array_[key1 - 1];
      return num_pos_[key1 - 1];
    }
    else{
      uint64_t tmp;
      while((tmp = u_hash_list_[key1 - 1]) != 0){
	if(kVar == Right(left_perm_.Access(pos_array_[tmp - 1].Get(0,bits_per_pos_)))){
	  result = pos_array_[tmp - 1];
	  return num_pos_[tmp - 1];
	}
	key1 = tmp;
      }
      return 0;
    }
  }
}

void OutLeaf::ReBuildPrefix(){

  InitUHashTable();
  ReBuildPreLeft();
  ReBuildPreRight();


}

void OutLeaf::ReBuildPreLeft(){
  
  //build EB;

  RSDic eb1 = BuildLeftEB();

  //Resize perm_

  uint64_t perm_index = prefix_len_+ suf_left_.Num() - 1;
  uint64_t prefix_perm_index = prefix_len_ -1; 
  left_perm_.SetPreBPD();
  left_perm_.Resize(prefix_len_ + suf_left_.Num());

  //Prepare tmp_b_
  vector<bool> tmp_bit_array;
  tmp_bit_array.clear();
  uint64_t prefix_max_var = left_gb_.Rank(left_gb_.num(),0);
  uint64_t num_suffix_var = eb1.Rank(eb1.num(),1);
  uint64_t suf_max_var = eb1.Select(num_suffix_var - 1,1);
  uint64_t max_var;
  if(suf_max_var > prefix_max_var){
    max_var = suf_max_var;
  }
  else{
    max_var = prefix_max_var;
  }

  uint64_t resized_b_len = max_var + prefix_len_ + suf_left_.Num();
  tmp_bit_array.resize(resized_b_len);

  vector<bool> tmp_ib;
  tmp_ib.clear();
  CFunc::ResizeVec(tmp_ib, prefix_len_ + suf_left_.Num());
 
  for(size_t i = 0; i < prefix_len_ + suf_left_.Num(); i++){
    tmp_ib[i] = false;
  }

  uint64_t tmp_bit_array_pos = resized_b_len - 1;
  uint64_t var_index = num_suffix_var - 1;
  FLCVector tmp_poss;

  ////insert larger vars to the tail of tmp_bit_array
  
  for(;var_index != kDummyCode;var_index--){
    size_t i = eb1.Select(var_index, 1);
    if (i <= prefix_max_var){
      break;
    }
    uint64_t num_i = suf_left_.Select(i,tmp_poss);    
    for(size_t j = num_i-1; j != kDummyCode; j--){
      tmp_bit_array[tmp_bit_array_pos--] = true;
      left_perm_.SetElem(perm_index--,
		    tmp_poss.Get(j,suf_left_.BPH()) + prefix_len_);
      tmp_ib[perm_index + 1] = true;
      AddPos(perm_index + 1);
    }
    uint64_t smaller_var,next;
    if(var_index == 0){
      smaller_var = prefix_max_var;
    }
    else if ((next = eb1.Select(var_index - 1,1)) > prefix_max_var){
      smaller_var = next;
    }
    else{
      smaller_var = prefix_max_var;
    }
    for(size_t j = 0; j < (i - smaller_var);j++ ){
     tmp_bit_array[tmp_bit_array_pos--] = false;
    }
  }

  ////insert smaller vars.
  uint64_t now_var = prefix_max_var;
  uint64_t num_var;
  if((num_var = suf_left_.Select(now_var,tmp_poss)) > 0){
    for(size_t i = num_var - 1;i != kDummyCode; i--){
      tmp_bit_array[tmp_bit_array_pos--] = true;
      left_perm_.SetElem(perm_index--,
		    tmp_poss.Get(i,suf_left_.BPH())+prefix_len_);
      tmp_ib[perm_index + 1] = true;
      AddPos(perm_index + 1);
    }
  }
  for(size_t i = left_gb_.num()-1;i != kDummyCode ;i--){
    bool tmp_bit = left_gb_.GetBit(i);
    tmp_bit_array[tmp_bit_array_pos--] = tmp_bit;
    if(!tmp_bit){ 
      now_var--;
      if((num_var = suf_left_.Select(now_var,tmp_poss)) > 0){
	for(size_t j = num_var - 1;j != kDummyCode; j--){
	  tmp_bit_array[tmp_bit_array_pos--] = true;
	  left_perm_.SetElem(perm_index--,
			tmp_poss.Get(j,suf_left_.BPH())+prefix_len_);
	  tmp_ib[perm_index + 1] = true;
	  AddPos(perm_index + 1);
	}
      }
    }
    else{
      uint64_t prev =  left_perm_.GetPreElem(prefix_perm_index--);
      left_perm_.SetElem(perm_index--,prev);
    }
  }


  left_gb_.Clear();
  for(size_t i = 0; i < resized_b_len; i++){
    left_gb_.PushBack(tmp_bit_array[i]);
  }

  left_gb_.LastProcess();
  
  ib_.Clear();
  for(size_t i = 0; i < prefix_len_ + suf_left_.Num(); i++){
    ib_.PushBack(tmp_ib[i]);
  }
  
  ib_.LastProcess();
  eb1.Clear();
  left_perm_.ReIndex();
  vector<bool> ().swap(tmp_bit_array);
  vector<bool> ().swap(tmp_ib);
  
}


void OutLeaf::ReBuildPreRight(){
  
  //build EB;

  RSDic eb2 = BuildRightEB();

  //Resize perm_

  uint64_t perm_index = prefix_len_+ suf_left_.Num() - 1;
  uint64_t prefix_perm_index = prefix_len_ -1; 
  right_perm_.SetPreBPD();
  right_perm_.Resize(prefix_len_ + suf_right_.Num());

  //Prepare tmp_b_
  vector<bool> tmp_bit_array;
  tmp_bit_array.clear();
  uint64_t prefix_max_var = right_gb_.Rank(right_gb_.num(),0);
  uint64_t num_suffix_var = eb2.Rank(eb2.num(),1);
  uint64_t suf_max_var = eb2.Select(num_suffix_var - 1,1);
  uint64_t max_var;
  if(suf_max_var > prefix_max_var){
    max_var = suf_max_var;
  }
  else{
    max_var = prefix_max_var;
  }

  uint64_t resized_b_len = max_var + prefix_len_ + suf_right_.Num();
  tmp_bit_array.resize(resized_b_len);
  
  uint64_t tmp_bit_array_pos = resized_b_len - 1;
  uint64_t var_index = num_suffix_var - 1;
  FLCVector tmp_poss;

  ////insert larger vars to the tail of tmp_bit_array
 
  for(;var_index != kDummyCode;var_index--){
    size_t i = eb2.Select(var_index, 1);
    if(i <= prefix_max_var){
      break;
    }
    
    uint64_t num_i = GetPos(i,tmp_poss);
    
    for(size_t j = 0; j < num_i ; j++){
      tmp_bit_array[tmp_bit_array_pos--] = true;
      right_perm_.SetElem(perm_index--,
		     tmp_poss.Get(j, bits_per_pos_));
    }
    uint64_t smaller_var,next;
    if(var_index == 0){
      smaller_var = prefix_max_var;
    }
    else if ((next = eb2.Select(var_index - 1,1)) > prefix_max_var){
      smaller_var = next;
    }
    else{
      smaller_var = prefix_max_var;
    }
    for(size_t j = 0; j < (i - smaller_var);j++ ){
      tmp_bit_array[tmp_bit_array_pos--] = false;
    }
  }
  ////insert smaller vars.
  uint64_t now_var = prefix_max_var;
  uint64_t num_var;

  for(size_t i = right_gb_.num()-1;i != kDummyCode; i--){
    if(!right_gb_.GetBit(i)){
      now_var--;
      tmp_bit_array[tmp_bit_array_pos--] = false;     
      if(i == 0){
	if((num_var = GetPos(now_var,tmp_poss)) > 0){
	  for(size_t j = 0; j < num_var; j++){
	    tmp_bit_array[tmp_bit_array_pos--] = true;
	    right_perm_.SetElem(perm_index--,
			   tmp_poss.Get(j, bits_per_pos_));
	  }
	}
      }

      if(i != 0 && !right_gb_.GetBit(i - 1)){
	if((num_var = GetPos(now_var,tmp_poss)) > 0){
	  for(size_t j = 0; j < num_var; j++){
	    tmp_bit_array[tmp_bit_array_pos--] = true;
	    right_perm_.SetElem(perm_index--,
			   tmp_poss.Get(j, bits_per_pos_));
	  }
	}
      }

    }
    else{
      if((num_var = GetPos(now_var,tmp_poss)) > 0){
	uint64_t tmp_pos_index = 0;
	for(; tmp_pos_index < num_var && i != kDummyCode && right_gb_.GetBit(i);){
	  tmp_bit_array[tmp_bit_array_pos--] = true;
	  uint64_t tmp1 = tmp_poss.Get(tmp_pos_index ,bits_per_pos_);
	  uint64_t tmp2 = ib_.Select(right_perm_.GetPreElem(prefix_perm_index),0);
	  if(tmp1 > tmp2){
	    right_perm_.SetElem(perm_index--, tmp1);
	    tmp_pos_index++;
	  }
	  else{
	    right_perm_.SetElem(perm_index--, tmp2);
	    prefix_perm_index--;
	    i--;
	  }
	}
	
	for(;tmp_pos_index < num_var;tmp_pos_index++){
	  tmp_bit_array[tmp_bit_array_pos--] = true;
	  uint64_t tmp1 = tmp_poss.Get(tmp_pos_index, bits_per_pos_);
	  right_perm_.SetElem(perm_index--, tmp1);
	}

	for(;i != kDummyCode && right_gb_.GetBit(i);i--,prefix_perm_index--){
	  tmp_bit_array[tmp_bit_array_pos--] = true;
	  uint64_t tmp2 = ib_.Select(right_perm_.GetPreElem(prefix_perm_index),0);
	  right_perm_.SetElem(perm_index--, tmp2);
	}
	i++;
      }
      else{
	for(;i!= kDummyCode && right_gb_.GetBit(i);i--){
	  tmp_bit_array[tmp_bit_array_pos--] = true;
	  uint64_t prefix_perm =  right_perm_.GetPreElem(prefix_perm_index--);
	  right_perm_.SetElem(perm_index--,
			 ib_.Select(prefix_perm, 0));
	}
	i++;
      }
    }
  }
  

  right_gb_.Clear();
  for(size_t i = 0; i < resized_b_len; i++){
    right_gb_.PushBack(tmp_bit_array[i]);
  }
  
  uint64_t eb2_size = eb2.GetUsageBytes();
  eb2.Clear();
  
  right_gb_.LastProcess();
  uint64_t space = Space();
  DeleteUHashTable();
  vector<bool> ().swap(tmp_bit_array);
  ib_.Clear();
  right_perm_.ReIndex();
  max_byte_size_ = space + eb2_size + resized_b_len/64 + 1;
  
}

void OutLeaf::Resize(){
 
  suffix_max_len_ = UpdateSuffixMaxLen();
  ResizeHash();  
  ClearSuffix();
  suf_left_.Resize(suffix_max_len_);
  suf_right_.Resize(suffix_max_len_);  

}

void OutLeaf::ResizeHash(){

  if(hash_size_ < suffix_max_len_){
    while(kPrimes[++primes_index_] < suffix_max_len_);
    hash_size_ = kPrimes[primes_index_]; 
    hash_table_.Init(hash_size_, hash_size_);
    hash_list_.Init(hash_size_, hash_size_);
  }
  else{
    for(size_t i = 0; i < suf_left_.Num(); i++){
      uint64_t var1 = suf_left_.Access(i);
      uint64_t var2 = suf_right_.Access(i);
      uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, (var1+3)*(var2+5));
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
  }

}

uint64_t OutLeaf::UpdateSuffixMaxLen(){
  return prefix_len_/(CFunc::MSB(CFunc::MSB(prefix_len_)+1) + 1) + 1;
}


uint64_t OutLeaf::Left(const uint64_t kPos){
  if(kPos < prefix_len_){
    uint64_t reverse_index = left_perm_.ReverseAccess(kPos);
    return left_gb_.Select(reverse_index, 1) - reverse_index; 
  }
  else{
    if(kPos <  (prefix_len_ + suf_left_.Num())){
      return suf_left_.Access(kPos - prefix_len_);
    }
    else{
      return kDummyCode;
    }
  }
}


uint64_t OutLeaf::Right(const uint64_t kPos){
  if(kPos < prefix_len_){
    uint64_t reverse_index1 = left_perm_.ReverseAccess(kPos); 
    uint64_t reverse_index2 = right_perm_.ReverseAccess(reverse_index1);
    return right_gb_.Select(reverse_index2, 1) - reverse_index2; 
  }
  else{
    if(kPos <  (prefix_len_ + suf_right_.Num())){
      return suf_right_.Access(kPos - prefix_len_);
    }
    else{
      return kDummyCode;
    }
  }
}

uint64_t OutLeaf::SearchRule(const uint64_t kVar1,
			     const uint64_t kVar2){
  
  if(prefix_len_ > 0){
    uint64_t pre_reverse_access = PreReverseAccess(kVar1, kVar2);
    if(pre_reverse_access != kDummyCode){
      return pre_reverse_access;
    }
  }
  return SufReverseAccess(kVar1, kVar2);  

}

uint64_t OutLeaf::PreReverseAccess(const uint64_t kVar1,
				   const uint64_t kVar2){
  uint64_t start_index = kDummyCode;
  uint64_t end_index   = kDummyCode;  
  if(kVar1 == 0){
    start_index = 0;
    if(left_gb_.GetBit(0)){
      end_index = left_gb_.Select(kVar1,false) - 1;
    }
  }
  else{
    if(kVar1 <= left_gb_.zero_num()){
      start_index = left_gb_.Select(kVar1 - 1, false) - kVar1 + 1;
      if(kVar1 == left_gb_.zero_num()){
	end_index = left_gb_.num() - kVar1 - 1;
      }
      else{
	end_index = left_gb_.Select(kVar1, false) - kVar1 - 1;
      }
    }
  }
  if(start_index != kDummyCode && end_index != kDummyCode){
    if(start_index <= end_index){
      uint64_t result = PreSelectInRightPerm(kVar2, PreRightRank(kVar2, start_index));
      if(result <= end_index){
	return left_perm_.Access(result);
      }
    }
  }
  return kDummyCode;
}

uint64_t OutLeaf::SufReverseAccess(const uint64_t kVar1,
				   const uint64_t kVar2){
  uint64_t hash_val = CFunc::ComputeHashVal(hash_size_, (kVar1+3)*(kVar2+5));
  uint64_t key1 = hash_table_[hash_val];
  if(key1 == 0){
    AddTable(hash_val);
    return kDummyCode;
  }
  else{
    if(suf_left_.Access(key1 - 1) == kVar1 && suf_right_.Access(key1 - 1) == kVar2){      
      return prefix_len_ + key1 - 1;
    }
    else{
      uint64_t tmp;
      while((tmp = hash_list_[key1 - 1]) != 0){
	if(suf_left_.Access(tmp - 1) == kVar1 && suf_right_.Access(tmp - 1) == kVar2){
	  return prefix_len_ + tmp - 1;
	}
	key1 = tmp;
      }
      AddList(key1 - 1);
      return kDummyCode; 
    }
  } 
}

//private
void OutLeaf::AddTable(const uint64_t kIndex){

  hash_table_.Set(kIndex, suf_left_.Num() + 1);

}

//private
void OutLeaf::AddList(const uint64_t kIndex){
  
   hash_list_.Set(kIndex, suf_left_.Num() + 1);

}


uint64_t OutLeaf::LeftRankAll(const uint64_t kVar){
  
  if(kVar > left_gb_.zero_num()){
    return 0;
  }
  else{  
    uint64_t select1 = 0;
    if(kVar == 0){
      if(!left_gb_.GetBit(0)){
	return 0;
      }
    }
    if(kVar != 0){
      select1 = left_gb_.Select(kVar - 1, 0) + 1;
      if(select1 == kDummyCode){
	return 0;
      }
    }
    uint64_t select2 = left_gb_.Select(kVar , 0);
    if(select2 == kDummyCode){
      return 0;
    }
    if(kVar == left_gb_.zero_num()){
      select2 = left_gb_.num();
    }
    
    return select2 - select1;
  
  }
}


uint64_t OutLeaf::PreLeftRank(const uint64_t kVar,
			   const uint64_t kPos){
  
  if(kVar > left_gb_.zero_num()){
    return 0;
  }
  else{
    uint64_t select1 = 0;
    uint64_t start = 0;
    if(kVar == 0){
      if(!left_gb_.GetBit(0)){
	return 0;
      }
    }
    if(kVar != 0){
      select1 = left_gb_.Select(kVar-1, 0);
      if(select1 == kDummyCode){
	return 0;
      }
      start = select1 - (kVar-1);
      
    }
    uint64_t select2 = left_gb_.Select(kVar , 0);
    if(select2 == kDummyCode){
      return 0;
    }
    if(kVar == left_gb_.zero_num()){
      select2 = left_gb_.num();
    }
    if((select1 + 1) != select2){
      uint64_t end = select2 - kVar-1;
      uint64_t i = start;
      uint64_t j = end;
      uint64_t num = (j - i)/2 + start;
      while(i < j){
	if(left_perm_.Access(num) < kPos){ 
	  if(kPos <= left_perm_.Access(num + 1)){
	    break;
	  }
	  else{
	    if(num == j){
	      i = num;
	    }
	    else{
	      i = num + 1;
	    }
	  }
	}
	else{
	  if(num == i){
	    j = num;
	  }
	  else{
	    j = num - 1;
	  }
	}
	num = (j - i)/2 + i;
      }
      if(left_perm_.Access(num) < kPos){
	return num - start + 1;
      }
      else{
	return num - start;
      }
    }
    else{
      return 0;
    }
  }
}


uint64_t OutLeaf::PreRightRank(const uint64_t kVar,
			       const uint64_t kPos){
  
  if(kVar > right_gb_.zero_num()){
    return 0;
  }
  else{
    uint64_t select1 = 0;
    uint64_t start = 0;
    if(kVar == 0){
      if(!right_gb_.GetBit(0)){
	return 0;
      }
    }
    if(kVar != 0){
      select1 = right_gb_.Select(kVar-1, 0);
      if(select1 == kDummyCode){
	return 0;
      }
      start = select1 - (kVar-1);
      
    }
    uint64_t select2 = right_gb_.Select(kVar , 0);
    if(select2 == kDummyCode){
      return 0;
    }
    if(kVar == right_gb_.zero_num()){
      select2 = right_gb_.num();
    }
    if((select1 + 1) != select2){
      uint64_t end = select2 - kVar-1;
      uint64_t i = start;
      uint64_t j = end;
      uint64_t num = (j - i)/2 + start;
      while(i < j){
	if(right_perm_.Access(num) < kPos){ 
	  if(kPos <= right_perm_.Access(num + 1)){
	    break;
	  }
	  else{
	    if(num == j){
	      i = num;
	    }
	    else{
	      i = num + 1;
	    }
	  }
	}
	else{
	  if(num == i){
	    j = num;
	  }
	  else{
	    j = num - 1;
	  }
	}
	num = (j - i)/2 + i;
      }
      if(right_perm_.Access(num) < kPos){
	return num - start + 1;
      }
      else{
	return num - start;
      }
    }
    else{
      return 0;
    }
  }
}


uint64_t OutLeaf::PreLeftSelect(const uint64_t kVar, const uint64_t kPos){
  if(kVar == 0){
    if(!left_gb_.GetBit(0)){
      return kDummyCode;
    }
    else{
      uint64_t select = left_gb_.Select(kVar,false);
      if(kPos < select){
	return left_perm_.Access(kPos);
      }
    }
  }
  else{
    if(kVar > left_gb_.zero_num()){
      return kDummyCode;
    }
      uint64_t select1 = left_gb_.Select(kVar-1, false);
      uint64_t select2 = 0;
      if(kVar == left_gb_.Rank(left_gb_.num(),0)){
	select2 = left_gb_.num();
      }
      else{
	select2 = left_gb_.Select(kVar, false);
      }
      if(kPos < (select2 - select1 - 1)){
	return left_perm_.Access(select1 - kVar + kPos + 1);
      }
  }
  
  return kDummyCode;
}


uint64_t OutLeaf::PreRightSelect(const uint64_t kVar, const uint64_t kPos){
  if(kVar == 0){
    if(!right_gb_.GetBit(0)){
      return kDummyCode;
    }
    else{
      uint64_t select = right_gb_.Select(kVar,false);
      if(kPos < select){
	return left_perm_.Access(right_perm_.Access(kPos));
      }
    }
  }
  else{
    if(kVar > right_gb_.zero_num()){
      return kDummyCode;
    }
    uint64_t select1 = right_gb_.Select(kVar-1, false);
    uint64_t select2 = 0;
    if(kVar == right_gb_.Rank(right_gb_.num(),0)){
      select2 = right_gb_.num();
    }
    else{
      select2 = right_gb_.Select(kVar, false);
    }
    if(kPos < (select2 - select1 - 1)){
      return left_perm_.Access(right_perm_.Access(select1 - kVar + kPos + 1));
    }
  }
  
  return kDummyCode;
}


uint64_t OutLeaf::PreSelectInRightPerm(const uint64_t kVar, const uint64_t kPos){

  if(kVar == 0){
    if(!right_gb_.GetBit(0)){
      return kDummyCode;
    }
    else{
      uint64_t select = right_gb_.Select(kVar,false);
      if(kPos < select){
	return right_perm_.Access(kPos);
      }
    }
  }
  else{
    if(kVar > right_gb_.zero_num()){
      return kDummyCode;
    }
    uint64_t select1 = right_gb_.Select(kVar-1, false);
    uint64_t select2 = 0;
    if(kVar == right_gb_.Rank(right_gb_.num(),0)){
      select2 = right_gb_.num();
    }
    else{
      select2 = right_gb_.Select(kVar, false);
    }
    if(kPos < (select2 - select1 - 1)){
      return right_perm_.Access(select1 - kVar + kPos + 1);
    }

  }

  return kDummyCode;
}

uint64_t OutLeaf::SufLeftSelect(const uint64_t kVar,
				FLCVector &result){
  return suf_left_.Select(kVar,result);
}


uint64_t OutLeaf::SufRightSelect(const uint64_t kVar,
				 FLCVector &result){
  return suf_right_.Select(kVar,result);
}

uint64_t OutLeaf::ByteSize() const{
  uint64_t space = Space();
  /*if(max_byte_size_ < space){
    max_byte_size_ = space;
    }*/
  return space;
}

uint64_t OutLeaf::Space() const{
    uint64_t byte_size 
    = left_gb_.GetUsageBytes()
    + left_perm_.ByteSize()
    + right_gb_.GetUsageBytes()
    + right_perm_.ByteSize()
    + suf_left_.ByteSize()
    + suf_right_.ByteSize()
    + hash_table_.ByteSize()
    + hash_list_.ByteSize()
    + u_hash_table_.ByteSize()
    + u_hash_list_.ByteSize()
    + num_pos_.ByteSize()
    + ib_.GetUsageBytes()
    + sizeof(OutLeaf);

  for(size_t i = 0; i < num_pos_.NumCodes();i++){
    byte_size += pos_array_[i].ByteSize(num_pos_[i], bits_per_pos_);
  }
  return byte_size;
}


void OutLeaf::Clear(){
  left_gb_.Clear();
  left_perm_.Clear();
  right_gb_.Clear();
  right_perm_.Clear();
  prefix_len_ = 0;
  suf_left_.Clear();
  suf_right_.Clear();
  suffix_max_len_ = 0;
  is_first_build_ = false;
  hash_table_.Clear();
  hash_list_.Clear();
  hash_size_ = 0;
  primes_index_  = 0;
  u_hash_table_.Clear();
  u_hash_list_.Clear();
  for(size_t i = 0; i < pos_array_.size();i++){
    pos_array_[i].Clear(num_pos_[i], bits_per_pos_);
  }
  pos_array_.clear();
  bits_per_pos_ = 0;
  num_pos_.Clear();
  ib_.Clear();
}


void OutLeaf::Save(ofstream &ofs){
  if(suf_left_.Num() > 0){
    if(is_first_build_){
      FirstBuildPrefix();
      is_first_build_ = false;
    }
    else{
      ReBuildPrefix(); 
    }
    prefix_len_ += suf_left_.Num();
  }
  left_gb_.Save(ofs);
  left_perm_.Save(ofs);
  right_gb_.Save(ofs);
  right_perm_.Save(ofs);
  ofs.write((char*)&prefix_len_, sizeof(prefix_len_));
  ofs.write((char*)&suffix_max_len_, sizeof(suffix_max_len_));
  ofs.write((char*)&is_first_build_, sizeof(is_first_build_));
  ofs.write((char*)&hash_size_, sizeof(hash_size_));
  ofs.write((char*)&primes_index_, sizeof(primes_index_));
  ofs.write((char*)&bits_per_pos_, sizeof(bits_per_pos_));
}
	
void OutLeaf::Load(ifstream &ifs){
  left_gb_.Load(ifs);
  left_perm_.Load(ifs);
  right_gb_.Load(ifs);
  right_perm_.Load(ifs);
  ifs.read((char*)&prefix_len_, sizeof(prefix_len_));
  ifs.read((char*)&suffix_max_len_, sizeof(suffix_max_len_));
  ifs.read((char*)&is_first_build_, sizeof(is_first_build_));
  ifs.read((char*)&hash_size_, sizeof(hash_size_));
  ifs.read((char*)&primes_index_, sizeof(primes_index_));
  ifs.read((char*)&bits_per_pos_, sizeof(bits_per_pos_));
}

