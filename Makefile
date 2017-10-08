#
# A simple makefile for building project composed of C source files.
#
# Julie Zelenski, for CS107, Sept 2014.
#
# Modified for use in CLexicon personal project. Dan McFalls, December 2015.
#

# It is likely that default C compiler is already gcc, but be explicit anyway
CC = gcc

# The CFLAGS variable sets the flags for the compiler.
#  -g          compile with debug information
#  -Ofast      optimize for speed
#  -std=gnu99  use the C99 standard language definition with GNU extensions
#  -Wall       turn on optional warnings (warnflags configures specific diagnostic warnings)
CFLAGS = -g -Ofast -std=gnu99 -Wall $$warnflags -fno-omit-frame-pointer -fno-stack-protector
export warnflags = -Wfloat-equal -Wtype-limits -Wpointer-arith -Wlogical-op -Wshadow -fno-diagnostics-show-option

# The LDFLAGS variable sets flags for the linker and the LDLIBS variable lists
# additional libraries being linked. The standard libc is linked by default
LDFLAGS = 
LDLIBS = 

# defines the default build targets
all:: lextest

# lextest is built by compiling lextest and linking with CLexicon.o
lextest: lextest.o CLexicon.o
	$(LINK.o) $^ $(LDLIBS) -o $@

# The entry below is a pattern rule. It defines the general recipe to make
# the 'name.o' object file by compiling the 'name.c' source file. It also
# lists CLexicon.h to be treated as a prerequisite.
%.o: %.c CLexicon.h
	$(COMPILE.c) -I. $< -o $@

# These pattern rules disable implicit rules for executables
# by supplying empty recipe. Accidentally attempting to build
# symbols gives confusing failure from implicit rules, if disabled
# build stops right away and reports "No rule"
%: %.c
%: %.o

# The line below defines the clean target to remove any previous build results
clean::
	rm -f lextest core *.o

# PHONY is used to mark targets that don't represent actual files/build products
.PHONY: clean all soln
