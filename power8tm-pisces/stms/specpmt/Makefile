ifneq ("$(wildcard $(LOCAL_DIR)/bin/g++)","")
	CXX := $(LOCAL_DIR)/bin/g++
else
	CXX := g++
endif

#CPPFLAGS := -DPTM -O2 --std=c++17 -g -fPIC -march=native -DCLWB
CPPFLAGS := -DPTM -O2 --std=c++17 -g -fPIC -mcpu=native -mtune=native -DCLFLUSH -DNO_WARN_X86_INTRINSICS
#CPPFLAGS := -DPTM -O0 --std=c++17 -g -fPIC

INCLUDE = ./include 

TXLIB = sptx_naive

LIBSPECPMT = sptx_naive.a

AR      := ar
RANLIB  := ranlib


.PHONY: all
all : libsptx_naive.a

# all: $(LIBSPECPMT)

libsptx_naive.a : sptx_naive.cpp specpmt.c
	$(CXX) $(CPPFLAGS) -I $(INCLUDE) $? -shared -o sptx_naive.o
	$(AR) cru $@ sptx_naive.o
	$(RANLIB) $@

# %.a : %.cpp
# 	$(CXX) $(CPPFLAGS) -I $(INCLUDE) $< -shared -o $@.o
# 	$(AR) cru $@ $@.o
# 	$(RANLIB) $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@



%.so : %.cpp
	$(CXX) $(CPPFLAGS) -I $(INCLUDE) $< -shared -o lib$@

clean:
	rm -rf *.so
	rm -rf *.a
	rm -rf *.so.dSYM
