all: a.out

a.out: machine.cc mycode.asm
	g++ -g3 -O0 machine.cc


.PHONY: clean
	
clean:
	rm a.out