#include <cstdio>
#include <vector>
#define DEBUG

typedef vector<vector<int> > DropList;

int E, N, T;


void calculate_values();
void calculate_worthtrees();

bool isWorth(int t);
vector<int> getAnnoy(int t);
bool isEnabler(int e, int t);


int main(){
	
	// read everything

	calculate_values(); //DP fixed
	calculate_worthtrees(); //checks the trees 
							//belonging to the 6th or 7th percentile
	vector<int> best_route;
	while(something){
		vector<int> route = create_route(); //generate a random (or not random) route 
											//between possible worthtrees
		for(auto t: route){
			//look for best enabler(s)
			//place them before t
		}

		simulate_route(route);
		best_route = pick_best(route, best_route);
	}

	print_route(best_route);

	return 0;
}