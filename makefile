all:
	clang++ -g -fsanitize=address --std=c++1z main.cpp -o run -lm
clean:
	rm run
