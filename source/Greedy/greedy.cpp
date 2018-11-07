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

vector<ii> value;
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

double simulate(int N, int idx, int dir, bool dropping = true){
	double result = 0.0f;
	int tree_propag = idx;

	while (tree_propag >= 0){
		Tree t = list[tree_propag];
		int H = t._h, xi = t._x, yi = t._y;

		for (int i = 0; i < H; i++){
			if (dir == UP) yi++;
			if (dir == DOWN) yi--;
			if (dir == LEFT) xi--;
			if (dir == RIGHT) xi++;

			if (!onBoundaries(N, xi, yi)){
				tree_propag = -1;
				break;
			}

			int to_check = grid[yi][xi];
			if (to_check != -1 && !down[to_check] && t.canDrop(list[to_check])){
				Tree dropped = list[to_check];
				result += dropped.getValue();
				tree_propag = to_check;
				if (dropping)
					down[to_check] = true;
				break;
			}

			if (i + 1 == H) 
				tree_propag = -1;
		}
	}

	return result;
}

int next(int N, int E, int x, int y){
	queue<ii> q;
	int queued = 0, limit = 4;
	int sol = -1, best_value = -1, t_sol, t_best;
	bool canCutSomething = false;
	map<ii, bool> visited;

	ii init(y, x);
	q.push(init);
	visited[init] = true;

	//falta alguna wea para limitar la búsqueda
	while(queued < limit && !q.empty()){
		ii act = q.front(); q.pop();
		int tree_idx = grid[act.first][act.second];
		if (tree_idx != -1 && !down[tree_idx]){
			Tree buff = list[tree_idx];
			int tx = buff._x, ty = buff._y, cost = costToGo(x, y, tx, ty);
			queued++;

			if (buff.canCut(E - cost) && value[tree_idx].first / cost > best_value){
				best_value = value[tree_idx].first / cost;
				sol = tree_idx;
				canCutSomething = true;
			}
			else if (value[tree_idx].first / cost > best_value){
				t_sol = tree_idx;
				t_best = value[tree_idx].first / cost;
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

	/*for (int i = 0; i < T; i++){
		printf("tree id: %d, est. value: %.2f, cut %s\n", i, (double)value[i].first, dir_str[value[i].second]);
	}*/

	int energy = E;
	int xi = 0, yi = 0, tree_index;
	double useless;
	while (energy > 0){
		tree_index = next(N, energy, xi, yi);
		if (tree_index == -1) return -1;
		//printf("going to tree %d\n", tree_index);
		Tree buff = list[tree_index];
		print_moves(energy, xi, yi, buff._x, buff._y);

		if (!energy || !buff.canCut(energy)) break;

		//printf("energy remaining: %d\n", energy);
		xi = buff._x; yi = buff._y;
		down[tree_index] = true;
		energy -= buff._d;

		printf("cut %s\n", dir_str[value[tree_index].second]);
		useless = simulate(N, tree_index, value[tree_index].second);
	}

	return 0;
}

