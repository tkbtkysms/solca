/* succinct_poslp.cpp
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
#include "succinct_poslp.hpp"

using namespace std;

namespace solca_comp
{

  //initilization and clear of data structures
  void SucPOSLP::Init(const string &kOutputFileName)
  {
    sfbt_.Init();
    inner_leaf_.Init(9);
    outer_leaf_.Init();
    buf_.clear();
    buf_size_ = 0;
    vector<uint64_t>().swap(buf_);
    num_rules_ = kAlphabetSize;
    output_file_name_ = kOutputFileName;
    freq_crdict_.Init();
  }

  void SucPOSLP::Clear()
  {
    sfbt_.Clear();
    inner_leaf_.Clear();
    outer_leaf_.Clear();
    vector<uint64_t>().swap(buf_);
    buf_size_ = 0;
    num_rules_ = 0;
  }

  //accessor
  uint64_t SucPOSLP::NumRules() const
  {
    return num_rules_;
  }

  //pushing back a symbol to data structures.
  void SucPOSLP::PushLastCP()
  {
    sfbt_.PushBack(kCP);
    ofstream ofs;
    ofs.open(output_file_name_.c_str());
    Save(ofs);
    ofs.close();
  }

  void SucPOSLP::PushBuffers(const uint64_t kBufSize)
  {
    for (uint64_t i = 0; i != kBufSize; ++i)
    {
      sfbt_.PushBack(kOP);
      inner_leaf_.PushBack(buf_[i], num_rules_);
    }
    buf_.clear();
    buf_size_ = 0;
  }

  uint64_t SucPOSLP::PushInnerSFBT()
  {
    PushBuffers(buf_size_);
    sfbt_.PushBack(kCP);

    return num_rules_++;
  }

  uint64_t SucPOSLP::PushOuterSFBT()
  {
    PushBuffers(buf_size_ - 2);
    sfbt_.PushBack(kOP);
    sfbt_.PushBack(kOP);
    sfbt_.PushBack(kCP);
    return num_rules_++;
  }

  uint64_t SucPOSLP::AccessLeaf(const uint64_t kInd)
  {
    uint64_t kOuterRank = sfbt_.OuterRank(kInd);
    if (sfbt_.IsOuter(kInd))
    {
      if (sfbt_.IsLeftChild(kInd))
      {
        return outer_leaf_.Left(kOuterRank - 1);
      }
      else
      {
        return outer_leaf_.Right(kOuterRank - 1);
      }
    }
    else
    {
      return inner_leaf_.Get(sfbt_.LeafRank(kInd) - (kOuterRank << 1) - 1,
                             sfbt_.InRank(kInd) + kAlphabetSize);
    }
  }

  //operations for accessing production rules.
  uint64_t SucPOSLP::Left(const uint64_t kVar)
  {
    if (kVar < kAlphabetSize)
    {
      return kVar;
    }
    uint64_t kSFBTPos = sfbt_.LeftChild(sfbt_.InSelect(Var2SFBTVar(kVar) + 1));
    if (sfbt_.IsLeaf(kSFBTPos))
    {
      uint64_t kOuterRank = sfbt_.OuterRank(kSFBTPos);
      if (sfbt_.IsOuter_LC(kSFBTPos))
      {
        return outer_leaf_.Left(kOuterRank - 1);
      }
      else
      {
        return inner_leaf_.Get(sfbt_.LeafRank(kSFBTPos) - (kOuterRank << 1) - 1,
                               sfbt_.InRank(kSFBTPos) + kAlphabetSize);
      }
    }
    return SFBTVar2Var(sfbt_.InRank(kSFBTPos) - 1);
  }

  uint64_t SucPOSLP::Right(const uint64_t kVar)
  {
    if (kVar < kAlphabetSize)
    {
      return kVar;
    }
    uint64_t kSFBTPos = sfbt_.RightChild(sfbt_.InSelect(Var2SFBTVar(kVar) + 1));
    if (sfbt_.IsLeaf(kSFBTPos))
    {
      uint64_t kOuterRank = sfbt_.OuterRank(kSFBTPos);
      if (sfbt_.IsOuter_RC(kSFBTPos))
      {
        return outer_leaf_.Right(kOuterRank - 1);
      }
      else
      {
        return inner_leaf_.Get(sfbt_.LeafRank(kSFBTPos) - (kOuterRank << 1) - 1,
                               sfbt_.InRank(kSFBTPos) + kAlphabetSize);
      }
    }
    return SFBTVar2Var(sfbt_.InRank(kSFBTPos) - 1);
  }

  uint64_t SucPOSLP::ParentOfInNode(const uint64_t kVar, bool &isleft)
  {

    if (kVar >= num_rules_ - 1 || kVar < kAlphabetSize)
    {
      return kDummyCode;
    }

    uint64_t tree_pos = sfbt_.InSelect(Var2SFBTVar(kVar) + 1);
    isleft = sfbt_.IsLeftChild(tree_pos);
    uint64_t x = SFBTVar2Var(sfbt_.InRank(sfbt_.Parent(tree_pos)) - 1);
    return x;
  }

  //Given two variables left.symbol and right.symbol,
  //This returns the Node ret if a production rule ret.symbol -> (left.symbol, right.symbol)
  //exists in now production rules otherwise returns new Node ret corresponding to
  // the new production rule ret.symbol -> (left.symbol, right.symbol).
  // In advance, this adds Node ret to the succinct data sturucture of POSLP.
  Node SucPOSLP::ReverseAccessAndUpdate(const Node &left,
                                        const Node &right)
  {
    Node ret;

    //Search if (left.symbol,right.symbol) exists in
    // the constant space reverse dictionary for frequent reverse access
    if ((ret.symbol = freq_crdict_.GetVar(left.symbol, right.symbol)) != kDummyCode)
    {
      ret.is_inner = false;
      PushToBuffer(ret.symbol);
    }
    else
    {
      //The production rule V-> left.symbol right.symbol does not exit in POSLP
      //if either or both of left and right is a root of now sub trees.
      if (left.is_inner || right.is_inner)
      {
        ret.symbol = PushInnerSFBT();
        ret.is_inner = true;
      }
      else
      {
        bool is_left = true;
        // check if the production rule V->left.symbol right.symbol
        // including the internal node of either or both of left and right node
        // is in POSLP.
        if (left.symbol >= right.symbol)
        {
          // If left.symbol >= rigth.symbol,
          //there is a possibility that the production rule V->(left.symbol , right.symbol)
          // that the form of left node is an internal node exits in now POSLP.
          ret.symbol = ParentOfInNode(left.symbol, is_left);
          if (ret.symbol != kDummyCode && is_left)
          {
            if (Right(ret.symbol) == right.symbol)
            {
              ret.is_inner = false;
              PushToBuffer(ret.symbol);
              freq_crdict_.AddRule(ret.symbol,
                                   left.symbol,
                                   right.symbol);
              return ret;
            }
          }
        }
        else
        {
          //if left.symbol < right.symbol,
          //there is a possibility that the production rule V->(left.symbol , right.symbol)
          // that the form of right node is an internal node exits in now POSLP.
          ret.symbol = ParentOfInNode(right.symbol, is_left);
          if (ret.symbol != kDummyCode && !is_left)
          {
            if (Left(ret.symbol) == left.symbol)
            {
              ret.is_inner = false;
              PushToBuffer(ret.symbol);
              freq_crdict_.AddRule(ret.symbol,
                                   left.symbol,
                                   right.symbol);
              return ret;
            }
          }
        }
        //check if the production rule exists in POSLP
        // as the both form of left and right node is a leaf.
        uint64_t tmp_code = outer_leaf_.ReverseAccessAndAdd(left.symbol,
                                                            right.symbol);
        if (tmp_code == kDummyCode)
        {
          ret.symbol = PushOuterSFBT();
          ret.is_inner = true;
        }
        else
        {
          uint64_t tpos = sfbt_.OuterSelect(tmp_code + 1) + 2;
          ret.symbol = SFBTVar2Var(sfbt_.InRank(tpos) - 1);
          ret.is_inner = false;
          PushToBuffer(ret.symbol);
        }
      }
      freq_crdict_.AddRule(ret.symbol,
                           left.symbol,
                           right.symbol);
    }

    return ret;
  }

  //get the byte size of each data structure.
  uint64_t SucPOSLP::ByteSizeOfB() const
  {
    return sfbt_.ByteSize();
  }

  uint64_t SucPOSLP::ByteSizeOfL1() const
  {
    return inner_leaf_.Space();
  }

  uint64_t SucPOSLP::ByteSizeOfL2L3() const
  {
    return outer_leaf_.ByteSize();
  }

  uint64_t SucPOSLP::ByteSizeOfL() const
  {
    return outer_leaf_.ByteSize() + inner_leaf_.Space();
  }

  uint64_t SucPOSLP::ByteSizeOfPOSLP() const
  {
    return ByteSizeOfB() + ByteSizeOfL();
  }

  uint64_t SucPOSLP::ByteSizeOfOther() const
  {
    return sizeof(buf_[0]) * buf_size_ + sizeof(SucPOSLP);
  }

  uint64_t SucPOSLP::ByteSizeOfHash() const
  {
    return freq_crdict_.ByteSize();
  }

  uint64_t SucPOSLP::ByteSize() const
  {
    return ByteSizeOfPOSLP() + ByteSizeOfOther() + ByteSizeOfHash();
  }
  //extract the substring derived by kVar.
  void SucPOSLP::Decode(const uint64_t kVar,
                        ofstream &ofs)
  {
    if (kVar < kAlphabetSize)
    {
      ofs << (char)kVar;
    }
    else
    {
      Decode(Left(kVar),
             ofs);
      Decode(Right(kVar),
             ofs);
    }
  }

  //save the data
  void SucPOSLP::Save(ofstream &ofs)
  {
    FLCVector output;
    uint64_t bit_pos = 0;
    uint64_t leaf_bits = 9;
    uint64_t max_code = 1 << leaf_bits;
    uint64_t tmp_leaf = 0;
    uint64_t inner_rank = kAlphabetSize;
    uint64_t leaf_rank = 0;

    output.Init(2, 64);
    sfbt_.Save(ofs);
    for (uint64_t i = 0; i < sfbt_.Length(); i++)
    {
      if (sfbt_.Get(i) == kOP)
      {
        tmp_leaf = AccessLeaf(leaf_rank + inner_rank - kAlphabetSize);
        output.SetBits(bit_pos,
                       tmp_leaf,
                       leaf_bits);
        bit_pos += leaf_bits;
        if (bit_pos >= kBlockSize)
        {

          output.Save(ofs, 1, kBlockSize);
          output.SetBits(0,
                         output.GetBits(kBlockSize, kBlockSize),
                         kBlockSize);
          output.SetBits(kBlockSize,
                         0,
                         kBlockSize);
          bit_pos = bit_pos - kBlockSize;
        }
        leaf_rank++;
      }
      else
      {
        inner_rank++;
        if (inner_rank >= max_code)
        {
          leaf_bits++;
          max_code = 1 << leaf_bits;
        }
      }
    }
    if (bit_pos != 0)
    {
      output.Save(ofs, 1, kBlockSize);
    }
  }

} // namespace solca_comp
