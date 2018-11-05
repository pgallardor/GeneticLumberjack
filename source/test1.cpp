#include "genetic.hpp"
#include <ctime>
using namespace std;

int main(){
	srand(time(NULL));
	vector<Tree> v;
	int t, n, tree;
	int x, y, h, d, c, p;
	scanf("%d %d %d", &t, &n, &tree);

	for (int i = 0; i < tree; i++){
		scanf("%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p);
		Tree tr(x, y, h, d, c, p);
		v.push_back(tr);
	}

	Info i(v, t, n);

	Genetic gen(&i, tree, 60, 0.15, 0.05, 0.90, 3.00);
	Chromosome sol = gen.run(10);
	printf("Best: %.2f\n", sol.getFitness());
	return 0;
}