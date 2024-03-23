build:
	mkdir -p output
	g++ -g run.cpp cthreadpool.cpp condition.cpp -o output/run -std=c++11 -lpthread

run:
	./output/run

clean:
	rm -rf ./output