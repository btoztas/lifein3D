all:
	gcc -Wall -g -std=c99 life3d.c -o life3d
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
	gcc -Wall -std=c99 -fopenmp -o life3d life3d.c
omptime:
	gcc -Wall -std=c99 -DTIME -fopenmp -o life3d life3d.c
gmon:
	gcc -Wall -pg  -std=c99 -DTIME -fopenmp -o life3d life3d.c
