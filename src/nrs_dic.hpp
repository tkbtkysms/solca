/* nrs_dic.hpp
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

#ifndef NRS_DIC_HPP_
#define NRS_DIC_HPP_

#include <cstdint>
#include <fstream>
#include "eflc_vector.hpp"
#include "flc_vector.hpp"

//naive rank/select dictionary for large alphabet
class NRSDic{
private : 
  EFLCVector data_;
  uint64_t hash_size_;
  uint64_t primes_index_;
  uint64_t bits_per_hash_;
  uint64_t avail_max_var_;
  uint64_t max_var_;
  EFLCVector hash_table_;
  EFLCVector hash_list_;
  EFLCVector num_var_;
  std::vector<FLCVector> var_pos_; 
 
public :
  NRSDic() : data_(),
	     hash_size_(),
	     primes_index_(),
	     bits_per_hash_(),
	     avail_max_var_(),
	     max_var_(),
	     hash_table_(), 
	     hash_list_(),
	     num_var_(),
	     var_pos_(){}
  ~NRSDic(){}
  
  void     Init           (const uint64_t kLength);
  uint64_t PushBack       (const uint64_t kVar);
 
  uint64_t Select         (const uint64_t kVar,
			   FLCVector    &result);
  uint64_t Access         (const uint64_t kIndex);
  void     Clear          ();
  void     CheckClear     ();
  void     Delete         ();
  uint64_t ByteSize       () const;
  void     DataResize     (const uint64_t kLen);
  void     Resize         (const uint64_t kLen);
  uint64_t BPH            ();
  uint64_t Num            ();
  uint64_t MaxVar         ();
  void     Save           (std::ofstream &ofs);
  void     Load           (std::ifstream &ifs);
private:
  void     AddTable       (const uint64_t kHashVal);
  void     AddList        (const uint64_t kHashVal);
}; 
#endif //NRS_DIC_HPP_

