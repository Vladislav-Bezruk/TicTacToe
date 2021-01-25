#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string>
#include <windows.h>

#define nLines 7

#define d 100

#define defHeight 7
#define defWidth 8

#define defColor 15
#define Black 0
#define DarkBlue 1
#define DarkGreen 2
#define LightBlue 3
#define DarkRed 4
#define Magenta 5
#define Orange 6
#define LightGray 7
#define Gray 8
#define Blue 9
#define Green 10
#define Cyan 11
#define Red 12
#define Pink 13
#define Yellow 14
#define White 15

#define uc unsigned char

using namespace std;

struct ASTR {
	string * str;
	uc color;
	int size;

	ASTR(int a, string* b, int c) {
		size = a;
		str = b;
		color = c;
	}
};

struct SYMBOL {
	uc color;
	char text;

	SYMBOL() {
		color = defColor;
	}

	SYMBOL(uc c, uc t) {
		color = c;
		text = t;
	}
};

struct PICTURE {
	int width;
	int height;

	SYMBOL** symbols;

	PICTURE() {

	}

	PICTURE(int h, int w) {
		width = w;
		height = h;

		symbols = new SYMBOL* [h];

		for (int i = 0; i < h; i++) {
			symbols[i] = new SYMBOL[w];
		}
	}
};

struct APICTURE {
	PICTURE** pictures;
	int width;
	int height;

	APICTURE(int a, int b) {
		height = a;
		width = b;

		pictures = new PICTURE * [height];

		for (int i = 0; i < height; i++) {
			pictures[i] = new PICTURE [width];
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

void printPicture(PICTURE picture, bool newLine) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int x = 0; x < picture.height; x++) {
		for (int y = 0; y < picture.width; y++) {
			SetConsoleTextAttribute(hConsole, picture.symbols[x][y].color);
			cout << picture.symbols[x][y].text;
		}
		if (newLine == true) {
			cout << endl;
		}
	}

	SetConsoleTextAttribute(hConsole, defColor);
}

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

DATA calcValue(MAP map, int cPlayer, bool analyze) {
	DATA data(cPlayer, map.mSize);

	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			if (map.value[x][y] == 0 || analyze == true) {
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
			}
		}
	}

	return data;
}

ACOOR analyzeGame(MAP map, int cPlayer) {
	ACOOR coords(map.mSize);
	DATA data = calcValue(map, cPlayer, true);

	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			for (int i = 0; i <= data.data[x][y].count; i++) {
				for (int j = 1; j <= cPlayer; j++) {
					if (data.data[x][y].value[i][j] == map.mSize) {
						coords.add(x, y);
					}
				}
			}
		}
	}

	return coords;
}

bool isFull(MAP map) {
	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			if (map.value[x][y] == 0) {
				return false;
			}
		}
	}
	return true;
}

