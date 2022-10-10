helper: helper.c
	gcc-11 -fPIC -shared -o libhelper.so helper.c -lm -fopenmp