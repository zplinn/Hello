

challenge: challenge_clean pack.py keygen
	python3 pack.py "pr1m3" challenge_clean challenge

challenge_clean: challenge.c stage2.c sha2-obf.c rc4.c
	gcc -O2 -s -o $@ $^

stage2.c: generate.py
	python3 generate.py "LiveCTF{a_pr1m3_t00_far}" $@

keygen: keygen.c sha2-obf.c
	gcc -O2 -s -o $@ $^

clean:
	rm -rf challenge challenge_clean stage2.c

.PHONY: clean
