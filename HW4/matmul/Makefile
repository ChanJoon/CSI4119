USER_ID := $(shell whoami)
CXX = g++ 
CXXFLAGS = -g -std=c++11 -Wall -Wno-sign-compare -O3

NVCXX = nvcc 
#NVCXXFLAGS = -g --ptxas-options=-v -std=c++11  -O3
NVCXXFLAGS = -g -std=c++11 -arch=sm_86 -O3


CUDALIB = /usr/local/cuda/lib64
CUDAINC = /usr/local/cuda/include
SRCDIR = src
OBJDIR = obj
CUOBJDIR = cuobj
BINDIR = bin

INCS := $(wildcard $(SRCDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(wildcard $(OBJDIR)/*.o)
CUSRCS := $(wildcard $(SRCDIR)/*.cu)
CUOBJS := $(CUSRCS:$(SRCDIR)/%.cu=$(CUOBJDIR)/%.o)

all:  bin/matmul

bin:
	mkdir -p bin


bin/matmul: $(OBJS) $(CUOBJS) 
	mkdir -p bin
	@echo "OBJ: "$(OBJS)
	@echo "CUOBJ: "$(CUOBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS) -L$(CUDALIB) -lcudart -Iinclude -I$(CUDAINC) 
			    @echo "Compiled "$<" successfully!"
256:
	timeout 60 ./bin/matmul /HW4_data/input_256.dat /HW4_data/output_256.dat

512:
	timeout 60 ./bin/matmul /HW4_data/input_512.dat /HW4_data/output_512.dat

1024:
	timeout 60 ./bin/matmul /HW4_data/input_1024.dat /HW4_data/output_1024.dat

2048:
	timeout 60 ./bin/matmul /HW4_data/input_2048.dat /HW4_data/output_2048.dat

4096:
	timeout 60 ./bin/matmul /HW4_data/input_4096.dat /HW4_data/output_4096.dat

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
# Submit
##########################
remote:
	scp src/matmul_kernel.cu mgp-run:/home/${USER_ID}/HW4/matmul/src
	ssh mgp-run "cd HW4/matmul && make | tee result/eval_output.txt"
	
