
OBFUS:=obfus.h/include
TCC_WIN:=tcc/tcc.exe

default: test

check.c: generate.py
	python3 $< "LiveCTF{obfus.h_is_pretty_cool}" 1337 $@

challenge.exe: obfuscate.h challenge.c check.c $(OBFUS)
	wine $(TCC_WIN) -o $@ challenge.c -I$(OBFUS)

tcc-0.9.27-win32-bin.zip:
	wget https://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win32-bin.zip

$(TCC_WIN): tcc-0.9.27-win32-bin.zip
	unzip $<

$(OBFUS):
	git clone https://github.com/DosX-dev/obfus.h

test: challenge.exe
	wine ./challenge.exe

clean:
	rm -rf challenge.exe

.PHONY: default test clean
