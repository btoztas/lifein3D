all:
	gcc -Wall -std=c99 -g life3d.c -o life3d
debug:
	gcc -Wall -std=c99 -DDEBUG -g life3d.c -o life3d
iteration:
	gcc -Wall -std=c99 -DITERATION -g life3d.c -o life3d
opt:
	gcc -Wall -std=c99 life3d.c -o life3d
optiter:
	gcc -Wall -std=c99 -DITERATION -o life3d life3d.c
optdebug:
	gcc -Wall -std=c99 -DDEGUG -o life3d life3d.c
omp:
	gcc -Wall -std=c99  -fopenmp -o life3d life3d.c
omptime:
	gcc -Wall -std=c99 -DTIME -fopenmp -o life3d life3d.c
gmon:
	gcc -Wall -pg  -std=c99 -DTIME -fopenmp -o life3d life3d.c
ompiter:
	gcc -Wall -std=c99 -DITERATION -fopenmp -o life3d life3d.c
ompdebug:
	gcc -Wall -std=c99 -DDEBUG -fopenmp -o life3d life3d.c
omptimer:
	gcc -Wall -std=c99 -DTIMER -fopenmp -o life3d life3d.c
ompk:
	kinst-ompp gcc -Wall -std=c99 -DTIMER -fopenmp -o life3d life3d.c


