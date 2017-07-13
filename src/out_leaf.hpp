/* out_leaf.hpp
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

#ifndef OUT_LEAF_HPP
#define OUT_LEAF_HPP

#include "RSDic.hpp"
#include "permutation.hpp"
#include "nrs_dic.hpp"
#include "flc_vector.hpp"

namespace solca_comp{

  class OutLeaf{

  private:
    //prefix is represented by GMR.
    rsdic::RSDic left_gb_;
    Permutation  left_perm_;
    rsdic::RSDic right_gb_;
    Permutation  right_perm_;
    uint64_t     prefix_len_;

    //suffix is represented by naive rsdic.
    NRSDic       suf_left_;
    NRSDic       suf_right_;
    uint64_t     suffix_max_len_;
    bool         is_first_build_;

    //hash table for accessing i from suf_left_[i] and suf_right_[i]
    EFLCVector    hash_table_;
    EFLCVector    hash_list_;
    uint64_t     hash_size_;
    uint64_t     primes_index_;

    //hash table for updating prefix
    EFLCVector    u_hash_table_;
    EFLCVector    u_hash_list_;
    std::vector<FLCVector>  pos_array_;
    uint64_t     bits_per_pos_;
    EFLCVector    num_pos_;
    rsdic::RSDic ib_; //storing whether a postion in left_perm_ is inserted
    // a sumbol by updating of left_perm_.

    uint64_t max_byte_size_;

  public:
    OutLeaf(): left_gb_(), 
	       left_perm_(),
	       right_gb_(),
	       right_perm_(),
	       prefix_len_(),
	       suf_left_(),
	       suf_right_(),
	       suffix_max_len_(),
	       is_first_build_(),
	       hash_table_(),
	       hash_list_(),
	       hash_size_(),
	       primes_index_(),
	       u_hash_table_(),
	       u_hash_list_(),
	       pos_array_(),
	       bits_per_pos_(),
	       num_pos_(),
	       ib_(),
	       max_byte_size_(){}
    ~OutLeaf() {}
    void     Init     ();
    uint64_t ReverseAccessAndAdd(const uint64_t kVar1,
				 const uint64_t kVar2); 
    uint64_t ReverseAccess   (const uint64_t kVar1,
			      const uint64_t kVar2); 
    uint64_t Left           (const uint64_t kPos);
    uint64_t Right          (const uint64_t kPos);
    uint64_t PreLeftRank(const uint64_t kVar,
			 const uint64_t kPos);
    uint64_t LeftRankAll(const uint64_t kVar);
    uint64_t PreRightRank(const uint64_t kVar,
			  const uint64_t kPos);
    uint64_t PreLeftSelect(const uint64_t kVar,
			   const uint64_t kPos);
    uint64_t PreRightSelect(const uint64_t kVar,
			    const uint64_t kPos);
    uint64_t SufLeftSelect(const uint64_t kVar,
			   FLCVector &result);
    uint64_t SufRightSelect(const uint64_t kVar,
			    FLCVector &result);
    uint64_t Len(){
      return prefix_len_ + suf_left_.Num();
    }
    uint64_t PreLen(){
      return prefix_len_;
    }
    uint64_t LeftBPD(){
      return suf_left_.BPH();
    }

    uint64_t RightBPD(){
      return suf_right_.BPH();
    }
    void     Save     (std::ofstream &ofs);
    void     Load     (std::ifstream &ifs);
    uint64_t ByteSize () const;
    void    Clear();

  private:
    void     InitUHashTable   ();
    void     DeleteUHashTable ();
    uint64_t SearchRule       (const uint64_t kVar1,
			       const uint64_t KVar2); 
    uint64_t PreReverseAccess (const uint64_t kVar1,
			       const uint64_t kVar2);
    uint64_t SufReverseAccess (const uint64_t kVar1,
			       const uint64_t kVar2);
    void     AddTable         (const uint64_t kIndex);
    void     AddList          (const uint64_t kIndex);
    void     FirstBuildPrefix (); 
    void     FirstBuildPreLeft (); 
    void     AddPos           (const uint64_t kPos); 
    void     AddUTable        (const uint64_t kIndex,
			       const uint64_t kPos); 
    void     AddUList         (const uint64_t kIndex,
			       const uint64_t kPos); 
    void     FirstBuildPreRight ();
    uint64_t GetPos           (const uint64_t kVar,
			       FLCVector   &result); 
    void     ReBuildPrefix    ();
    void     ReBuildPreLeft   ();
    void     ReBuildPreRight  ();
    rsdic::RSDic  BuildLeftEB ();
    rsdic::RSDic  BuildRightEB();
    void     ClearSuffix();
    void     Resize();
    void     ResizeHash();
    uint64_t UpdateSuffixMaxLen();
    uint64_t PreSelectInRightPerm(const uint64_t kVar,
				  const uint64_t kPos);
    uint64_t Space() const;
  };

}//namespace solca_comp
#endif //OUT_LEAF_HPP
