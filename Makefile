build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out

clean:
	rm -f *.out *.perf processes.txt

all: clean build

run:
	./process_generator.out
