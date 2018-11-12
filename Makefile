# El archivo que calcula la solución (el que sería subido a optil.io)
SOLUTION=source/Greedy/greedy.cpp

# make gen: compila y ejecuta el programa que genera casos de prueba
gen:
	g++ test_gen.cpp -o test_gen
	./test_gen > input.txt

# make run: compila y ejecuta el programa principal
run:
	g++ $(SOLUTION)
	./a.out < input.txt > output.txt

# make plot: grafica la solución obtenida por 'make run'
plot:
	python plot.py
