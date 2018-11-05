// Este programa genera casos de prueba con valores dentro de los rangos
// especificados.
#include <iostream>
#include <ctime>
#include <cstring>

#define STEP_MIN 1
#define STEP_MAX 5000
#define GRID_SIZE_MIN 1
#define GRID_SIZE_MAX 1000
#define TREE_MIN 1
#define TREE_MAX 10000
#define HEIGHT_MIN 1
#define HEIGHT_MAX 20
#define WIDTH_MIN 1
#define WIDTH_MAX 20
#define UNIT_COST_MIN 1
#define UNIT_COST_MAX 500
#define UNIT_PROFIT_MIN 1
#define UNIT_PROFIT_MAX 500

using namespace std;

// genera un valor aleatorio entre min y max, inclusive
int random(int min, int max);

int main() {
	srand(time(nullptr));
	int t = random(STEP_MIN, STEP_MAX);
	int n = random(GRID_SIZE_MIN, GRID_SIZE_MAX);
	int nn = n*n;
	// los min son necesarios para no exceder el tamaño de la grilla y para que
	// el valor min pasado al random no sea mayor que el max
	int k = random(min(TREE_MIN, nn), min(TREE_MAX, nn));
	int M[nn];
	memset(&M[0], 0, nn*sizeof(int));
	printf("%d %d %d\n", t, n, k);
	for (int i = 0; i < k; i++) {
		M[i] = 1;
	}
	// Fisher-Yates
	for (int i = 0; i < nn; i++) {
		int j = random(0, i);
		int aux = M[i];
		M[i] = M[j];
		M[j] = aux;
	}
	// descomentar para visualizar donde se están colocando los árboles
	// for (int i = 0; i < n; i++) {
	// 	for (int j = 0; j < n; j++) {
	// 		cerr << M[i*n + j] << " ";
	// 	}
	// 	cerr << endl;
	// }
	for (int i = 0; i < nn; i++) {
		if (M[i]) {
			int x = i / n;
			int y = i % n;
			int h = random(HEIGHT_MIN, HEIGHT_MAX);
			int d = random(WIDTH_MIN, WIDTH_MAX);
			int c = random(UNIT_COST_MIN, UNIT_COST_MAX);
			int p = random(UNIT_PROFIT_MIN, UNIT_PROFIT_MAX);
			printf("%d %d %d %d %d %d\n", x, y, h, d, c, p);
		}
	}
	return 0;
}

int random(int min, int max) {
	return rand() % (max - min + 1) + min;
}
