/*decompress_main.cpp
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

#include <string>
#include "decomp_suc_poslp.hpp"
#include "poslp.hpp"
#include "cmdline.h"

using namespace std;

int main(int argc, char *argv[])
{
  cmdline::parser p;
  p.add<string>("compressed_file", 'i', "compressed file name", true);
  p.add<string>("output_file", 'o', "output file name", true);
  p.add<bool>("naive_encoding", 'n', "decompress from the naive encoding of solca's grammar", false, false);

  p.parse_check(argc, argv);
  const string input_file = p.get<string>("compressed_file");
  const string output_file = p.get<string>("output_file");
  const bool naive_encoding = p.get<bool>("naive_encoding");

  if (naive_encoding)
  {
    solca_comp::POSLP poslp;
    return poslp.Decompress(input_file,
                            output_file);
  }
  else
  {
    solca_comp::DSucPOSLP dsuc_poslp;
    return dsuc_poslp.Decompress(input_file,
                                 output_file);
  }
}
