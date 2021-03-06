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
#define LIMIT 256 //32
using namespace std;

typedef pair<double, int> di;
typedef pair<double, pair<int, int> > dii;

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

double table[4][10005];
vector<ii> compression_row, compression_column;

void DP(int TX, int t_idx, int hi, int dir){
	if (hi == H) {
		table[dir][t_idx] = (double)list[t_idx].getValue() / list[t_idx]._d; 
		return;
	}

	
}

long bestValueCalc(int N, int idx){
	Tree buff = list[idx];
	int x = buff._x, y = buff._y, H = buff._h;

	if (H == 1) return 0;

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			if (grid[i][j] != -1) compression_row.push_back(ii(grid[i][j], j));
			if (grid[j][i] != -1) compression_column.push_back(ii(grid[j][i], j));
		}
		int lr = compression_row.size(), lc = compression_column.size();
		DP(lr, compression_row[0].first, 0, UP);
		DP(lr, compression_row[lr - 1].first, lr - 1, DOWN);
		DP(lc, compression_column[0].first, 0, UP);
		DP(lc, compression_row[lc - 1].first, lc - 1, DOWN);

		compression_row.clear();
		compression_column.clear();
	}
}

//

long simulate(int N, int idx, int dir, bool dropping){
	long result = 0;
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

//esto funcionará lol??
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
	long useless = 0;
	while (energy > 0){
		tree_index = next(N, energy, xi, yi);
		Tree buff = list[tree_index];
		useless += buff.getValue();
		print_moves(energy, xi, yi, buff._x, buff._y);

		if (!energy || !buff.canCut(energy)) break;

		//printf("energy remaining: %d\n", energy);
		xi = buff._x; yi = buff._y;
		down[tree_index] = true;
		energy -= buff._d;

		printf("cut %s\n", dir_str[value[tree_index].second]);
		useless += simulate(N, tree_index, value[tree_index].second, true);
	}
	
	// fprintf(stderr, "PROFIT: %ld\n", useless);
	return 0;
}
