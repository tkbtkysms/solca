/* poslp.hpp
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

#ifndef POSLP_HPP_
#define POSLP_HPP_

#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <cassert>
#include "constant_numbers.hpp"

namespace solca_comp
{
    class POSLP
    {
    private:
        std::vector<uint32_t> left_;
        std::vector<uint32_t> right_;

    public:
        //constructor and destructor
        POSLP() : left_(),
                  right_(){};
        ~POSLP(){};

        uint64_t Decompress(const std::string &kInputFileName,
                            const std::string &kOutputFileName);

    private:
        uint64_t Left(const uint64_t kVar);
        uint64_t Right(const uint64_t kVar);
        void Load(std::ifstream &ifs);

    }; // class POLSP

} //namespace solca_comp

#endif // POSLP_HPP_
