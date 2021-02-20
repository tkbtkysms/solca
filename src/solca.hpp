/* solca.hpp
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

#ifndef SOLCA_HPP_
#define SOLCA_HPP_

#include <vector>
#include <deque>
#include <string>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <ctime>
#include <sys/time.h>
#include "constant_numbers.hpp"
#include "succinct_poslp.hpp"

namespace solca_comp
{
  class SOLCA
  {
  private:
    std::vector<std::deque<Node>> ques_;
    std::vector<uint64_t> que_len_;
    uint64_t max_level_;
    SucPOSLP sposlp_;
    const uint64_t kMaxQueLen = 6U;
    const uint64_t kMaxNumQue = 256U;

  public:
    SOLCA() : ques_(),
              sposlp_() {}
    ~SOLCA() {}
    void Init(const std::string &kOutputFileName);

    uint64_t Compress(const std::string &kInputFileName,
                      const std::string &kOutputFileName,
                      const bool kEraseBr,
                      const bool kPrintLogs,
                      const bool kNaiveEncoding);
    uint64_t Decompress(const std::string &kInputFileName,
                        const std::string &kOutputFileName);

  private:
    inline bool IsRepetition(const uint64_t kLevel,
                             const uint64_t kPos) const;
    inline bool IsMaximal(const uint64_t kLevel,
                          const uint64_t kPos) const;
    bool HasLandmark(const uint64_t kLevel) const;
    inline void PopFrontQue2(const uint64_t kLevel);
    inline void PopFrontQue3(const uint64_t kLevel);
    void BuildPOSLP(const Node &kNode,
                    const uint64_t kLevel);
    void RepeatDecompress(const uint64_t kVar,
                          std::ofstream &ofs);
    inline uint64_t AlphabetReduction(const uint64_t kFirst,
                                      const uint64_t kSecond) const;
    void ProcessLastSymbols(const bool kNavieEncoding);
    //space infromations
    uint64_t BSpace() const;
    uint64_t L1Space() const;
    uint64_t L2L3Space() const;
    uint64_t LSpace() const;
    uint64_t HashSpace() const;
    uint64_t OtherSpace() const;
    uint64_t Space() const;
    uint64_t NumRules() const;
    uint64_t DictNumRules() const;
    uint64_t WriteSize() const;
    void PrintColumns() const;
    void PrintLogs(const uint64_t kCnt,
                   const double kTime) const;
    double GetTimeOfDaySec() const;
  }; // class SOLCA

  //inline implementations
  inline bool SOLCA::IsRepetition(const uint64_t kLevel,
                                  const uint64_t kPos) const
  {
    if (ques_[kLevel][kPos].symbol == ques_[kLevel][kPos + 1].symbol)
    {
      return true;
    }
    return false;
  }

  inline uint64_t SOLCA::AlphabetReduction(const uint64_t kFirst,
                                           const uint64_t kSecond) const
  {

    uint64_t diff_bit_pos = CFunc::LSB(kFirst ^ kSecond);

    if ((kSecond >> diff_bit_pos) & kOne)
    {
      return (diff_bit_pos << 1) + 1;
    }
    else
    {
      return (diff_bit_pos << 1);
    }
  }

  inline bool SOLCA::IsMaximal(const uint64_t kLevel,
                               const uint64_t kPos) const
  {
    uint64_t symbol1 = ques_[kLevel][kPos - 1].symbol;
    uint64_t symbol2 = ques_[kLevel][kPos].symbol;

    uint64_t middle = AlphabetReduction(symbol1, symbol2);
    if (middle > AlphabetReduction(ques_[kLevel][kPos - 2].symbol, symbol1) && middle > AlphabetReduction(symbol2, ques_[kLevel][kPos + 1].symbol))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  inline void SOLCA::PopFrontQue2(const uint64_t kLevel)
  {
    ques_[kLevel].pop_front();
    ques_[kLevel].pop_front();
    que_len_[kLevel] -= 2;
  }

  inline void SOLCA::PopFrontQue3(const uint64_t kLevel)
  {
    ques_[kLevel].pop_front();
    ques_[kLevel].pop_front();
    ques_[kLevel].pop_front();
    que_len_[kLevel] -= 3;
  }
} // namespace solca_comp
#endif // SOLCA_HPP_
