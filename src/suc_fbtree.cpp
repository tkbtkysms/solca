/* suc_fbtree.cpp
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
#include "suc_fbtree.hpp"

using namespace std;


////////////////////////////////////////////////////////////////
// Initialization and clear
////////////////////////////////////////////////////////////////

void SucFBTree::Init(){
  obp_.Init();
}

void SucFBTree::Clear(){
  obp_.Free();
}

////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////

size_t SucFBTree::ByteSize() const{
  return sizeof(SucFBTree) + obp_.Size();
}

size_t SucFBTree::Length(){
  return obp_.Length();
}

void SucFBTree::PushBack(const int c){
  obp_.Push(c);
}

uint64_t SucFBTree::OuterRank(uint64_t i)
{
  return obp_.OuterRank(i);
}

uint64_t SucFBTree::OuterSelect(uint64_t i)
{
  return obp_.OuterSelect(i);
}

////////////////////////////////////////////////////////////////
//        postorder full binary tree‚É‘Î‚·‚é‘€ì
////////////////////////////////////////////////////////////////

uint64_t SucFBTree::RightChild(uint64_t i)
{
  return i - 1;
}

uint64_t SucFBTree::LeftChild(uint64_t i)
{
  return obp_.Bwd(i, 0);
}


uint64_t SucFBTree::InSelect(uint64_t i) {
  return obp_.Select(i, kCP);
}

uint64_t SucFBTree::LeafSelect(uint64_t i) {
  return obp_.Select(i, kOP);
}

uint64_t SucFBTree::InRank(uint64_t i) {
    return obp_.Rank(i, kCP);
}

uint64_t SucFBTree::LeafRank(uint64_t i) {
  return obp_.Rank(i, kOP);
}

uint64_t SucFBTree::LeftMostLeaf(uint64_t i)
{
  if (obp_.Get(i)) {
    return i;
  }
  uint64_t ret = obp_.Bwd(i, -1);
  if (ret != NOT_FOUND) {
    return ret + 1;
  } else {
    return 0;
  }
}

//??
uint64_t SucFBTree::RightMostLeaf(uint64_t i)
{
  return obp_.Select(obp_.Rank(i, kOP), kOP); 
}

bool SucFBTree::IsLeaf(uint64_t i) {
  if (obp_.Get(i) == kOP) return true;
  return false;
}

// A parent might not be fixed. It should be checked by user of this func.
int SucFBTree::IsRightChild(uint64_t i)
{
  if (!(obp_.Get(i+1))) {
    return 1; // i is a right child.
  } else {
    return 0;
  }
}

// A parent might not be fixed. It should be checked by user of this func.
int SucFBTree::IsLeftChild(uint64_t i)
{
  if (!(obp_.Get(i + 1))) {
    return 0;
  } else {
    return 1; // i is a left child.
  }
}

bool SucFBTree::IsOuter_LC(uint64_t i) {
  if (obp_.GetBits(i, 3) == 6) return true;
  return false;
}

bool SucFBTree::IsOuter_RC(uint64_t i) {
  if (obp_.GetBits(i-1, 3) == 6) return true;
  return false;
}

void SucFBTree::Save(ofstream &ofs){
  obp_.Save(ofs);
}
  
void SucFBTree::Load(ifstream &ifs){
  obp_.Load(ifs);
}

uint64_t SucFBTree::Parent(uint64_t i)
{
  if (i >= obp_.Length() - 1) {
    return NOT_FOUND;
  } else if (IsRightChild(i)) {
    return i +1 ;
  } else {
    return obp_.Fwd(i, 0);
  }
}
