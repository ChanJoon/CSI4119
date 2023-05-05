USER_ID := $(shell whoami)
GPP=g++ -std=c++11 -pthread -lpthread -fopenmp -O3 -Wl,--no-as-needed -march=znver2

.PHONY: 

run: matmul
	./matmul /HW3_data/input_2048_1.dat /HW3_data/output_2048_1.dat | tee result/eval_output.txt

run2: matmul
	./matmul /HW3_data/input_2048_2.dat /HW3_data/output_2048_2.dat | tee result/eval_output.txt

run_256: matmul
	./matmul /HW3_data/input_256.dat /HW3_data/output_256.dat | tee result/eval_output.txt

run_512: matmul
	./matmul /HW3_data/input_512.dat /HW3_data/output_512.dat | tee result/eval_output.txt

run_1024: matmul
	./matmul /HW3_data/input_1024.dat /HW3_data/output_1024.dat | tee result/eval_output.txt

matmul: matmul_o
	$(GPP) -o $@ build/matmul.o build/driver.o

matmul_o:
	$(GPP) -c src/matmul.cpp -o build/matmul.o

remote:
	scp src/matmul.cpp mgp-run:/home/${USER_ID}/HW3/matmul/src
	ssh mgp-run "cd HW3/matmul && make" | tee result/eval_output.txt

#########################
# Environment variable
#########################
build=build
src=src

#########################
# Run
#########################

# You don't need to run this
# $(build)/driver.o is provided
driver_o: matmul_o
	$(GPP) -c $(src)/driver.cpp -o $(build)/driver.o


#########################
# Utils
#########################
clean:
	rm -f $(build)/matmul.o matmul

format:
	clang-format -i -style=Google $(src)/*.cpp $(src)/*.h

