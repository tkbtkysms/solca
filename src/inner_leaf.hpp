/* inner_leaf.hpp
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

#ifndef INNER_LEAF_HPP_
#define INNER_LEAF_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdint>
#include "flc_vector.hpp"

namespace comp {

  class InnerLeaf {  
  private:
    std::vector<FLCVector> elems_;
    std::vector<uint64_t> num_each_bit_elems_;
    int init_bit_len_;
  int bit_len_;
    uint64_t num_elems_;
    
  public:
    InnerLeaf() : elems_(), 
		  num_each_bit_elems_(),
		  init_bit_len_(0), 
		  num_elems_(0) {}
    ~InnerLeaf(){}
  
    void Init(const uint64_t kInitBitLen);
    uint64_t Get(const uint64_t kIndex,
		 const uint64_t kMax) const;
    void PushBack(const uint64_t kVar,
		const uint64_t kMax);
    uint64_t NumElems() const;
    void Clear();
    uint64_t Space() const;
    double CompRatio() const;
    void Write(std::ostream& os);
    void Read(std::istream& is);
  
  }; // class InnerLeaf
  
} // namespace comp

#endif // INNER_LEAF_HPP_
