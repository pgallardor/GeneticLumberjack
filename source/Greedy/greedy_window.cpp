#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <queue>
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
int LIMIT = 512;
int MAX_CASES = 200;
// #define DEBUG
using namespace std;

typedef pair<int, int> ii;

char dir_str[4][7] = {"up", "down", "left", "right"};

class Tree{
	public:
		int _x, _y, _d, _h, _cu, _pu;
		Tree(int x, int y, int h, int d, int c, int p) :
			_x(x), _y(y), _h(h), _d(d), _cu(c), _pu(p) {}

		double getValue() { return _pu * _h * _d; }
		bool canCut(int E) { return E >= _d; }
		bool canDrop(Tree t) { return _cu * _h * _d > t._cu * t._h * t._d;}
};

vector<ii> value, points;
vector<Tree> list;
vector<bool> down;
double total = 0.0f;
int **grid;

bool onBoundaries(int N, int x, int y){
	return (x >= 0) && (y >= 0) && (x < N) && (y < N);
}

int costToGo(int x1, int y1, int x2, int y2){
	return abs(x2 - x1) + abs(y2 - y1);
}

void generate_points(int N){
	int divider = 5;
	if (N >= 1 && N <= 50) divider = 2;
	if (N > 50 && N <= 250) divider = 3;
	if (N > 250 && N <= 500) divider = 4;

	int aug = N/divider;

	for(int x = 0; x < N; x+=aug){
		for (int y = 0; y < N; y+=aug){
			points.push_back(ii(x, y));
		}
	}
}

double simulate(int N, int idx, int dir, bool dropping){
	double result = 0.0f;
	int tree_propag = idx;

	while (tree_propag >= 0){
		Tree t = list[tree_propag];
		int H = t._h, xi = t._x, yi = t._y;

		if (H == 1) break;

		for (int i = 1; i < H; i++){
			if (dir == UP) yi++;
			if (dir == DOWN) yi--;
			if (dir == LEFT) xi--;
			if (dir == RIGHT) xi++;

			if (!onBoundaries(N, xi, yi)){
				tree_propag = -1;
				break;
			}

			int to_check = grid[yi][xi];
			if (to_check != -1 && !down[to_check]){
				if (!t.canDrop(list[to_check])){
					tree_propag = -1;
					break;
				}

				Tree dropped = list[to_check];
				result += dropped.getValue();
				tree_propag = to_check;

				if (dropping){
					down[to_check] = true;
				}

				break;
			}

			if (i + 1 == H){
				tree_propag = -1;
				break;
			}
		}
	}

	return result;
}

int next(int N, int E, int x, int y){
	int x_begin = max(0, x - LIMIT/2);
	int x_end = min(N, x + LIMIT/2);
	int y_begin = max(0, y - LIMIT/2);
	int y_end = min(N, y + LIMIT/2);
	int best_tree = -1;
	double val = -1.0;
	for (int i = x_begin; i < x_end; i++) {
		for (int j = y_begin; j < y_end; j++) {
			int t = grid[j][i];
			if (t != -1 and not down[t]) {
				Tree tree = list[t];
				double ratio = double(value[t].first) / (tree._d + costToGo(x, y, tree._x, tree._y));
				if (ratio > val) {
					best_tree = t;
					val = ratio;
				}
			}
		}
	}
	if (best_tree == -1) {
		for (auto tt: list) {
			int idx = grid[tt._y][tt._x];
			int cost = costToGo(x, y, tt._x, tt._y);
			double ratio = double(value[idx].first) / (cost + tt._d);
			if (not down[idx] and tt.canCut(E - cost) and ratio > val) {
				best_tree = idx;
				val = ratio;
			}
		}
	}
	return best_tree;
}

void calculateValues(int N, int T){
	for (int i = 0; i < T; i++){
		double val = list[i].getValue(), best_drop_value = -1.0f;
		int dir = -1;
		for (int direction = 0; direction < 4; direction++){
			double tmp = simulate(N, i, direction, false);
			if (tmp > best_drop_value){
				dir = direction;
				best_drop_value = tmp;
			}
		}
		value.push_back(ii(val + best_drop_value, dir));
	}
}

