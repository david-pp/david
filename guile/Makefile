
all : libguile-math.so simple-guile

simple-guile : simple-guile.c
	gcc -o $@ $< -lguile

libguile-math.so : libguile-math.c
	gcc -shared -o $@ -fPIC $< -lguile

clean :
	@rm *.exe
	@rm *.so