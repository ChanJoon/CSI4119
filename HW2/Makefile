USER_ID := $(shell whoami)

.PHONY: run HTtest clean remote report format
# HT Test
run: HTtest
	./HTtest 0 | tee result/base.txt
	./HTtest 1 | tee result/better.txt

HTtest: main.cc 
	g++ -std=c++11 -g -fopenmp -pthread  $< -o $@ -lboost_system  -lboost_thread

clean:
	rm -f HTtest result/*.txt

remote: 
	scp better_locked_hash_table.h mgp-run:/home/${USER_ID}/HW2
	scp locked_hash_table.h mgp-run:/home/${USER_ID}/HW2
	scp hash_table.h mgp-run:/home/${USER_ID}/HW2
	ssh mgp-run "cd HW2 && make" | tee result/eval_output.txt

report:
	cat result/base.txt
	cat result/better.txt

# Development
format:
	clang-format -i -style=Google *.cc *.h
