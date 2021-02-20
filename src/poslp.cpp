/* poslp.cpp
MIT License

Copyright (c) 2021 Yoshimasa Takabatake

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

#include "poslp.hpp"

using namespace std;

namespace solca_comp
{

    uint64_t POSLP::Left(const uint64_t kVar)
    {
        assert(kVar >= kAlphabetSize);
        return left_[kVar - kAlphabetSize];
    }

    uint64_t POSLP::Right(const uint64_t kVar)
    {
        assert(kVar >= kAlphabetSize);
        return right_[kVar - kAlphabetSize];
    }

    void POSLP::Load(std::ifstream &ifs)
    {
        ifs.seekg(0, std::ios::end);
        uint64_t eofpos = ifs.tellg();
        ifs.clear();
        ifs.seekg(0, std::ios::beg);
        uint64_t begpos = ifs.tellg();
        uint64_t num_rules = (eofpos - begpos) / (sizeof(uint32_t) * 2);
        left_.resize(num_rules);
        right_.resize(num_rules);
        for (uint64_t i = 0; i < num_rules; ++i)
        {
            ifs.read((char *)&left_[i], sizeof(left_[i]));
            ifs.read((char *)&right_[i], sizeof(right_[i]));
        }
    }

    uint64_t POSLP::Decompress(const string &kInputFileName,
                               const string &kOutputFileName)
    {
        ifstream ifs(kInputFileName.c_str());
        ofstream ofs(kOutputFileName.c_str());
        Load(ifs);
        std::stack<uint64_t> st;
        st.push(left_.size() + kAlphabetSize - 1);
        while (!st.empty())
        {
            uint64_t var = st.top();
            st.pop();
            if (var < kAlphabetSize)
            {
                ofs.write((char *)&var, sizeof(char));
            }
            else
            {
                st.push(Right(var));
                st.push(Left(var));
            }
        }
        return 0;
    }

} // namespace solca_comp
