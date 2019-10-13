CC = gcc
CXX = g++
OPT = -Wall -O3 -Wno-unused-function -Wno-unused-variable
PSVNOPT = --state_map --backward_moves --fwd_history_len=0 --bwd_history_len=0

psvn2c_core.c:
	cp ../src/psvn2c_core.c ./psvn2c_core.c

psvn2c_state_map.c:
	cp ../src/psvn2c_state_map.c ./psvn2c_state_map.c

psvn2c_abstraction.c:
	cp ../src/psvn2c_abstraction.c ./psvn2c_abstraction.c

%.c: %.psvn psvn2c_core.c psvn2c_state_map.c psvn2c_abstraction.c
	../bin/psvn2c $(PSVNOPT) --name=$(*F) < $< > $@
	rm -f ./psvn2c_core.c ./psvn2c_state_map.c ./psvn2c_abstraction.c

.PRECIOUS: %.c

%.succ: %.c ../global/succ.c
	$(CC) $(OPT) ../global/succ.c -include $< -o $@
	rm -f $*.c

%.dist: %.c ../global/dist.cpp
	$(CXX) $(OPT) ../global/dist.cpp -include $< -o $@
	rm -f $*.c

%.distSummary: %.c ../global/distSummary.cpp
	$(CXX) $(OPT) ../global/distSummary.cpp -include $< -o $@
	rm -f $*.c

%.generator: %.c ../global/generator.cpp
	@echo "**** Constructing $@ ..."
	@$(CXX) $(OPT) ../global/generator.cpp -include $< -o $@
	@rm -f $*.c

abstractor:
	@echo "**** Constructing abstractor ..."
	@$(CXX) $(OPT) ../src/abstractor.cpp ../src/psvn.cpp -o $@

%.pdb: abstractor
	@rm -f `basename $*`.abst `basename $*`.pdb `basename $*`.psvn
	@echo "**** Running abstractor on `dirname $*`.psvn with `basename $*`.txt ..."
	@./abstractor `dirname $*`.psvn `basename $*` < `basename $*`.txt > /dev/null
	@$(MAKE) `basename $*`.distSummary
	@echo "**** Constructing PDB `basename $*`.pdb ..."
	@./`basename $*`.distSummary `basename $*`.pdb
	@rm -f abstractor `basename $*`.distSummary

%.dist_pdb: %.c ../global/dist_pdb.cpp
	@echo "**** Constructing $@ ..."
	@$(CXX) $(OPT) ../global/dist_pdb.cpp -include $< -o $@
	@rm -f $*.c

%.bfs: %.c ../global/bfs.cpp
	$(CXX) $(OPT) ../global/bfs.cpp -include $< -o $@
	rm -f $*.c

%.dfs: %.c ../global/dfs.cpp
	$(CXX) $(OPT) ../global/dfs.cpp -include $< -o $@
	rm -f $*.c

%.ucs: %.c ../global/ucs.cpp
	$(CXX) $(OPT) ../global/ucs.cpp -include $< -o $@
	rm -f $*.c

%.branch: %.c ../global/branch.cpp
	$(CXX) $(OPT) ../global/branch.cpp -include $< -o $@
	rm -f $*.c

%.iducs: %.c ../global/iducs.cpp
	$(CXX) $(OPT) ../global/iducs.cpp -include $< -o $@
	rm -f $*.c

%.ida: %.c ../global/ida.cpp
	$(CXX) $(OPT) ../global/ida.cpp -include $< -include $*.heuristic.hpp -o $@
	rm -f $*.c

%.ida_pdb: %.c ../global/ida_pdb.cpp
	$(CXX) $(OPT) ../global/ida_pdb.cpp -include $< -o $@
	rm -f $*.c

%.a_star: %.c ../global/a_star.cpp
	$(CXX) $(OPT) ../global/a_star.cpp -include $< -include $*.heuristic.hpp -o $@
	rm -f $*.c

%.a_star_pdb: %.c ../global/a_star_pdb.cpp
	$(CXX) $(OPT) ../global/a_star_pdb.cpp -include $< -o $@
	rm -f $*.c

.PHONY: clean
clean:
	rm -fr *.succ *.dist *.distSummary *.generator *.dist_pdb *.bfs *.dfs *.ucs *.iducs *.ida *.ida_pdb *.a_star *.a_star_pdb *.branch psvn2c_core.c psvn2c_state_map.c psvn2c_abstraction.c *.dSYM *.o *~ __pycache__
