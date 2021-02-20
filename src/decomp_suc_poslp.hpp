/* decomp_suc_poslp.hpp
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

#ifndef DECOMP_SUC_POSLP_HPP_
#define DECOMP_SUC_POSLP_HPP_

#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "constant_numbers.hpp"
#include "suc_fbtree.hpp"
#include "inner_leaf.hpp"

namespace solca_comp
{
  //the succinct data structure of post-order straight line program for decompression
  //this supports accessing production rules in constant time
  //using 2n + n\lg (n + \sigma) + o(n) bits of space.
  class DSucPOSLP
  {
  private:
    //sunccinct data structure
    SucFBTree sfbt_;
    InnerLeaf leave_;

  public:
    //constructor and destructor
    DSucPOSLP() : sfbt_(),
                  leave_(){};
    ~DSucPOSLP(){};

    uint64_t Decompress(const std::string &kInputFileName,
                        const std::string &kOutputFileName);

  private:
    //accessing POSLP
    uint64_t Left(const uint64_t kVar);
    uint64_t Right(const uint64_t kVar);

    //decompression method
    void RepeatDecompress(const uint64_t kVar,
                          std::ofstream &ofs);
    //save and load
    void Load(std::ifstream &ifs);

    // transformation of the variable of post order straight line program
    //to the variable of succinct full binary tree
    // and the reversed transformation
    inline uint64_t Var2SFBTVar(const uint64_t kVar);
    inline uint64_t SFBTVar2Var(const uint64_t kVar);
  }; // class DSucPOLSP

  //inline implementations
  inline uint64_t DSucPOSLP::Var2SFBTVar(const uint64_t kVar)
  {
    if (kVar < kAlphabetSize)
    {
      return kDummyCode;
    }
    return kVar - kAlphabetSize;
  }

  inline uint64_t DSucPOSLP::SFBTVar2Var(const uint64_t kVar)
  {
    return kVar + kAlphabetSize;
  }
} //namespace solca_comp

#endif // DECOMP_SUC_POSLP_HPP_
