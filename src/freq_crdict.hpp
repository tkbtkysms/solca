/* freq_crdict.hpp
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

#ifndef FREQ_CRDICT_HPP_
#define FREQ_CRDICT_HPP_

#include <iostream>
#include <cstdint>
#include <vector>
#include <stack>
#include "constant_numbers.hpp"
#include "common_functions.hpp"

class FreqCRDict{ //constant space reverse dictionary for frequent rules;
private:
  std::vector<uint64_t> var_;
  std::vector<uint64_t> left_;
  std::vector<uint64_t> right_;
  std::vector<uint64_t> freq_;
  std::vector<uint32_t> next_;
  std::vector<uint32_t> hash_table_;
  std::vector<uint32_t> not_used_;

  const uint32_t hash_size_ = kPrimes[16];
  uint64_t num_frules_;
  uint64_t insert_ht_pos_;
  uint32_t num_not_used_;
  uint64_t num_bucket_;

public:
  FreqCRDict(): var_(0),
		left_(0),
		right_(0),
		freq_(0),
		next_(0),
		hash_table_(0),
		not_used_(0),
                num_frules_(0),
                insert_ht_pos_(0),
		num_not_used_(0){};
  ~FreqCRDict(){};
  
  void Init();
  uint64_t GetVar(const uint64_t kLeft,
		  const uint64_t kRight);
  void AddRule(const uint64_t kVar,
		   const uint64_t kLeft,
		   const uint64_t kRight);
  uint64_t ByteSize() const;
private:
  void InsertToHash(const uint64_t kInsertPos);
  void DeleteFromHash(const uint64_t kPos);
  void DeleteNotFreqRules();
  void PrintCRDict();

};

#endif // FREQ_CRDICT_HPP_
