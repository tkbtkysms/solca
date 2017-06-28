/* suc_fbtree.hpp
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

#ifndef SUC_FBTREE_HPP
#define SUC_FBTREE_HPP

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "online_bp.hpp"

//succinct data sturucture for full binary tree

class SucFBTree{
private:
  OnlineBP obp_;
  
public:
  SucFBTree(): obp_(){}
  ~SucFBTree(){}

  //operations for member's space
  void Init();
  void Clear();

  //accessors for bp
  size_t Length();
  size_t ByteSize() const;

  //set a bit c to the tail of BP
  void PushBack(const int c);
  
  // rank/seelct operations
  uint64_t Rank(uint64_t i,
		int c);
  uint64_t Select(uint64_t i,
		  int c);
  uint64_t OuterRank(uint64_t i);
  uint64_t OuterSelect(uint64_t i);
  
  // operations for postiorder full binary tree
  uint64_t RightChild(uint64_t i);
  uint64_t LeftChild(uint64_t i);
  uint64_t Parent(uint64_t i);
  uint64_t InSelect(uint64_t i);
  uint64_t LeafSelect(uint64_t i);
  uint64_t InRank(uint64_t i);
  uint64_t LeafRank(uint64_t i);  
  int IsRightChild(uint64_t i);
  int IsLeftChild(uint64_t i);
  bool IsLeaf(uint64_t i);
  uint64_t LeftMostLeaf(uint64_t i);
  uint64_t RightMostLeaf(uint64_t i);
  bool IsOuter_LC(uint64_t i);
  bool IsOuter_RC(uint64_t i);
  
  //save and load
  void Save(std::ofstream &ofs);  
  void Load(std::ifstream &ifs);
}; //class SucFBTree


#endif // SUC_FBTree_HPP
