CXX = g++
CXXFLAGS = -std=c++11 -Ofast -W -Wall -Wno-deprecated -march=native -mtune=native -DNDEBUG
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
	poslp.cpp\
	decomp_suc_poslp.cpp\
	decompress_main.cpp\

OCOMMON = $(COMMON:%.cpp=%.o)
OCOMP = $(COMP:%.cpp=%.o)
ODECOMP = $(DECOMP:%.cpp=%.o)
DCOMMON = $(COMMON:%.cpp=%.d)
DCOMP = $(COMP:%.cpp=%.d)
DDECOMP = $(DECOMP:%.cpp=%.d)
DEPS := $(DCOMMON) $(DCOMP) $(DDECOMP)

all: compress decompress

-include $(DEPS)

compress:  $(OCOMMON) $(OCOMP)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OCOMMON) $(OCOMP) $(LINKFLAGS) -o compress

decompress: $(OCOMMON) $(ODECOMP)
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) $(OCOMMON) $(ODECOMP) $(LINKFLAGS) -o decompress

debug:
	make all CXXFLAGS="-ggdb -W -Wall -pedantic"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OTHERFLAGS) -c -MMD $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) $(OHTERFLAGS) -c -MMD $< -o $@

clean:
	rm -f compress decompress $(DEPS) *.o *~


ifeq ($(or ${IS_DRYRUN},${ONLY_CLEAN}),)
  -include ${DEPS}
endif

.PHONY: all clean
