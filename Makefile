helper: helper.c
	gcc -fPIC -shared -o helper.so helper.c -lm