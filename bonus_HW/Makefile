#########################
# Variables
#########################
NVCC=/usr/local/cuda/bin/nvcc
BUILD=build
SRC=src
FLAGS=-std=c++11 -O3 -arch=sm_86

#########################
# Main
#########################
all: predict
	mkdir -p build
	./predict /vgg_data/cifar10/test_batch.bin 0 128 /vgg_data/vgg_weight/values_vgg.txt

$(BUILD):
	mkdir -p build

predict: $(BUILD)/main.o $(BUILD)/util.o $(BUILD)/vgg16_cpu.o $(BUILD)/vgg16_cuda.o $(BUILD)/vgg16.o 
	$(NVCC) $(FLAGS) -o $@ $^

$(BUILD)/main.o: $(SRC)/main.cpp $(BUILD)/util.o $(BUILD)/vgg16.o $(BUILD)/vgg16_cpu.o $(BUILD)/vgg16_cuda.o
	$(NVCC) $(FLAGS) -o $@ -c $< 

$(BUILD)/util.o: $(SRC)/util.cpp
	$(NVCC) $(FLAGS) -o $@ -c $< 

$(BUILD)/vgg16_cpu.o: $(SRC)/vgg16_cpu.cpp $(BUILD)/vgg16.o
	$(NVCC) $(FLAGS) -o $@ -c $<

$(BUILD)/vgg16_cuda.o: $(SRC)/vgg16_cuda.cu $(BUILD)/vgg16.o
	$(NVCC) $(FLAGS) -o $@ -c $<

$(BUILD)/vgg16.o: $(SRC)/vgg16.cpp
	$(NVCC) $(FLAGS) -o $@ -c $<

#########################
# Util
#########################
format:
	clang-format -i -style=Google $(SRC)/*.cu $(SRC)/*.cpp

clean:
	rm -rf predict $(BUILD)/*.o
