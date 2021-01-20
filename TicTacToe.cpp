﻿#include <stdio.h>

struct COORD {
	int x;
	int y;

	COORD() {
		x = 0;
		y = 0;
	}
};

struct ACOORD {
	COORD * data;
	int size;
	int pos;

	ACOORD(int a) {
		size = a;
		data = new COORD[size];
		pos = -1;
	}

	void add(int x, int y) {
		pos++;

		data[pos].x = x; 
		data[pos].y = y;
	}
};

struct DATAU {
	int import;
	int prob[4][2];
	int type;
	int count;
	int** value;
	int rept;

	void create(int cPlayer) {
		value = new int* [4];


		for (int i = 0; i < 4; i++) {
			value[i] = new int[cPlayer + 1];
			prob[i][0] = prob[i][1] = -2;


			for (int j = 0; j <= cPlayer; j++) {
				value[i][j] = 0;
			}

		}

		import = -2;
		rept = 0;
		count = 0;
		type = 0;
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

int max(int a, int b) {
	if (a >= b) {
		return a;
	}
	return b;
}

int isMax(int a, int b) {
	return a == max(a, b);
}

void debugDATA(DATA data) {
	printf("////debugDATA////\n");
	for (int x = 0; x < data.mSize; x++) {
		for (int y = 0; y < data.mSize; y++) {
			printf("x = %d, y = %d\n", x, y);

			printf("\t import = %d\n", data.data[x][y].import);
			printf("\t type = %d\n", data.data[x][y].type);
			printf("\t rept = %d\n", data.data[x][y].rept);

			//for (int i = 0; i <= data.data[x][y].count; i++) {
			//	printf("\t \t count = %d\n", i);	
			//	printf("\t \t \t prob0 = %d, prob1 = %d\n", data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]);			
			//}
		}
	}
}

void printCoords(ACOORD coords) {
	printf("////PrintResult:////\n");
	for (int i = 0; i < coords.size; i++) {
		printf("x = %d, y = %d\n", coords.data[i].x, coords.data[i].y);
	}
}

ACOORD getMaxData(DATA data) {
	int count = 0, maxImport = -2, minType = 2, maxRept = -1;

	for (int x = 0; x < data.mSize; x++) { //calc maxImport
		for (int y = 0; y < data.mSize; y++) {
			if (data.data[x][y].import > maxImport) {
				maxImport = data.data[x][y].import;
			}
		}
	}

	for (int x = 0; x < data.mSize; x++) { //calc minType
		for (int y = 0; y < data.mSize; y++) {
			if (data.data[x][y].import == maxImport && data.data[x][y].type < minType) {
				minType = data.data[x][y].type;
			}
		}
	}

	for (int x = 0; x < data.mSize; x++) { //calc maxRept
		for (int y = 0; y < data.mSize; y++) {
			if (data.data[x][y].import == maxImport && data.data[x][y].type == minType && data.data[x][y].rept > maxRept) {
				maxRept = data.data[x][y].rept;
			}
		}
	}

	for (int x = 0; x < data.mSize; x++) { //calc count
		for (int y = 0; y < data.mSize; y++) {
			if (data.data[x][y].import == maxImport && data.data[x][y].type == minType && data.data[x][y].rept == maxRept) {
				count++;
			}
		}
	}

	ACOORD coords(count);

	for (int x = 0; x < data.mSize; x++) { //add all coord
		for (int y = 0; y < data.mSize; y++) {
			if (data.data[x][y].import == maxImport && data.data[x][y].type == minType && data.data[x][y].rept == maxRept) {
				coords.add(x, y);
			}
		}
	}

	printf("maxImport = %d, minType = %d, maxRept = %d\n", maxImport, minType, maxRept);
	printCoords(coords);

	return coords;
}

COORD getCoord(ACOORD coords) {
	COORD coord;


	return coord;
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
						data.data[x][y].rept += (data.data[x][y].prob[i][j] > -1);
					}

					//calc import and type
					if (max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]) > data.data[x][y].import || (max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]) == data.data[x][y].import && data.data[x][y].prob[i][0] >= data.data[x][y].prob[i][1])) {
						data.data[x][y].import = max(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]);
						data.data[x][y].type = !isMax(data.data[x][y].prob[i][0], data.data[x][y].prob[i][1]);
					}
				}
			}
		}
	}

	debugDATA(data); //debug
	getMaxData(data);

	return data;
}

int main() {

	MAP map(3);

	for (int i = 0; i < map.mSize; i++) {
		for (int j = 0; j < map.mSize; j++) {
			scanf_s("%d", &map.value[i][j]);
		}
	}

	calc(map, 1, 2);

	return 0;
}

