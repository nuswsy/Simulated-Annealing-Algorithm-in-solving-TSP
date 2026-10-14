#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#define MAP_N 8
#include <windows.h>
#include <string.h>

#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")

#define GRID 8
#define TOWNS 13
double distance(int a, int b) {
	int xa = a % MAP_N, ya = a / MAP_N, xb = b % MAP_N, yb = b / MAP_N;
	return (double)sqrt((xa - xb) * (xa - xb) + (ya - yb) * (ya - yb));

}
//=============================================
double calculateCost(int route[]) {
	double sum = 0;
	for (int i = 0;i < 12;i++) {
		sum += distance(route[i], route[i + 1]);
	}
	sum += distance(route[12], route[0]);
	return sum;
}
//=============================================
void swap(int route[], int i, int j) {
	int temp = route[i];
	route[i] = route[j];
	route[j] = temp;
}
//=============================================
void copyRoute(int destination[], int source[]) {
	for (int i = 0; i < 13; i++) {
		destination[i] = source[i];
	}
}
int pointBetween(int a, int b, int c) {

	int xa = (a - 1) % 8;
	int ya = (a - 1) / 8;

	int xb = (b - 1) % 8;
	int yb = (b - 1) / 8;

	int xc = (c - 1) % 8;
	int yc = (c - 1) / 8;

	// Check whether A, B, C are collinear
	int cross = (xb - xa) * (yc - ya)
		- (yb - ya) * (xc - xa);

	if (cross != 0) {
		return 0;
	}

	// Check whether C lies between A and B
	int dot = (xc - xa) * (xc - xb)
		+ (yc - ya) * (yc - yb);

	return dot < 0;
}
int isValidRoute(int route[]) {

	for (int i = 0; i < 13; i++) {

		int a = route[i];
		int b = route[(i + 1) % 13];

		// Check whether this segment passes through another town
		for (int j = 0; j < 13; j++) {

			int c = route[j];

			if (c == a || c == b) {
				continue;
			}

			if (pointBetween(a, b, c)) {
				return 0;
			}
		}
	}

	return 1;
}
//===================================================================
// Visualization
//===================================================================
int visualRoute[TOWNS];

LRESULT CALLBACK WindowProc(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
) {
	switch (msg) {

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		int spacing = 65;
		int offset = 65;

		// Draw grid
		HPEN gridPen = CreatePen(
			PS_DOT, 1, RGB(180, 180, 180)
		);
		HGDIOBJ oldPen = SelectObject(hdc, gridPen);

		for (int i = 0; i < GRID; i++) {
			int p = offset + i * spacing;

			MoveToEx(hdc, offset, p, NULL);
			LineTo(hdc, offset + (GRID - 1) * spacing, p);

			MoveToEx(hdc, p, offset, NULL);
			LineTo(hdc, p, offset + (GRID - 1) * spacing);
		}

		SelectObject(hdc, oldPen);
		DeleteObject(gridPen);

		// Draw final route
		HPEN routePen = CreatePen(
			PS_SOLID, 3, RGB(0, 100, 220)
		);
		oldPen = SelectObject(hdc, routePen);

		for (int i = 0; i < TOWNS; i++) {

			int a = visualRoute[i];
			int b = visualRoute[(i + 1) % TOWNS];

			int xa = offset + ((a - 1) % GRID) * spacing;
			int ya = offset + ((a - 1) / GRID) * spacing;

			int xb = offset + ((b - 1) % GRID) * spacing;
			int yb = offset + ((b - 1) / GRID) * spacing;

			MoveToEx(hdc, xa, ya, NULL);
			LineTo(hdc, xb, yb);
		}

		SelectObject(hdc, oldPen);
		DeleteObject(routePen);

		// Draw town points and labels
		HBRUSH townBrush = CreateSolidBrush(
			RGB(220, 50, 50)
		);
		HGDIOBJ oldBrush = SelectObject(hdc, townBrush);

		SetBkMode(hdc, TRANSPARENT);

		for (int i = 0; i < TOWNS; i++) {

			int p = visualRoute[i];

			int x = offset + ((p - 1) % GRID) * spacing;
			int y = offset + ((p - 1) / GRID) * spacing;

			Ellipse(hdc, x - 6, y - 6, x + 6, y + 6);

			char label[16];
			sprintf(label, "%d", p);

			TextOutA(hdc, x + 8, y - 18,
				label, (int)strlen(label));
		}

		SelectObject(hdc, oldBrush);
		DeleteObject(townBrush);

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

void visualizeRoute(const int route[]) {

	for (int i = 0; i < TOWNS; i++) {
		visualRoute[i] = route[i];
	}

	HINSTANCE hInstance = GetModuleHandleA(NULL);

	WNDCLASSA wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "RouteWindow";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClassA(&wc);

	HWND hwnd = CreateWindowA(
		"RouteWindow",
		"Simulated Annealing - Best Route",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		650, 650,
		NULL, NULL, hInstance, NULL
	);

	if (hwnd == NULL) {
		printf("Cannot create visualization window.\n");
		return;
	}

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
//===================================================================
int main() {
	srand((unsigned int)time(NULL));
	int arr[8][8] = {
		{0, 1, 2, 3, 4, 5, 6, 7},
		{8, 9, 10,11,12,13,14,15},
		{16,17,18,19,20,21,22,23},
		{24,25,26,27,28,29,30,31},
		{32,33,34,35,36,37,38,39},
		{40,41,42,43,44,45,46,47},
		{48,49,50,51,52,53,54,55},
		{56,57,58,59,60,61,62,63}
	};//this is just for visualization of a 4x4 map
	int route[13] = { 7,9,12,28,17,37,34,43,53,58,62,14,39, };

	//this is the visiting order, mind that the route must return to 0;
	int currentroute[13], newroute[13], bestsroute[13];
	copyRoute(currentroute, route);
	copyRoute(bestsroute, route);

	double currentCost = calculateCost(currentroute);
	double bestCost = currentCost;
	double temperature = 5000.0;
	double coolingRate = 0.001;
	double minTemperature = 0.000001;



	while (temperature > minTemperature) {

		// 1. Generate a new route
		copyRoute(newroute, currentroute);

		int i = rand() % 13;
		int j = rand() % 13;

		while (i == j) {
			j = rand() % 13;
		}

		swap(newroute, i, j);
		if (isValidRoute(newroute)) {
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
	}
	printf("Best cost: %.4f\n", bestCost);

	printf("Best route:\n");
	for (int i = 0; i <13; i++) {
		printf("%d ", bestsroute[i]);
	}
	printf("-> %d\n", bestsroute[0]);
	visualizeRoute(bestsroute);
	return 0;
}