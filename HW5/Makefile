USER_ID := $(shell whoami)
CXX = g++ 
CXXFLAGS = -g -std=c++11 -Wall -Wno-sign-compare -O3

NVCXX = nvcc 
NVCXXFLAGS = -g -std=c++11 -O3 -arch=sm_86#--ptxas-options=-v 


CUDALIB = /usr/local/cuda/lib64
CUDAINC = /usr/local/cuda/include
SRCDIR = src
OBJDIR = obj
CUOBJDIR = cuobj
BINDIR = bin

INCS := $(wildcard $(SRCDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cc)
OBJS := $(wildcard $(OBJDIR)/*.o)
CUSRCS := $(wildcard $(SRCDIR)/*.cu)
CUOBJS := $(CUSRCS:$(SRCDIR)/%.cu=$(CUOBJDIR)/%.o)

all:  bin/reduce

bin:
	mkdir -p bin


bin/reduce: $(OBJS) $(CUOBJS) 
	mkdir -p bin
	@echo "OBJ: "$(OBJS)
	@echo "CUOBJ: "$(CUOBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS) -L$(CUDALIB) -lcudart -Iinclude -I$(CUDAINC) 
			    @echo "Compiled "$<" successfully!"


.PHONY:	test clean

$(CUOBJS): $(CUOBJDIR)/%.o : $(SRCDIR)/%.cu
		mkdir -p cuobj
	    @echo $(NVCXX) $(NVCXXFLAGS) "-Iinclude -c" $< "-o" $@
	    @$(NVCXX) $(NVCXXFLAGS) -Iinclude -c $< -o $@
			    @echo "CUDA Compiled "$<" successfully!"

clean: 
	rm -f $(CUOBJS) $(CUOBJS:%.o=%.d) 
	rm -rf bin/*

#########################
# Local Run
##########################

1:
	timeout 60 ./bin/reduce /HW5_data/input_64.dat /HW5_data/output_64.dat

2:
	timeout 60 ./bin/reduce /HW5_data/input_4194304.dat /HW5_data/output_4194304.dat

3:
	timeout 60 ./bin/reduce /HW5_data/input_8388608.dat /HW5_data/output_8388608.dat

4:
	timeout 60 ./bin/reduce /HW5_data/input_16777216.dat /HW5_data/output_16777216.dat

#########################
# Remote Execution
##########################

remote:
	scp src/reduction_kernel.cu mgp-run:/home/${USER_ID}/HW5/src
	ssh mgp-run "cd HW5 && make"

remote1:
	scp src/reduction_kernel.cu mgp-run:/home/${USER_ID}/HW5/src
	ssh mgp-run "cd HW5 && make 1"

remote2:
	scp src/reduction_kernel.cu mgp-run:/home/${USER_ID}/HW5/src
	ssh mgp-run "cd HW5 && make 2"

remote3:
	scp src/reduction_kernel.cu mgp-run:/home/${USER_ID}/HW5/src
	ssh mgp-run "cd HW5 && make 3"

remote4:
	scp src/reduction_kernel.cu mgp-run:/home/${USER_ID}/HW5/src
	ssh mgp-run "cd HW5 && make 4"
