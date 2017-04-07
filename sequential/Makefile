all:
	gcc -fopenmp -std=c99 -Wall -g life3d.c -o life3d
debug:
	gcc -fopenmp -std=c99 -Wall -DDEBUG -g life3d.c -o life3d
iteration:
	gcc -fopenmp -std=c99 -Wall -DITERATION -g life3d.c -o life3d
opt:
	gcc -fopenmp -std=c99 -Wall life3d.c -o life3d
optiter:
	gcc -fopenmp -std=c99 -Wall -DITERATION -o life3d life3d.c
optdebug:
	gcc -fopenmp -std=c99 -Wall -DDEGUG -o life3d life3d.c
opttimer:
	gcc -Wall -std=c99 -DTIMER -fopenmp -o life3d life3d.c

