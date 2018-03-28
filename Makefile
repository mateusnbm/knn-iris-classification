run:	
	gcc -Wall -o iris-knn iris-knn.c	
	./iris-knn
	gnuplot -e "set terminal png; set output \"graph.png\"; plot  \"graph.dat\" using 1:2 title \"Column\", \"graph.dat\" using 1:3 title \"Beam\""
