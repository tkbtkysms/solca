/* permutation.hpp
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

#ifndef PERMUTATION_HPP
#define PERMUTATION_HPP

#include "eflc_vector.hpp"
#include "RSDic.hpp"

class Permutation{

private:
  //permutation structures
  EFLCVector elems_;
  rsdic::RSDic has_skip_elem_; 
  EFLCVector skip_list_;
  uint8_t pre_bpd_;
  //hash table for updating
  uint64_t  hash_size_;
  EFLCVector hash_table_;
  EFLCVector hash_list_;
  EFLCVector tmp_index_;
  EFLCVector tmp_skip_list_;
  
public:
  Permutation (): elems_(),
		  has_skip_elem_(),
		  skip_list_(),
		  pre_bpd_(),
		  hash_size_(),
                  hash_table_(),
		  hash_list_(),
		  tmp_index_(),
		  tmp_skip_list_(){}
  ~Permutation(){}
  //build/clear/resize/ permutation
  void Build(EFLCVector &elem);
  void Clear();
  void Resize(const uint64_t kLength);
  //setters and getters
  void SetElem(const uint64_t kIndex, 
	       const uint64_t kVar);
  uint64_t GetElem(const uint64_t kIndex);
  uint64_t GetPreElem(const uint64_t kIndex);
  void SetPreBPD();
  
  void ReIndex();
  uint64_t Access(const uint64_t kPos);
  uint64_t ReverseAccess(const uint64_t kPos);
  uint64_t Length();
  void Save(std::ofstream &ofs);
  void Load(std::ifstream &ifs);
  uint64_t ByteSize() const;
private:
  //operations for the update of hash table
  void InitHash();
  void SetHash(const uint64_t kIndex,
	       const uint64_t kSkipPos);
  uint64_t GetHash(const uint64_t kIndex);
  void AddTable(const uint64_t kHashVal,
		const uint64_t kIndex,
		const uint64_t kSkipPos);
  void AddList(const uint64_t kHashVal,
	       const uint64_t kIndex,
	       const uint64_t kSkipPos);
  void DeleteHash();


};

#endif // PERMUTATION_HPP
