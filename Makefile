helper: helper.c
	gcc -fPIC -shared -o libhelper.so helper.c -lm