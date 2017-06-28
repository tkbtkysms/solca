CXX = g++
CXXFLAGS = -std=c++11 -O3 -W -Wall -Wno-deprecated
LINKFLAGS = -lm

SRCS =\
	EnumCoder.cpp\
	RSDic.cpp\
	bit_vector.cpp\
	eflc_vector.cpp\
	flc_vector.cpp\
	freq_crdict.cpp\
	inner_leaf.cpp\
	nrs_dic.cpp\
	online_bp.cpp\
	out_leaf.cpp\
	permutation.cpp\
	solca.cpp\
	suc_fbtree.cpp\
	succinct_poslp.cpp\

SRCS1 = compress_main.cpp\

SRCS2 = decompress_main.cpp\

OBJS = $(SRCS:%.cpp=%.o)
OBJS1 = $(SRCS1:%.cpp=%.o)	
OBJS2 = $(SRCS2:%.cpp=%.o)

all: compress decompress

compress:  $(OBJS) $(OBJS1) 
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS1) $(LINKFLAGS) -o compress

decompress: $(OBJS) $(OBJS2)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OBJS) $(OBJS2) $(LINKFLAGS) -o decompress

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c $<

%.o: %.c
	$(CXX) $(CXXFLAGS) $(OHTERFLAGS) -c $<

clean:
	rm -f compress decompress fcrdic *.o *~


