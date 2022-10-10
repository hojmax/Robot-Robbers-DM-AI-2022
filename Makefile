helper: helper.c
	gcc -shared -o libhelper.so -fPIC helper.c