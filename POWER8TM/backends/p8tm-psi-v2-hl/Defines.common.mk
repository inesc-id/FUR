CC       := gcc
CFLAGS   += -std=c++11 -gdwarf-2 -g3 -w -pthread -fpermissive -mcpu=power8 -mtune=power8 -I$(LIB) -O2
CPP      := g++
CPPFLAGS += -std=c++11 -gdwarf-2 -g3 -w -pthread -fpermissive -mcpu=power8 -mtune=power8 -I$(LIB) -O2
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib
