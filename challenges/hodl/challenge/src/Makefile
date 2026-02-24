

challenge: challenge.c data.c rc4.c
	gcc -s -Wall -Wpedantic -O2 -o $@ $^

data.c: generate.py
	python3 generate.py 1337 1000000 'LiveCTF{bl4zing_f4st_0h_y3ah}' $@

clean:
	rm -rf challenge data.c

.PHONY: clean
