# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

CC                 = gcc -std=c99
CFLAGS            += -g -Wall -O0
OFLAGS            += -Wall -pedantic -O2
INCS               = -I .
LIBS               = -pthread
SOL1               = phi1.out
SOL2               = phi2.out
SOL3               = phi3.out

.PHONY: all debug optimized \
		test1 test2 test3 \
		memtest1 memtest2 memtest3 \
		benchmark benchmark-opt \
		clean cleanall

all: $(SOL1) $(SOL2) $(SOL3) debug optimized
debug: phi1.c phi2.c phi3.c
	$(CC) $(INCS) $(CFLAGS) -o dbg-phi1.out phi1.c -DDEBUG $(LIBS)
	$(CC) $(INCS) $(CFLAGS) -o dbg-phi2.out phi2.c -DDEBUG $(LIBS)
	$(CC) $(INCS) $(CFLAGS) -o dbg-phi3.out phi3.c -DDEBUG $(LIBS)
optimized: phi1.c phi2.c phi3.c
	$(CC) $(INCS) $(OFLAGS) -o opt-phi1.out phi1.c -DNDEBUG $(LIBS)
	$(CC) $(INCS) $(OFLAGS) -o opt-phi2.out phi2.c -DNDEBUG $(LIBS)
	$(CC) $(INCS) $(OFLAGS) -o opt-phi3.out phi3.c -DNDEBUG $(LIBS)
$(SOL1): phi1.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -DNDEBUG
$(SOL2): phi2.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -DNDEBUG
$(SOL3): phi3.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) -DNDEBUG
test1: $(SOL1)
	/bin/sh -e ./testing.sh 1
test2: $(SOL2)
	/bin/sh -e ./testing.sh 2
test3: $(SOL2)
	/bin/sh -e ./testing.sh 3
benchmark: $(SOL1) $(SOL2) $(SOL3)
	/bin/sh -e ./benchmark.sh $(SOL1) $(SOL2) $(SOL3)
benchmark-opt: optimized
	/bin/sh -e ./benchmark.sh opt-phi1.out opt-phi2.out opt-phi3.out
memtest1: $(SOL1)
	/bin/sh -e ./memtest.sh 1;
memtest2: $(SOL2)
	/bin/sh -e ./memtest.sh 2;
memtest3: $(SOL3)
	/bin/sh -e ./memtest.sh 3;
clean: 
	-rm -fr *.o *~
cleanall: clean
	-rm -fr *.out
