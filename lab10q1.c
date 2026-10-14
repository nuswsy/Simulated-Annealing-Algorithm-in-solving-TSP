#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#define MAP_N 8
double distance(int a, int b) {
	int xa = a % MAP_N, ya = a / MAP_N, xb = b % MAP_N, yb = b / MAP_N;
	return (double)sqrt((xa - xb) * (xa - xb) + (ya - yb) * (ya - yb));

}
//=============================================
double calculateCost(int route[]) {
	double sum = 0;
	for (int i = 0;i < MAP_N*MAP_N-1;i++) {
		sum += distance(route[i], route[i + 1]);
	}
	sum += distance(route[MAP_N*MAP_N-1], route[0]);
	return sum;
}
//=============================================
void swap(int route[],int i,int j) {
	int temp = route[i];
	route[i] = route[j];
	route[j] = temp;
}
//=============================================
void copyRoute(int destination[], int source[]) {
	for (int i = 0; i < MAP_N*MAP_N; i++) {
		destination[i] = source[i];
	}
}
int main() {
	srand((unsigned int)time(NULL));
	int arr[4][4] = {
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{8, 9, 10,11},
		{12,13,14,15}
	};//this is just for visualization of a 4x4 map
	int route[MAP_N*MAP_N];
	for (int i = 0;i < MAP_N * MAP_N;i++) {
		route[i] = i;
	}
	//this is the visiting order, mind that the route must return to 0;
	int currentroute[MAP_N*MAP_N], newroute[MAP_N*MAP_N], bestsroute[MAP_N*MAP_N];
	copyRoute(currentroute, route);
	copyRoute(bestsroute, route);

	double currentCost = calculateCost(currentroute);
	double bestCost = currentCost;
	double temperature = 500.0;
	double coolingRate = 0.001;
	double minTemperature = 0.000001;
	
	
	
	while (temperature > minTemperature) {

		// 1. Generate a new route
		copyRoute(newroute, currentroute);

		int i = rand() % (N*N);
		int j = rand() % (N*N);

		while (i == j) {
			j = rand() % (N*N);
		}

		swap(newroute, i, j);

		// 2. Calculate new cost
		double newCost = calculateCost(newroute);

		// 3. Decide whether to accept
		double delta = newCost - currentCost;

		if (delta <= 0) {
			copyRoute(currentroute, newroute);
			currentCost = newCost;
		}
		else {
			double probability = exp(-delta / temperature);
			double r = (double)rand() / RAND_MAX;

			if (r < probability) {
				copyRoute(currentroute, newroute);
				currentCost = newCost;
			}
		}

		// 4. Update best solution
		if (newCost < bestCost) {
			bestCost = newCost;
			copyRoute(bestsroute, newroute);
		}

		// 5. Cool down
		temperature *= (1.0 - coolingRate);
	}
	printf("Best cost: %.4f\n", bestCost);

	printf("Best route:\n");
	for (int i = 0; i < MAP_N*MAP_N; i++) {
		printf("%d ", bestsroute[i]);
	}
	printf("-> %d\n", bestsroute[0]);

	return 0;
}