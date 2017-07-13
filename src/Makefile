CXX = g++
CXXFLAGS = -std=c++11 -O3 -W -Wall -Wno-deprecated
LINKFLAGS = -lm

COMMON =\
	bit_vector.cpp\
	flc_vector.cpp\
	inner_leaf.cpp\
	online_bp.cpp\
	suc_fbtree.cpp\

COMP =\
	EnumCoder.cpp\
	RSDic.cpp\
	eflc_vector.cpp\
	nrs_dic.cpp\
	permutation.cpp\
	out_leaf.cpp\
	freq_crdict.cpp\
	succinct_poslp.cpp\
	solca.cpp\
	compress_main.cpp\

DECOMP =\
	decomp_suc_poslp.cpp\
	decompress_main.cpp\

OCOMMON = $(COMMON:%.cpp=%.o)
OCOMP = $(COMP:%.cpp=%.o)	
ODECOMP = $(DECOMP:%.cpp=%.o)

all: compress decompress

compress:  $(OCOMMON) $(OCOMP) 
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OCOMMON) $(OCOMP) $(LINKFLAGS) -o compress

decompress: $(OCOMMON) $(ODECOMP)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OCOMMON) $(ODECOMP) $(LINKFLAGS) -o decompress

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

%.o: %.c
	$(CXX) $(CXXFLAGS) $(OHTERFLAGS) -c $<

clean:
	rm -f compress decompress *.o *~


