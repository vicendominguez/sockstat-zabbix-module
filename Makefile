zbx_sockstat: zbx_sockstat.c
	gcc -shared -o zbx_sockstat.so zbx_sockstat.c -I../../../include -fPIC
