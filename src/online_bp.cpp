/* online_bp.cpp
MIT License

Copyright (c) 2017 Tomohiro I

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

#include "online_bp.hpp"
#ifdef ONLINEBP_MAIN
#include <time.h>
#endif
//#define DEBUG

using namespace std;

// operations for bit array

uint64_t OnlineBP::FwdSel(uint64_t block_pos,
			  uint64_t i)
{
  uint64_t ret = 0;
  uint64_t ret64;
  do {
    ret64 = FwdSel64(BP_.GetBlock(block_pos++), i);
    ret += ret64;
  } while (ret64 == 64);

  return ret;
}

uint64_t OnlineBP::FwdSel_0(uint64_t block_pos,
			    uint64_t i)
{
  uint64_t ret = 0;
  uint64_t ret64;
  do {
    ret64 = FwdSel64(~(BP_.GetBlock(block_pos++)), i);
    ret += ret64;
  } while (ret64 == 64);

  return ret;
}

uint64_t OnlineBP::FwdSelOuter(uint64_t block_pos,
			       uint64_t i)
{
  uint64_t ret = 0;
  uint64_t ret64;
  do {
    ret64 = FwdSelOuter64(BP_.GetBlock(block_pos),
			  BP_.GetBlock(block_pos + 1),
			  i);
    ++block_pos;
    ret += ret64;
  } while (ret64 == 64);

  return ret;
}

////////////////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////////////////

size_t OnlineBP::Size() const{
  return size_;
}

size_t OnlineBP::Length()
{
  return n_;
}

int OnlineBP::Alpha(){
  return kAlpha;
}

int OnlineBP::L(){
  return kBlock.size;
}

void OnlineBP::Init()
{

  size_ = sizeof(OnlineBP);
  n_ = 0;
  last_ex_ = 0;
  N_ = 0;
  curOuterRank_ = 0;
  BP_.Init(0);
  exA_.clear();
  exA_.push_back(0);
  CFunc::ResizeVec(minA_, LEVEL_OF_MINA);
  for (int i = 0; i < LEVEL_OF_MINA; ++i) {
    minA_[i].clear();
    minA_[i].push_back(1);
    minA_[i].push_back(0);
  }
  minNextA_.clear();
  minPrevA_.clear();
  outerRankLA_.clear();
  outerRankHA_.clear();
  outerRankHA_.push_back(0);
  outerSelHA_.clear();
  outerSelHA_.push_back(1);
  outerSelHA_.push_back(0);
}

void OnlineBP::Free()
{
  BP_.Delete();
  vector<uint8_t> ().swap(exA_);
  for (int i = 0; i < LEVEL_OF_MINA; ++i) {
    vector<uint8_t> ().swap(minA_[i]);
  }
  vector<vector<uint8_t> > ().swap(minA_);
  vector<uint64_t> ().swap(minNextA_);
  vector<uint64_t> ().swap(minPrevA_);
  vector<uint8_t> ().swap(outerRankLA_);
  vector<uint64_t> ().swap(outerRankHA_);
  vector<uint64_t> ().swap(outerSelHA_);
}


void OnlineBP::Push(const int c)
{
  uint64_t N = N_;
  const uint64_t n = n_;
  const uint32_t L = kBlock.size;
  
  if (n + 2 >= N) { // +2 is to avoid accessing out of bounds in rank/select of '110'.
    // BP の領域を拡張
    const uint64_t N_old = N;
    N_ = N = std::max((uint64_t)(N * kAlpha / (kAlpha-1)), (uint64_t)L);
    size_ += ((N-N_old)/kMidBlock.size)*sizeof(uint64_t); // for BP
    size_ += ((N-N_old)/L) * sizeof(exA_[0]);
    size_ += (N-N_old)/L * sizeof(outerRankLA_[0]);
    size_ += ((N-N_old) >> kOuterH.lg) * sizeof(outerRankHA_[0]);
    size_ += ((N-N_old) >> kOuterH.lg) * sizeof(outerSelHA_[0]);
    uint64_t size_at_some_level_old = (N_old+L-1)/L;
    uint64_t size_at_some_level_new = (N+L-1)/L;
    int32_t level = 0;
    while (level < LEVEL_OF_MINA && size_at_some_level_new > 1) {

      size_ += (size_at_some_level_new - size_at_some_level_old)*sizeof(minA_[level][0]);
      size_at_some_level_new += kArity.size - 1;
      size_at_some_level_new >>= kArity.lg;
      size_at_some_level_old += kArity.size - 1;
      size_at_some_level_old >>= kArity.lg;
      ++level;
    }
    if (level >= LEVEL_OF_MINA && size_at_some_level_new > 1) {
      size_ += (size_at_some_level_new - size_at_some_level_old)*sizeof(minPrevA_[0]);
      size_ += (size_at_some_level_new - size_at_some_level_old)*sizeof(minNextA_[0]);
    }
  }
  // n は追加前の長さ (== 新しいビットを追加する位置(0始まり)), 追加後長さを 1 増やす
  BP_.PushBack(c);
  
  uint64_t idx = n >> kBlock.lg; //the last block of exA 
  if((n & kBlock.lo_bits) == 0){ 
    exA_.push_back(0);
  }
  exA_[idx + 1] = last_ex_ += (c << 1) - 1;
  int32_t level = 0;//idx = the last block of level 0 of minA
  //n/(L*kArity.size) = n/(512*32) = the last block of level 1 of minA 
  if ((n & kBlock.lo_bits) == 0) { // Lブロックの最初に追加
    while (idx > 0) {
      minA_[level].push_back(0);
      minA_[level][idx] = last_ex_;
      minA_[level][idx + 1] = 0;
      ++level;
      if (level >= LEVEL_OF_MINA) {
        goto high;
      }
      if ((idx & kArity.lo_bits) != 0) {
	idx >>= kArity.lg;
        break;
      }
      
      idx >>= kArity.lg; // /= 32
    }
  }
  while (idx > 0 && level < LEVEL_OF_MINA && minA_[level][idx] > last_ex_) {
    minA_[level][idx] = last_ex_;
    ++level;
    idx >>= kArity.lg;
  }
  goto last;

 high:
  if ((idx & kArity.lo_bits) == 0) { // compute minNextA and minPrevA
    --level;//level = 1
    uint8_t min = UINT8_MAX;
    for (uint32_t t = 0; t < kArity.size; ++t) {
      min = std::min(minA_[level][--idx], min);
    }
    idx >>= kArity.lg;
    minNextA_.push_back(min);
    minPrevA_.push_back(min);
    if (idx) {
      uint64_t idx_p = idx - 1;
      uint64_t value = minNextA_[idx_p];
      while ((value & 0xffULL) > min) {
        minNextA_[idx_p] &= 0xffULL;
        minNextA_[idx_p] |= (idx << 8);
        idx_p = (value >> 8);
        value = minNextA_[idx_p];
      }
      minNextA_[idx] |= (idx_p << 8);
      if ((value & 0xffULL) == min) {
        idx_p = (minPrevA_[idx_p] >> 8);
      }
      minPrevA_[idx] |= (idx_p << 8);
    }
  }

 last:
  if (n >= 2) { // update arrays for outer rank/selec
    if(((n - 2) & kBlock.lo_bits) == 0){
      outerRankLA_.push_back(0);
    }
    if (BP_.GetBits(n - 2, 3) == 6) {
      ++(curOuterRank_);
      ++(outerRankLA_[(n-2) >> kBlock.lg]);
      if((curOuterRank_ & kOuterH.lo_bits) == 0){
	outerSelHA_.push_back(0);
      }
    }
    if(((n -2) & kOuterH.lo_bits) == 0){
      outerRankHA_.push_back(0);
    }
    const uint64_t idx_rha = ((n-2) >> kOuterH.lg) + 1;
    outerRankHA_[idx_rha] = curOuterRank_;
    outerSelHA_[(curOuterRank_ >> kOuterH.lg) + 1] = idx_rha;
  }

  n_ = n + 1;
}

////////////////////////////////////////////////////////////////
//        Operations for parentheses
//////////////////////////////////////////////////////////////
// return excess[i]
uint32_t OnlineBP::Depth(uint64_t i)
{
  uint32_t q = i & kBlock.lo_bits; // position in block L
  if (q == kBlock.size - 1) {
    return exA_[(i+1) >> (kBlock.lg)];
  }
  uint64_t idx = i >> (kBlock.lg);
  return exA_[idx] + (FwdCnt((idx << (kMidInBlock.lg)), q) << 1) - (q + 1);
}


// return # of occ of c in [0..i]
uint64_t OnlineBP::Rank(uint64_t i,
			int c)
{
  uint64_t d = Depth(i);
  if (c == kOP) {
    return (d+i+1)/2; // # of kOP in [0..i]
  } else {
    return (i+1-d)/2; // # of kCP in [0..i]
  }
}


// return the position (zero-indexed) of i-th (one-indexed) occ of c
// !NOTE! Assume that the answer exists.
// Assume that excess is in [1, 127].
uint64_t OnlineBP::Select(uint64_t i,
			  int c)
{
  if (c == kOP) {
    // Because 2 i = ret + 1 + excess[ret], ret = 2 i - 1 - excess[ret] >= 2 i - 128
    uint64_t j = ((i << 1) < 128) ? 0 : ((i << 1) - 128) >> (kBlock.lg);
    while ((((j+1) << (kBlock.lg)) + exA_[j+1])/2 < i) {
      ++j;
    }
    uint64_t remain = i - ((j << (kBlock.lg)) + exA_[j])/2;
    return (j << (kBlock.lg)) + FwdSel((j << kMidInBlock.lg), remain);
  } else {
    // Because 2 i = ret + 1 - excess[ret], ret = 2 i + 1 - excess[ret] >= 2 i
    uint64_t j = i >> (kBlock.lg - 1);
    while ((((j+1) << (kBlock.lg)) - exA_[j+1])/2 < i) {
      ++j;
    }
    uint64_t remain = i - ((j << (kBlock.lg)) - (exA_[j]))/2;
    return (j << kBlock.lg) + FwdSel_0((j << kMidInBlock.lg), remain);
  }
}


uint64_t OnlineBP::OuterRank(uint64_t i)
{
  uint64_t ret = outerRankHA_[i >> kOuterH.lg];
  const uint64_t last = ((i+1) >> kBlock.lg);
  for (uint64_t block = ((i & kOuterH.hi_bits) >> kBlock.lg); block != last; ++block) {
    ret += outerRankLA_[block];
  }
  const uint32_t q = i & kBlock.lo_bits; // Lブロック内の位置
  if (q == kBlock.size - 1) {
    return ret;
  } else {
    return ret + FwdCntOuter(((i & kBlock.hi_bits) >> kMidBlock.lg), q);
  }
}

uint64_t OnlineBP::OuterSelect(uint64_t i)
{
  uint64_t idx = (i >> kOuterH.lg);
  uint64_t *hblock = std::lower_bound(&outerRankHA_[outerSelHA_[idx]],
				      &outerRankHA_[outerSelHA_[idx+1]],
				      i);
  --hblock;
  i -= *hblock;
  uint64_t ret = (hblock - &outerRankHA_[0]) * kOuterH.size;
  uint64_t block = (ret >> kBlock.lg);
  while (i > outerRankLA_[block]) {
    i -= outerRankLA_[block++];
    ret += kBlock.size;
  }
  return ret + FwdSelOuter((ret >> kMidBlock.lg), i);
}


//! Investigate words (a part of bwd_search). The ending position in the first word is passed by end_pos.
//! \param end_pos: in [0..63]. We are going to investigate bitvector ... x_{-2} x_{-1} x_{0}[0..end_pos]
//! \param num_words: We are going to investigate bitvector x_{-(num_words+1)} ... x_{-2} x_{-1} x_{0}[0..end_pos]
//! \param d: A relative value to the target excess value from excess value for x_{0}[end_pos].
//! \return Return the relative position "ret" from x_{-(num_words+1)}[0] such that the position "ret-1" achieves the target excess value.
//!         If such does not exist return NOT_FOUND.
//  Example: If d == -2, w[0..7] == 11011101, then return 5.
//           Suppose that e[0] == 3, then e[0..7] == 34345656.
//           See that 5 is the position where (# of 0's) - (# of 1's) == -2 (scanning from right to left),
//           and then the target excess value is achieved at 5 - 1 == 4.
uint64_t OnlineBP::BwdWords(uint64_t block_pos,
			    const uint32_t end_pos,
			    const uint32_t num_words,
			    int32_t &d)
{
  // For the first word, we fill dummy bits (OP) to align ETW block boundary.
  // Also we change x_0[end_pos] to OP when x_[end_pos] == CP.
  // So modify d appropriately here.
  d -= (ETW - (end_pos % ETW)) - (BP_[(block_pos << kMidBlock.lg) + end_pos] << 1) + 1;
  uint32_t processed_len_in_word = kMidBlock.size - ((end_pos/ETW)+1)*ETW;
  // We change bits in x_0[end_pos..63] to OP and align to right deleting unused ETW blocks.
  uint64_t x = (BP_.GetBlock(block_pos--) | (0xffffffffffffffffULL >> end_pos)) >> processed_len_in_word;
  for (uint32_t t = num_words; t > 0; --t) {
    do {
      uint32_t w = x & ((1<<ETW)-1);
      if (d + (int32_t)ETW >= 0) { // 答えがある可能性がある
        uint32_t r = kBwdTbl[((d+ETW)<<ETW)+w];
        if (r < ETW) {
          return t*kMidBlock.size - processed_len_in_word - r - 1; // the answer found
        }
      }
      d += (kCNT_TBL[w] << 1) - ETW;
      x >>= ETW;
      processed_len_in_word += ETW;
    } while (processed_len_in_word < kMidBlock.size);
    processed_len_in_word = 0;
    x = BP_.GetBlock(block_pos--);
  }

  return NOT_FOUND; // not found
}


//! return the position where excess value is e[i] - d for the first time to the left of i.
//! It returns NOT_FOUND if the answer does not exist.
uint64_t OnlineBP::Bwd(const uint64_t i,
		       int32_t d)
{
  uint64_t offset = BwdWords(i/kMidBlock.size,
			     i & kMidBlock.lo_bits,
			     ((i/kMidBlock.size) & kMidInBlock.lo_bits) + 1,
			     d); // offset is zero indexed
  if (offset != NOT_FOUND) {
    return (i & kBlock.hi_bits) + offset - 1; // -1 is needed for the nature of bwd_search (it is not symmetry to fwd_search).
  }

  // Find the block where target exists by using min tree.
  uint64_t idx = i >> (kBlock.lg); // Let (idx-1) point to the cell we are going to start investigation.
  const int32_t target = exA_[idx] + d;
  if (target <= 0) {
    return NOT_FOUND;
  }
  int32_t level = 0;
  uint64_t p;
  int32_t p_level;
  while (true) {
    int32_t t = (idx & kArity.lo_bits) + kArity.size; // Investigate t blocks in this level.
    p = idx; // (p-1) points to the cell we are going to start investigation.
    p_level = level;
    while (--t >= 0 && minA_[p_level][--p] > target) {}
    if (t < 0) { // Not found in this level. Go up.
      ++level;
      idx = idx/kArity.size - 1;
      if (level >= LEVEL_OF_MINA) {
        --idx;
        uint64_t value = minPrevA_[idx];
        while ((value & 0xffULL) > (uint64_t)target) {
          idx = value >> 8;
          value = minPrevA_[idx];
        }
        ++idx;
        break;
      }
    } else { // Found the target. Break.
      idx = (idx & kArity.hi_bits) + t + 1 - kArity.size; // (idx-1) is pointing to the cell such that min <= target.
      break;
    }
  }
  while (--level >= 0) {
    idx *= kArity.size;
    p = idx;
    p_level = level;
    while (minA_[p_level][--p] > target) {
      --idx;
    }
  }

  // Screan the block where target exists.
  d = target - exA_[idx];
  if (d == 0) {
    return (idx << (kBlock.lg)) - 1;
  }
  offset = BwdWords(((idx << (kMidInBlock.lg))-1),
		    kMidBlock.size - 1,
		    kMidInBlock.size,
		    d);
  return ((idx-1) << kBlock.lg) + offset - 1;
}


//! Investigate words (a part of fwd_search). The starting position in the first word is passed by beg_pos.
//! \param bitvec: bitvector represented by 64bits uints x_0 x_1 x_2 ...
//! \param beg_pos: in [0..63]. We are going to investigate bitvector x_0[beg_pos..63] x_1 x_2 ...
//! \param num_words: We are going to investigate bitvector x_0[beg_pos..63] x_1 x_2 ... x_{num_words-1}
//! \param d: A relative value to the target excess value from excess value for x_0[beg_pos-1] (out of the bound in this function).
//! \return Return the relative position from x_0[0] where the target excess value is achieved.
//!         If such does not exist return D*num_words.
uint64_t OnlineBP::FwdWords(uint64_t block_pos,
			    const uint32_t beg_pos,
			    const uint32_t num_words,
			    int32_t &d)
{
  // For the first word, we fill dummy bits (CP) to align ETW block boundary.
  // Also we change x_0[end_pos] to CP when x_[end_pos] == OP.
  // So modify d appropriately here.
  d -= (beg_pos % ETW) + (BP_[(block_pos << kMidBlock.lg) + beg_pos] << 1);
  uint32_t processed_len_in_word = (beg_pos/ETW)*ETW;
  // We change bits in x_0[0..beg_pos] to CP and align to left deleting unused ETW blocks.
  uint64_t x = (BP_.GetBlock(block_pos++) & (0x7fffffffffffffffULL >> beg_pos)) << processed_len_in_word;
  for (uint32_t t = 0; t < num_words; ++t) {
    do {
      uint32_t w = x >> 56;
      if (d + (int32_t)ETW >= 0) { // 答えがある可能性がある
        uint32_t r = kFwdTbl[((d+ETW)<<ETW)+w];
        if (r < ETW) {
          return t*kMidBlock.size + processed_len_in_word + r; // found
        }
      }
      d += ETW - (kCNT_TBL[w] << 1);
      x <<= ETW;
      processed_len_in_word += ETW;
    } while (processed_len_in_word < kMidBlock.size);
    processed_len_in_word = 0;
    x = BP_.GetBlock(block_pos++);
  }

  return kMidBlock.size * num_words; // not found
}


// WARNING: [assumption] target (== onlinebp_depth(bp, i) + d) >= 0.
uint64_t OnlineBP::Fwd(const uint64_t i,
		       int32_t d)
{
  uint64_t idx = (i+1)/kMidBlock.size;
  uint64_t num_words = std::min((n_)/kMidBlock.size - idx + 1, kMidInBlock.size - (idx & kMidInBlock.lo_bits));
  uint64_t offset = FwdWords((i+1)/kMidBlock.size,
			     (i+1) & kMidBlock.lo_bits,
			     num_words,
			     d); // offset is zero indexed
  uint64_t ans = ((i+1) & kMidBlock.hi_bits) + offset;
  if (ans >= n_) {
    return NOT_FOUND;
  } else if (ans < ((i+1) & kBlock.hi_bits) + kBlock.size) {
    return ans;
  }

  // Find the block where target exists by using min tree.
  idx = (i+1) >> kBlock.lg; // Let (idx+1) point to the cell we are going to start investigation.
  const int32_t target = exA_[idx+1] + d;
  int32_t level = 0;
  uint64_t p;
  int32_t p_level;
  while (true) {
    int32_t t = (kArity.size << 1) - (idx & kArity.lo_bits) - 1; 
    p = idx; // (p+1) points to the cell we are going to start investigation.
    p_level = level;
    while (--t >= 0 && minA_[p_level][++p] > target){}
    if (t < 0) {
      ++level;
      idx = idx/kArity.size;
      if (level >= LEVEL_OF_MINA) {
        ++idx;
        uint64_t value = minNextA_[idx];
        while ((value & 0xffULL) > (uint64_t)target) {
          if (idx > (value >> 8)) {
            return NOT_FOUND;
          }
          idx = value >> 8;
          value = minNextA_[idx];
        }
        break;
      }
    } else {
      idx = (idx & kArity.hi_bits) + (kArity.size << 1) - t - 1; // idx is now pointing to the cell such that min <= target.
      break;
    }
  }
  if (minA_[p_level][p] == 0) {
    return NOT_FOUND;
  }
  while (--level >= 0) {
    idx *= kArity.size;
    p = idx - 1;
    p_level = level;
    while (minA_[p_level][++p] > target) {
      ++idx;
    }
  }

  // Screan the block where target exists.
  d = target - exA_[idx];
  return (idx << kBlock.lg) + FwdWords((idx << kMidInBlock.lg),
				  0,
				  kMidInBlock.size,
				  d);
}

uint32_t OnlineBP::Get(uint64_t i)
{
  return BP_[i];
}


uint64_t OnlineBP::GetBits(uint64_t i,
			   uint32_t d)
{
  return BP_.GetBits(i,
		     d);
}

void OnlineBP::Save(ofstream &ofs){
  ofs.write((char*)&n_, sizeof(n_));
  BP_.Save(ofs, n_);
}
  
void OnlineBP::Load(ifstream &ifs){
  uint64_t len  = 0;
  ifs.read((char*)&len, sizeof(len));
  uint64_t BP_len = len/64;
  if((len & (kMidBlock.size - 1)) > 0){
    BP_len++;
  }
  uint64_t tmp_bits;
  Init();
  for(size_t i = 0; i < BP_len ; i++){
    ifs.read((char*)&tmp_bits, sizeof(tmp_bits));
    for(size_t j = 0; (j < 64) && (n_ < len); j++){
      const uint64_t shift = kMidBlock.size - j  - 1;
      if((tmp_bits >> shift) & 1){
	Push(1);
      }
      else{
	Push(0);
      }
    }
  }
}
 
#ifdef ONLINEBP_MAIN
void OnlineBP::Dump()
{
  for (uint64_t j = 0; j < n_; j++) {
    fprintf(stderr, "%1d", GetBit(BP_, j));
  }
  fprintf(stderr, "\n");
  unsigned char c2 = 'a'-1;
  for (uint64_t j = 0; j < n_; j++) {
    if (GetBit(BP_, j)) {
      ++c2;
      if (c2 == 'z'+1) {
        c2 = 'a';
      }
    } else {
      --c2;
      if (c2 == 'a'-1) {
        c2 = 'z';
      }
    }
    fprintf(stderr, "%c", c2);
  }
  fprintf(stderr, "\n");
}


int main(int argc, char *argv[])
{
  clock_t start, end;
  onlinebp bp; bp.Init();
  int c;
  int NUM = 1000000;
  unsigned long long int checksum = 0;

  if (argc >= 2) {
    srand(argv[1][0]);
  }
  
  start = clock();
  bp.Push(kOP);
  for (uint64_t i = 1; i < NUM; ++i) {
    if (bp.depth(n_-1) == 1) {
      c = kOP;
    } else if (bp.Depth(n_-1) >= 64) {
      c = kCP;
    } else {
      c = rand() % 2;
    }
    bp.Push(c);
    ////////////////////////////////////////////////// debug
    // if (c == kCP) {
    //   const uint64_t ori_pos = n_ - 1;
    //   const uint64_t lmleaf_pos = onlinebp_leftmost_leaf(bp, ori_pos);
    //   const uint64_t rmleaf_pos = onlinebp_rightmost_leaf(bp, ori_pos);
    //   uint64_t pos = n_ - 1;
    //   printf("L:%llu", pos);
    //   while (c == kCP) {
    //     printf("-L>");
    //     pos = onlinebp_leftchild(bp, pos);
    //     checksum += pos;
    //     printf("%llu", pos);
    //     c = onlinebp_get(bp, pos);
    //   }
    //   if (pos != lmleaf_pos) {
    //     fprintf(stderr, "\nFAIL LEFTMOST:pos%llu,lmleafpos%llu\n", pos, lmleaf_pos);
    //     dump(bp);
    //   }
    //   printf("\nU:%llu", pos);
    //   while (true) { // go up
    //     uint64_t temp_pos = onlinebp_parent(bp, pos);
    //     if (temp_pos == NOT_FOUND) break;
    //     pos = temp_pos;
    //     checksum += pos;
    //     printf("-P>%llu", pos);
    //   };
    //   printf("\n");
    //   if (ori_pos != pos) {
    //     fprintf(stderr, "\nFAIL BACK FROM LEFTMOST: ori_pos%llu, pos%llu\n", ori_pos, pos);
    //     dump(bp);
    //   }

    //   pos = ori_pos;
    //   c = kCP;
    //   printf("R:%llu", pos);
    //   while (c == kCP) {
    //     printf("-R>");
    //     pos = onlinebp_rightchild(bp, pos);
    //     checksum += pos;
    //     printf("%llu", pos);
    //     c = onlinebp_get(bp, pos);
    //   }
    //   if (pos != rmleaf_pos) {
    //     fprintf(stderr, "\nFAIL RIGHTMOST:pos%llu,rmleafpos%llu\n", pos, rmleaf_pos);
    //     dump(bp);
    //   }
    //   printf("\nU:%llu", pos);
    //   while (true) { // go up
    //     uint64_t temp_pos = onlinebp_parent(bp, pos);
    //     if (temp_pos == NOT_FOUND) break;
    //     pos = temp_pos;
    //     checksum += pos;
    //     printf("-P>%llu", pos);
    //   };
    //   printf("\n");
    //   if (ori_pos != pos) {
    //     fprintf(stderr, "\nFAIL BACK FROM RIGHTMOST: ori_pos%llu, pos%llu\n", ori_pos, pos);
    //     dump(bp);
    //   }
    // }
    // if (i % 100000 == 0) {
    //   fprintf(stderr, "n = %ld size = %ld bytes (%f bpc)\n", n_, onlinebp_size(bp), (double)onlinebp_size(bp)*8/n_);
    // }
    //////////////////////////////////////////////////
  }
  end = clock();
  fprintf(stderr, "n = %ld size = %ld bytes (%f bpc) (checksum %llu)\n", n_, Size(bp), (double)Size(bp)*8/n_, checksum);
  fprintf(stderr, "%f\n", (double)(end - start)/CLOCKS_PER_SEC);

  // performance test for each query
  uint64_t query;
  const uint64_t QNUM = NUM;
  const uint64_t JUMP = 38201; // a prime number
  const unsigned int NUM0 = bp.Rank(n_ - 1, kCP);
  const unsigned int NUM1 = bp.Rank(n_ - 1, kOP);
  double sec;

  // performance test for depth query
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.Depth(query);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu depth queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for rank/select on BP_
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.Rank(query, kOP);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu rank (OP) queries (checksum %llu)\n", sec, QNUM, checksum);

  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.Rank(bp, query, kCP);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu rank (CP) queries (checksum %llu)\n", sec, QNUM, checksum);

  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM1;
    query++;
    checksum += bp.Select(query, kOP);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu select (OP) queries (checksum %llu)\n", sec, QNUM, checksum);

  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM0;
    query++;
    checksum += bp.Select(query, kCP);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu select (CP) queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for parent
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.Parent(query);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu parent queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for left child
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    if (bp.Get(query) == kCP) {
      checksum += LeftChild(query);
    }
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu leftchild queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for leftmost leaf
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.LeftMostLeaf(query);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu leftmost leaf queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for right child
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    if (bp.Get(query) == kCP) {
      checksum += bp.RightChild(query);
    }
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu rightchild queries (checksum %llu)\n", sec, QNUM, checksum);

  // performance test for rightmost leaf
  checksum = 0;
  start = clock();
  query = 1;
  for (int i = 0; i < QNUM; ++i) {
    query += JUMP;
    query %= NUM;
    checksum += bp.RightMostLeaf(query);
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for %llu rightmost leaf queries (checksum %llu)\n", sec, QNUM, checksum);

  // debug outer rank
  checksum = 0;
  start = clock();
  for (uint64_t i = 0; i < NUM-2; ++i) {
    uint64_t ans1 = bp.FwdCntOuter(BP_, i);
    uint64_t ans0 = bp.OuterRank(i);
    if (ans0 != ans1) {
      fprintf(stderr, "query%llu: ans%llu, naive_ans%llu\n", i, ans0, ans1);
    }
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for outer rank test\n", sec);

  // debug outer select
  checksum = 0;
  start = clock();
  for (uint64_t i = 1; i <= curOuterRank_; ++i) {
    uint64_t ans1 = bp.FwdSelOuter(BP_, i);
    uint64_t ans0 = bp.OuterSelect(i);
    if (ans0 != ans1) {
      fprintf(stderr, "query%llu: ans%llu, naive_ans%llu\n", i, ans0, ans1);
    }
  }
  end = clock();
  sec = (double)(end - start)/CLOCKS_PER_SEC;
  fprintf(stderr, "%f sec for outer select test (curOuterRank = %llu)\n", sec, curOuterRank_);

  return 0;
}
#endif
