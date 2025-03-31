CFLAGS = -Oz -std=c17 \
	-Wall -Wextra -Wconversion -Wdouble-promotion \
	-Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion \
	-fsanitize=undefined -fsanitize-trap -pedantic

clean:
	rm -f bruteforce_su