DATA calc(MAP map, int player, int cPlayer) {
	DATA data = calcValue(map, cPlayer, false);

	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			if (map.value[x][y] == 0) {
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

PICTURE convert(ASTR s) {
	PICTURE picture(s.size, s.str[0].length());

	for (int i = 0; i < s.size; i++) {
		for (int j = 0; j < s.str[0].length(); j++) {
			picture.symbols[i][j].text = s.str[i][j];
			if (s.str[i][j] != ' ') {
				picture.symbols[i][j].color = s.color;
			}
		}
	}

	return picture;
}

ASTR convertS(int color, initializer_list<string> sl) {
	string * s = new string [sl.size()];
	int i = 0;

	for (string si : sl) {
		s[i] = si;
		i++;
	}
	ASTR as(sl.size(), s, color);
	return as;
}

PICTURE null = convert(convertS(defColor, {
	{ "        "},	
	{ " ~~~~~~ "},
	{ " ~~~~~~ "},
	{ " ~~~~~~ "},
	{ " ~~~~~~ "},
	{ " ~~~~~~ "},
	{ "        "}
	}));

PICTURE cross = convert(convertS(Green, {
	{ "        "},
	{ " $$  $$ "},
	{ "  $$$$  "},
	{ "   $$   "},
	{ "  $$$$  "},
	{ " $$  $$ "},
	{ "        "}
	}));

PICTURE naught = convert(convertS(Red, {
	{ "        "},
	{ "  $$$$  "},
	{ " $$  $$ "},
	{ " $$  $$ "},
	{ " $$  $$ "},
	{ "  $$$$  "},
	{ "        "}
	}));

PICTURE square = convert(convertS(Blue, {
	{ "        "},
	{ " $$$$$$ "},
	{ " $$  $$ "},
	{ " $$  $$ "},
	{ " $$  $$ "},
	{ " $$$$$$ "},
	{ "        "}
	}));

PICTURE triangle = convert(convertS(Yellow, {
	{ "        "},
	{ "   $$   "},
	{ "   $$   "},
	{ "  $$$$  "},
	{ "  $$$$  "},
	{ " $$$$$$ "},
	{ "        "}
	}));

PICTURE makePicture(APICTURE p) {
	PICTURE picture(p.height * p.pictures[0][0].height, p.width * p.pictures[0][0].width);

	for (int x = 0; x < picture.height; x++) {
		for (int y = 0; y < picture.width; y++) {
			picture.symbols[x][y] = p.pictures[x / p.pictures[0][0].height][y / p.pictures[0][0].width].symbols[x - int(x / p.pictures[0][0].height) * p.pictures[0][0].height][y - int(y / p.pictures[0][0].width) * p.pictures[0][0].width];
		}
	}

	return picture;
}

APICTURE makeAPicture(MAP map) {
	APICTURE p(map.mSize, map.mSize);

	for (int x = 0; x < map.mSize; x++) {
		for (int y = 0; y < map.mSize; y++) {
			if (map.value[x][y] == 0) {
				p.pictures[x][y] = null;
			}
			if (map.value[x][y] == 1) {
				p.pictures[x][y] = cross;
			}
			if (map.value[x][y] == 2) {
				p.pictures[x][y] = naught;
			}
			if (map.value[x][y] == 3) {
				p.pictures[x][y] = square;
			}
			if (map.value[x][y] == 4) {
				p.pictures[x][y] = triangle;
			}
		}
	}
	return p;
}

PICTURE welcome = convert(convertS(Blue,  {
	{ "                     $$   $ $$$$$  $$      $$$$   $$$$  $$   $$ $$$$$                      "},
	{ "                     $$   $ $$     $$     $$  $$ $$  $$ $$$ $$$ $$                         "},
	{ "                     $$ $ $ $$$$   $$     $$     $$  $$ $$ $ $$ $$$$                       "},
	{ "                     $$$$$$ $$     $$     $$  $$ $$  $$ $$   $$ $$                         "},
	{ "                      $$ $$ $$$$$  $$$$$$  $$$$   $$$$  $$   $$ $$$$$                      "},
	{ "                                                                                           "},
	{ "              $$$$$$ $$$$$$  $$$$  $$$$$$  $$$$   $$$$  $$$$$$  $$$$  $$$$$                "},
	{ "                $$     $$   $$  $$   $$   $$  $$ $$  $$   $$   $$  $$ $$                   "},
	{ "                $$     $$   $$       $$   $$$$$$ $$       $$   $$  $$ $$$$                 "},
	{ "                $$     $$   $$  $$   $$   $$  $$ $$  $$   $$   $$  $$ $$                   "},
	{ "                $$   $$$$$$  $$$$    $$   $$  $$  $$$$    $$    $$$$  $$$$$                "},
	{ "                                                                                           "},
	{ "$$$$$  $$  $$    $$  $$ $$      $$$$  $$$$$  $$$$$$  $$$$  $$      $$$$  $$  $$   $$$$$    "},
	{ "$$  $$  $$$$     $$  $$ $$     $$  $$ $$  $$   $$   $$     $$     $$  $$ $$  $$   $$  $$   "},
	{ "$$$$$    $$      $$  $$ $$     $$$$$$ $$  $$   $$    $$$$  $$     $$$$$$ $$  $$   $$$$$    "},
	{ "$$  $$   $$       $$$$  $$     $$  $$ $$  $$   $$       $$ $$     $$  $$  $$$$    $$  $$   "},
	{ "$$$$$    $$        $$   $$$$$$ $$  $$ $$$$$  $$$$$$  $$$$  $$$$$$ $$  $$   $$     $$$$$  $$"}
	}));

bool isWin(ACOOR coords, int mSize) {
	if (coords.pos == mSize - 1) {
		return true;
	}
	return false;
}

COOR getMinCoord(ACOOR coords, int mSize) {
	int minX = mSize, minY = mSize;
	int cX = 0, cY = 0, iX, iY;

	for (int i = 0; i < coords.size; i++) {
		if (coords.data[i].x == minX) {
			cX++;
		}
		if (coords.data[i].y == minY) {
			cY++;
		}
		if (coords.data[i].x < minX) {
			iX = i;
			minX = coords.data[i].x;
			cX = 1;
		}
		if (coords.data[i].y < minY) {
			iY = i;
			minY = coords.data[i].y;
			cY = 1;
		}
	}

	if (cX == 1) {
		return coords.data[iX];
	}
	if (cY == 1) {
		return coords.data[iY];
	}
}

COOR getMaxCoord(ACOOR coords, int mSize) {
	int maxX = -1, maxY = -1;
	int cX = 0, cY = 0, iX, iY;

	for (int i = 0; i < coords.size; i++) {
		if (coords.data[i].x == maxX) {
			cX++;
		}
		if (coords.data[i].y == maxY) {
			cY++;
		}
		if (coords.data[i].x > maxX) {
			iX = i;
			maxX = coords.data[i].x;
			cX = 1;
		}
		if (coords.data[i].y > maxY) {
			iY = i;
			maxY = coords.data[i].y;
			cY = 1;
		}
	}

	if (cX == 1) {
		return coords.data[iX];
	}
	if (cY == 1) {
		return coords.data[iY];
	}
}

PICTURE drawLine(PICTURE picture, COOR a, COOR b, int size) {
	int k = d;
	for (int i = 0; i <= k; i++) {
		for (int dx = -1 * size; dx <= size; dx++) {
			
			int tY = a.y + (i * (b.y - a.y) / k);
			int tX = a.x + (i * (b.x - a.x) / k) + dx;
			
			picture.symbols[tX][tY].color = Orange;
			picture.symbols[tX][tY].text = '&';
		}
	}

	return picture;
}

int main() {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	printPicture(welcome, true);

	MAP map(3);

	for (int i = 0; i < map.mSize; i++) {
		for (int j = 0; j < map.mSize; j++) {
			cin >> map.value[i][j];
		}
	}

	//test
	//cout << "Result: x = " << getRandomCoord(getMaxData(calc(map, 1, 2))).x << " y = " << getRandomCoord(getMaxData(calc(map, 1, 2))).y << endl;

	ACOOR coords = analyzeGame(map, 4);
	if (isWin(coords, map.mSize)) {
		//cout << getMinCoord(coords, map.mSize).x << " " << getMinCoord(coords, map.mSize).y << " " << getMaxCoord(coords, map.mSize).x << " " << getMaxCoord(coords, map.mSize).y << endl;

		COOR a = getMinCoord(coords, map.mSize);
		COOR b = getMaxCoord(coords, map.mSize);

		//cout << a.x << " " << a.y << " " << b.x << " " << b.y << endl;

		a.x = a.x * defHeight + defHeight / 2;
		a.y = a.y * defWidth + defWidth / 2;

		b.x = b.x * defHeight + defHeight / 2;
		b.y = b.y * defWidth + defWidth / 2;

		//cout << a.x << " " << a.y << " " << b.x << " " << b.y << endl;

		printPicture(drawLine(makePicture(makeAPicture(map)), a, b, 1), true);
	}
	else {
		printPicture(makePicture(makeAPicture(map)), true);
	}

	//cout << coords.pos << endl;

	return 0;
}

