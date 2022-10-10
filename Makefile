helper: helper.c
	gcc -shared -o libhelper.so -fPIC helper.c


helper2: helper.c
	gcc -c -Wall -Werror -fpic helper.c

helper3: helper.c
	gcc -shared -o libhelper.so helper.o