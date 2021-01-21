#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string>
#include <windows.h>

using namespace std;

struct SYMBOL {
	unsigned char color;
	unsigned char text;
};

struct PICTURE {
	int width;
	int height;

	SYMBOL** symbols;

	PICTURE(int w, int h) {
		width = w;
		height = h;

		symbols = new SYMBOL * [h];

		for (int i = 0; i < h; i++) {
			symbols[i] = new SYMBOL[w];
		}
	}
};

struct COOR {
	int x;
	int y;

	COOR() {
		x = 0;
		y = 0;
	}
};

struct ACOOR {
	COOR* data;
	int size;
	int pos;

	ACOOR(int a) {
		size = a;
		data = new COOR[size];
		pos = -1;
	}

	void add(int x, int y) {
		pos++;

		data[pos].x = x;
		data[pos].y = y;
	}
};

struct CELL {
	int import;
	int type;
	int rept;

	CELL() {
		import = -2;
		type = 2;
		rept = -1;
	}
};

struct DATAU {
	int prob[4][2];
	int count;
	int** value;
	CELL cell;

	void create(int cPlayer) {

		value = new int* [4];

		for (int i = 0; i < 4; i++) {
			value[i] = new int[cPlayer + 1];
			prob[i][0] = prob[i][1] = -2;

			for (int j = 0; j <= cPlayer; j++) {
				value[i][j] = 0;
			}
		}

		count = 0;
	}
};

struct DATA {
	DATAU** data;
	int cPlayer;
	int mSize;

	DATA(int a, int b) {
		cPlayer = a;
		mSize = b;

		data = new DATAU * [mSize];
		for (int i = 0; i < mSize; i++) {
			data[i] = new DATAU[mSize];

			for (int j = 0; j < mSize; j++) {
				data[i][j].create(cPlayer);
			}
		}
	}

	void add(int x, int y, int count, int player) {
		if (count > data[x][y].count) {
			data[x][y].count = count;
		}
		data[x][y].value[count][player]++;
	}
};

struct MAP {
	int** value;
	int mSize;

	MAP(int size) {
		mSize = size;
		value = new int* [mSize];
		for (int i = 0; i < mSize; i++) {
			value[i] = new int[mSize];
		}
		CLEAR();
	}

	void CLEAR() {
		for (int i = 0; i < mSize; i++) {
			for (int j = 0; j < mSize; j++) {
				value[i][j] = 0;
			}
		}
	}
};

int isMax(int a, int b) {
	return a == max(a, b);
}

bool isMaxCell(CELL a, CELL b) {
	if (a.import > b.import || a.import == b.import && a.type < b.type || a.import == b.import && a.type == b.type && a.rept > b.rept) {
		return true;
	}
	return false;
}

CELL getMaxCell(DATA data) {
	CELL cell;

	for (int x = 0; x < data.mSize; x++) {
		for (int y = 0; y < data.mSize; y++) {
			if (isMaxCell(data.data[x][y].cell, cell)) {
				cell = data.data[x][y].cell;
			}
		}
	}

	return cell;
}

bool isEqualCells(CELL a, CELL b) {
	if (a.import == b.import && a.rept == b.rept && a.type == b.type) {
		return true;
	}
	return false;
}

int countInData(DATA data, CELL maxCell) {
	int count = 0;

	for (int x = 0; x < data.mSize; x++) {
		for (int y = 0; y < data.mSize; y++) {
			if (isEqualCells(data.data[x][y].cell, maxCell)) {
				count++;
			}
		}
	}

	return count;
}

ACOOR getMaxData(DATA data) {
	CELL maxCell = getMaxCell(data);
	ACOOR coords(countInData(data, maxCell));

	for (int x = 0; x < data.mSize; x++) { //add all coord
		for (int y = 0; y < data.mSize; y++) {
			if (isEqualCells(data.data[x][y].cell, maxCell)) {
				coords.add(x, y);
			}
		}
	}

	return coords;
}

int random(int a) {
	return round((a - 1) * (double)(rand() % 100) / 100);
}

COOR getRandomCoord(ACOOR coords) {
	srand(time(NULL));
	return coords.data[random(coords.size)];
}

DATA calc(MAP map, int player, int cPlayer) {
	DATA data(cPlayer, map.mSize);

	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			if (map.value[x][y] == 0) {

				for (int a = 0; a < map.mSize; a++) { //calc value
					data.add(x, y, 0, map.value[a][y]);
					data.add(x, y, 1, map.value[x][a]);
					if (x == y) {
						data.add(x, y, 2, map.value[a][a]);
					}
					if (x == map.mSize - (y + 1)) {
						data.add(x, y, 2 + (x == y), map.value[a][map.mSize - (a + 1)]);
					}
				}

				for (int i = 0; i <= data.data[x][y].count; i++) { //calc prob
					data.data[x][y].prob[i][0] = data.data[x][y].prob[i][1] = -1;

					if (data.data[x][y].value[i][player] == map.mSize - data.data[x][y].value[i][0]) {
						data.data[x][y].prob[i][0] = data.data[x][y].value[i][player];
					}

					for (int j = 1; j <= cPlayer; j++) {
						if (j != player && data.data[x][y].value[i][j] == map.mSize - data.data[x][y].value[i][0]) {
							data.data[x][y].prob[i][1] = data.data[x][y].value[i][j];
							break;
						}
					}

					for (int j = 0; j < 2; j++) { //calc rept
						data.data[x][y].cell.rept += (data.data[x][y].prob[i][j] > -1);
					}

					//calc import and type
					if (max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]) > data.data[x][y].cell.import || (max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]) == data.data[x][y].cell.import && data.data[x][y].prob[i][0] >= data.data[x][y].prob[i][1])) {
						data.data[x][y].cell.import = max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]);
						data.data[x][y].cell.type = !isMax(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]);
					}
				}
			}
		}
	}

	return data;
}

int main() {

	MAP map(3);

	for (int i = 0; i < map.mSize; i++) {
		for (int j = 0; j < map.mSize; j++) {
			cin >> map.value[i][j];
		}
	}

	//test
	cout << "Result: x = " << getRandomCoord(getMaxData(calc(map, 1, 2))).x << " y = " << getRandomCoord(getMaxData(calc(map, 1, 2))).y << endl;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int k = 1; k < 255; k++) {
		SetConsoleTextAttribute(hConsole, k);
		cout << k << " I love Valeriy!" << endl;
	}

	return 0;
}

