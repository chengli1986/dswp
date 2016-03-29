CFLAGS += -MMD

# -rdynamic
# Pass the flag -export-dynamic to the ELF linker, 
# on targets that support it. This instructs the 
# linker to add all symbols, not only used ones, 
# to the dynamic symbol table. This option is needed 
# for some uses of dlopen or to allow obtaining 
# backtraces from within a program. 

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -g -O0
# DEBUG
#CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -v -g -O0

VALGRIND_ARGS = 

.PHONY: all runtime-tests gdb/% valgrind/% time/% \
		tidy clean clean-examples

all: clean DSWP.so runtime/libruntime.a

PASS_OBJS = DSWP_0.o DSWP_1.o DSWP_2.o DSWP_3.o DSWP_4.o DSWP_5.o DSWP_DEBUG.o \
	        Utils.o raw_os_ostream.o
RUNTIME_OBJS = runtime/queue.o runtime/simple_sync.o runtime/runtime_debug.o
RT_TEST_OBJS = runtime/tests/sync_test.o runtime/tests/test.o
-include $(PASS_OBJS:%.o=%.d) $(RUNTIME_OBJS:%.o=%.d) $(RT_TEST_OBJS:%.o=%.d)

################# 
# the main pass
################# 

# $(CXX) = g++
# -dylib is for MacOS...
# -flat_namespace is also for MacOS...
# $(PASS_OBJS) = list of linking objects
DSWP.so: $(PASS_OBJS)
	$(CXX) -shared -g -O0  $^ -o $@
	
# add -v for DEBUG
#$(CXX) -shared -g -O0  $^ -o $@
#$(CXX) -dylib -flat_namespace -shared -g -O0  $^ -o $@

# We're including raw_os_ostream.o because we can't just link in libLLVMSupport:
# http://lists.cs.uiuc.edu/pipermail/llvmdev/2010-June/032508.html

#######################
# the runtime library
#######################

# manipulate the static library/archive
# include all runtime objects
# r - insert file members to the archive
# c - create the archive
# s - write  an object-file index into the archive, or update an existing one
runtime/libruntime.a: $(RUNTIME_OBJS)
	ar rcs $@ $^

# should really be using target-specific rules, but couldn't get them to work
# $(COMPILE.c) = cc in ubuntu
# $(LINK.o) = cc in ubuntu
runtime/tests/%.o: runtime/tests/%.c
	$(COMPILE.c) -pthread -Iruntime/ $< -o $@
runtime/%.o: runtime/%.c
	$(COMPILE.c) -pthread $< -o $@
runtime/tests/%: runtime/tests/%.o runtime/libruntime.a
	$(LINK.o) -pthread $^ -o $@
runtime-tests: runtime/tests/test runtime/tests/sync_test
	runtime/tests/test
	runtime/tests/sync_test

##################################
# compiling examples to bitcode
##################################
Example/%.bc: Example/%.c
	clang -O0 -c -emit-llvm $< -o $@
Example/%.bc: Example/%.cpp
	clang++ -O0 -c -emit-llvm $< -o $@

######################
# optimizing bitcode
######################
Example/%-m2r.bc: Example/%.bc
	opt -mem2reg $< -o $@
Example/%-o2.bc: Example/%.bc
	opt -O2 $< -o $@
Example/%-o3.bc: Example/%.bc
	opt -O3 $< -o $@
Example/%-DSWP.bc: Example/%.bc DSWP.so
	opt -load ./DSWP.so -dswp $< -o $@
gdb/%: Example/%.bc DSWP.so
	gdb --args opt -load ./DSWP.so -dswp $< -o /dev/null
valgrind/%: Example/%.bc DSWP.so
	valgrind $(VALGRIND_ARGS) opt -load ./DSWP.so -dswp $< -o /dev/null

###########################
# (dis)assembling bitcode
###########################
Example/%.bc.ll: Example/%.bc
	llvm-dis $< -o $@


# Utility: print to PS and PDF
Example/%.bc.ll.ps: Example/%.bc.ll
	enscript $< -q -E -fCourier10 --tabsize=4 -p $@
Example/%.bc.ll.pdf: Example/%.bc.ll.ps
	ps2pdf $< $@
Example/%.o: Example/%.bc
	clang -O0 -c $< -o $@

# -lm is not necessary???
# http://stackoverflow.com/questions/10447791/what-does-lm-option-do-in-g
# hmm if I run problem44.c then -lm is necessary
Example/%.out: Example/%.bc runtime/libruntime.a
	clang -O0 -pthread $< runtime/libruntime.a -lm -o $@
run/%: Example/%.out
	$<
time/%: Example/%.out
	time $<

###############
# cleaning up
###############
tidy:
	rm -f *.o runtime/*.o runtime/tests/*.o *.d runtime/*.d runtime/tests/*.d
tidy-output:
	rm -f dag partition showgraph
clean-examples: tidy-output
	rm -f Example/*.bc Example/*.bc.ll Example/*.o Example/*.out
clean-objs: tidy
	rm -f DSWP.so runtime/libruntime.a runtime/tests/test runtime/tests/sync_test
clean: clean-objs clean-examples