void print_moves(int &E, int x1, int y1, int x2, int y2){
	if (x1 < x2){
		while (E && x1++ < x2){
			printf("move right\n");
			E--;
		}
	}

	else if (x1 > x2){
		while (E && x1-- > x2){
			printf("move left\n");
			E--;
		}
	}

	if (!E) return;

	if (y1 < y2){
		while (E && y1++ < y2){
			printf("move up\n");
			E--;
		}
	}

	else if (y1 > y2){
		while(E && y1-- > y2){
			printf("move down\n");
			E--;
		}
	}
}

void init(int N){
	grid = new int*[N];
	for (int i = 0; i < N; i++){
		grid[i] = new int[N];
		for (int j = 0; j < N; j++) grid[i][j] = -1;
	}
}

void free(int N){
	for (int i = 0; i < N; i++)
		delete[] grid[i];
	delete[] grid;
}

void gen_point(int N, int &x, int &y){
	x = rand() % (N >> 1);
	y = rand() % (N >> 1);
}


int main(){
	int E, N, T, x, y, h, d, c, p;
	scanf("%d %d %d", &E, &N, &T);
	// if (N == 1000 or T == 1000) {
	// 	LIMIT = 32;
	// 	MAX_CASES = 60;
	// }
	init(N);
	srand(time(NULL));

	for (int i = 0; i < T; i++){
		scanf("%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p);
		Tree tr(x, y, h, d, c, p);
		list.push_back(tr);
		grid[y][x] = i;
	}

	down.assign(T, false);
	calculateValues(N, T);

	int bx, by, cases = MAX_CASES, tree_index, xi, yi, tx, ty, energy;

	if (N == 250 && T == 793) cases = 2;

	double bprofit = -1.0f, useless;
	while(cases--){
		down.assign(T, false);
		energy = E;
		xi = 0; yi = 0;

		useless = 0.0f;

		if (cases) gen_point(N, xi, yi);
		energy -= costToGo(0, 0, xi, yi);
		tx = xi; ty = yi;

		while (energy > 0){
			tree_index = next(N, energy, xi, yi);
			Tree buff = list[tree_index];
			//print_moves(energy, xi, yi, buff._x, buff._y);
			energy -= costToGo(xi, yi, buff._x, buff._y);

			if (!energy || !buff.canCut(energy)) break;

			xi = buff._x; yi = buff._y;
			down[tree_index] = true;
			energy -= buff._d;

			//printf("cut %s\n", dir_str[value[tree_index].second]);
			useless += buff.getValue() + simulate(N, tree_index, value[tree_index].second, true);
		}

		#ifdef DEBUG
			fprintf(stderr, "Case %d. Starting from (%d, %d). PROFIT = %.0f\n", MAX_CASES - cases, tx, ty, useless);
		#endif

		if (useless > bprofit){
			bx = tx; by = ty;
			bprofit = useless;
		}
	}

	int final_energy = E;


	#ifdef DEBUG
		fprintf(stderr, "BEST START POINT: (%d, %d)\n", bx, by);
		fprintf(stderr, "Best case: start (%d, %d) /w PROFIT %ld\n", bx, by, int(bprofit));
	#endif

	down.assign(T, false);
	print_moves(final_energy, 0, 0, bx, by);
	while (final_energy > 0){
		tree_index = next(N, final_energy, bx, by);
		Tree tg = list[tree_index];
		print_moves(final_energy, bx, by, tg._x, tg._y);
		//energy -= costToGo(xi, yi, buff._x, buff._y);

		if (final_energy <= 0 || !tg.canCut(final_energy)) break;

		bx = tg._x; by = tg._y;
		down[tree_index] = true;
		final_energy -= tg._d;

		printf("cut %s\n", dir_str[value[tree_index].second]);
		simulate(N, tree_index, value[tree_index].second, true);
	}

	return 0;
}