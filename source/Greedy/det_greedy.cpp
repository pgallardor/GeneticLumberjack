#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define LIMIT 42
#define RND_TESTS 4
//#define DEBUG
using namespace std;

typedef pair<int, int> ii;

char dir_str[4][7] = {"up", "down", "left", "right"};

class Tree{
	public:
		int _x, _y, _d, _h, _cu, _pu;
		Tree(int x, int y, int h, int d, int c, int p) :
			_x(x), _y(y), _h(h), _d(d), _cu(c), _pu(p) {}

		long getValue() { return _pu * _h * _d; }
		bool canCut(int E) { return E >= _d; }
		bool canDrop(Tree t) { return _cu * _h * _d > t._cu * t._h * t._d;}
};

vector<ii> value; //valores máximos posibles para cada árbol
vector<Tree> list; //lista de árboles con su información
vector<bool> down; //arreglo de bools que lleva cuenta de los árboles caídos durante la simulación
//double total = 0.0; 
int **grid; //campo

bool onBoundaries(int N, int x, int y){
	return (x >= 0) && (y >= 0) && (x < N) && (y < N);
}

int costToGo(int x1, int y1, int x2, int y2){
	return abs(x2 - x1) + abs(y2 - y1);
}

long simulate(int N, int idx, int dir, bool dropping){
	long result = 0;
	int tree_propag = idx;

	while (tree_propag >= 0){
		//printf("tree_propag = %d\n", tree_propag);
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
	queue<ii> q;
	int queued = 0;
	int sol = -1, best_value = -1, t_sol, t_best;
	bool canCutSomething = false;
	map<ii, bool> visited;

	ii init(y, x);
	q.push(init);
	visited[init] = true;

	//falta alguna wea para limitar la búsqueda
	while(queued < LIMIT && !q.empty()){
		ii act = q.front(); q.pop();
		int tree_idx = grid[act.first][act.second];
		if (tree_idx != -1 && !down[tree_idx]){
			Tree buff = list[tree_idx];
			int tx = buff._x, ty = buff._y, cost = costToGo(x, y, tx, ty);
			queued++;
			double ratio = double(value[tree_idx].first) / (cost + buff._d);
			if (buff.canCut(E - cost) && ratio > best_value){
				best_value = ratio;
				sol = tree_idx;
				canCutSomething = true;
			}
			else if (ratio > best_value){
				t_sol = tree_idx;
				t_best = ratio;
			}
		}
		ii neighs[4];
		neighs[UP] = ii(act.first + 1, act.second); neighs[DOWN] = ii(act.first - 1, act.second);
		neighs[LEFT] = ii(act.first, act.second - 1); neighs[RIGHT] = ii(act.first, act.second + 1);

		for (int i = 0; i < 4; i++){
			if (onBoundaries(N, neighs[i].second, neighs[i].first) && !visited.count(neighs[i])){
				q.push(neighs[i]);
				visited[neighs[i]] = true;
			}
		}

	}

	if (!canCutSomething) sol = t_sol;

	return sol;
}

void calculateValues(int N, int T){
	for (int i = 0; i < T; i++){
		long val = list[i].getValue(), best_drop_value = -1;
		int dir = -1;
		for (int direction = 0; direction < 4; direction++){
			long tmp = simulate(N, i, direction, false);
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

void generate_test(int tcase, int N, int &x, int &y, int &E){
	if (tcase == RND_TESTS) return;
	switch(tcase){
		case 1:{
			int perc = ceil(E*0.15);
			x += min(N - 1, perc/2);
			y += min(N - 1, perc/2 + perc%2);
			E -= perc;
			break;
		}
		case 2:{
			int perc = ceil(E*0.10);
			E -= perc;
			y += min(perc, N - 1);
			break;
		}
		case 3:{
			int perc = ceil(E*0.10);
			E-= perc;
			x += min(N - 1, perc);
		}
	}
}

int main(){
	int E, N, T, x, y, h, d, c, p;
	scanf("%d %d %d", &E, &N, &T);

	init(N);

	for (int i = 0; i < T; i++){
		scanf("%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p);
		Tree tr(x, y, h, d, c, p);
		list.push_back(tr);
		grid[y][x] = i;
	}
	
	down.assign(T, false);
	calculateValues(N, T);
	

	int rnd_tests = RND_TESTS;
	long best_profit = -1;
	int b_energy = -1, bx = -1, by = -1;
	vector<int> best_config, tmp_config;

	while(rnd_tests--){
		down.assign(T, false);

		/*for (int i = 0; i < T; i++){
			printf("tree id: %d, est. value: %.2f, cut %s\n", i, (double)value[i].first, dir_str[value[i].second]);
		}*/

		int tmp_energy = E, energy;
		int xi = 0, yi = 0, tree_index, t_xi, t_yi;

		generate_test(rnd_tests + 1, N, xi, yi, tmp_energy);

		energy = tmp_energy; t_xi = xi; t_yi = yi;

		#ifdef DEBUG
			fprintf(stderr, "case %d: starting at (%d, %d)\n", RND_TESTS - rnd_tests, xi, yi);
		#endif

		long useless = 0;
		while (energy > 0){
			tree_index = next(N, energy, xi, yi);
			Tree buff = list[tree_index];
			//print_moves(energy, xi, yi, buff._x, buff._y);
			energy -= costToGo(xi, yi, buff._x, buff._y);
			tmp_config.push_back(tree_index);

			if (energy <= 0|| !buff.canCut(energy)) break;


			useless += buff.getValue();
			xi = buff._x; yi = buff._y;
			down[tree_index] = true;
			energy -= buff._d;

			//printf("cut %s\n", dir_str[value[tree_index].second]);
			useless += simulate(N, tree_index, value[tree_index].second, true);
		}

		#ifdef DEBUG
			fprintf(stderr, "PROFIT: %ld\n", useless);
		#endif

		if (best_profit < useless){
			best_profit = useless;
			bx = t_xi; by = t_yi;
			best_config.clear();
			best_config.insert(best_config.end(), tmp_config.begin(), tmp_config.end());
		}
		tmp_config.clear();
	}

	b_energy = E;
	down.assign(T, false);
	print_moves(b_energy, 0, 0, bx, by);
	
	for (int i = 0; i < best_config.size(); i++){
		if (b_energy <= 0) break;
		int trindex = best_config[i];
		Tree buff = list[trindex];
		print_moves(b_energy, bx, by, buff._x, buff._y);

		if (b_energy <= 0 || !buff.canCut(b_energy)) break;

		bx = buff._x; by = buff._y;
		down[trindex] = true;
		b_energy -= buff._d;
		printf("cut %s\n", dir_str[value[trindex].second]);
		long sss = simulate(N, trindex, value[trindex].second, true);
	}

	return 0;
}
