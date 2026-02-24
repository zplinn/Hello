
challenge: challenge.c code.c exceptions4c/lib/libexceptions4c.a
	gcc -Wall -Wpedantic -s -O0 -Iexceptions4c/src -o $@ $^

code.c: assembler.py
	python3 assembler.py 'LiveCTF{once-m0re-w1th-f33ling}' 1337 code.c

exceptions4c/src/exceptions4c.c:
	git clone https://github.com/guillermocalvo/exceptions4c.git

exceptions4c/lib/libexceptions4c.a: exceptions4c/src/exceptions4c.c
	cd exceptions4c; autoreconf --install; ./configure; make all

clean:
	rm -rf challenge

.PHONY: clean
