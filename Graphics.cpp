#include "DxLib.h"

/*screen = 50
ratio[2] = {16, 9}
size_x = ratio[0] * screen;
size_y = ratio[1] * screen;
*/
class Display {
private:
	int screen = 50;
	int ratio[2] = {16, 9};
	int size_x, size_y;
public:
	Display(int screen) {
		size_x = ratio[0] * screen;
		size_y = ratio[1] * screen;
	}
	Display() {
		size_x = ratio[0] * screen;
		size_y = ratio[1] * screen;
	}
	int GetScreenX() {
		return size_x;
	}
	int GetScreenY() {
		return size_y;
	}
};

/*
画像表示関係
int graphID;
int sizeX, sizeY;
*/
class Graph {
private:
	int graphID;
	int sizeX, sizeY;
public:
	Graph() {
		graphID = NULL;
	}
	/*
	Graph(LoadGraph("\\00.png"))
	*/
	Graph(int _graphID) {
		graphID = _graphID;
		GetGraphSize(graphID, &sizeX, &sizeY);
	}
	/*
	setGraph(LoadGraph("\\00.png"))
	*/
	void setGraph(int graph) {
		graphID = graph;
		GetGraphSize(graphID, &sizeX, &sizeY);
	}
	/*
	x, y = 画像を表示させたい座標
	number = 特定の位置を中心として表示(テンキー)
	*/
	void Draw(int x, int y, int number) {
		if (number == 1)
			DrawGraph(x, y, graphID, TRUE);
		else if (number == 2)
			DrawGraph(x - sizeX / 2, y, graphID, TRUE);
		else if (number == 3)
			DrawGraph(x - sizeX, y, graphID, TRUE);
		else if (number == 4)
			DrawGraph(x, y - sizeY / 2, graphID, TRUE);
		else if (number == 5)
			DrawGraph(x - sizeX / 2, y - sizeY / 2, graphID, TRUE);
		else if (number == 6)
			DrawGraph(x - sizeX, y - sizeY / 2, graphID, TRUE);
		else if (number == 7)
			DrawGraph(x, y - sizeY, graphID, TRUE);
		else if (number == 8)
			DrawGraph(x - sizeX / 2, y - sizeY, graphID, TRUE);
		else if (number == 9)
			DrawGraph(x - sizeX, y - sizeY, graphID, TRUE);
	}
};