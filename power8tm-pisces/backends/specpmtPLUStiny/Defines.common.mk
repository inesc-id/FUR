# ==============================================================================
#
# Defines.common.mk
#
# ==============================================================================


CC       := gcc
CFLAGS   += -pthread
CFLAGS   += -I$(LIB)
CPP      := g++
CPPFLAGS += $(CFLAGS)
LD       := g++
LIBS     += -lpthread

# Remove these files when doing clean
OUTPUT +=

LIB := ../lib

STM := ../../../stms
SPEC := ../../../stms/specpmt
TINY := ../../../stms/tinystm



# ==============================================================================
#
# End of Defines.common.mk
#
# ==============================================================================
