#include "DxLib.h"
#include <Math.h>

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
	Display display;
	int graphID;
	int sizeX, sizeY;
public:
	Graph() {
		graphID = NULL;
	}
	Graph(Display _display) {
		display =  _display;
	}
	/*
	Graph(LoadGraph("\\00.png"))
	*/
	Graph(int _graphID) {
		graphID = _graphID;
		GetGraphSize(graphID, &sizeX, &sizeY);
	}
	/*displayの大きさ*/
	void setDisplay(Display _display) {
		display = _display;
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
	/*
	DrawLine(x1,y1,x2,y2, GetColor(255,255,255));
	*/
	void Draw_LinkLine(int x1, int y1, int x2, int y2) {
		DrawLine(x1, y1, x2, y2, GetColor(255, 255, 255));
	}
	/*
	画像サイズ変更
	_time = 時間制限、dt=今の時間
	if (time < dt) dt = time;
	double dx = ((double)sizeX / sqrt(time)) * sqrt(dt);
	double dy = ((double)sizeY / sqrt(time)) * sqrt(dt);
	DrawExtendGraph(x - dx / 2.0, y - dy / 2.0, 
		x + dx / 2.0, y + dy / 2.0, graphID, TRUE);
	*/
	void DrawNote(int x, int y, double time, double dt) {
		if (time < dt) dt = time;
		double dx = ((double)sizeX / sqrt(time)) * sqrt(dt);
		double dy = ((double)sizeY / sqrt(time)) * sqrt(dt);
		DrawExtendGraph((int)(x - dx / 2.0), (int)(y - dy / 2.0),
			(int)(x + dx / 2.0), (int)(y + dy / 2.0), graphID, TRUE);
	}
	/*_time = 時間制限、dt=今の時間
	if (time < dt) dt = time;
		double dx = ((double)sizeX / time) * dt;
		double dy = ((double)sizeY / time) * dt;
		DrawExtendGraph(x - dx / 2.0, y - dy / 2.0,
			x + dx / 2.0, y + dy / 2.0, graphID, TRUE);*/
	void DrawHitEffect(int x, int y, double time, double dt) {
		if (time < dt) dt = time;
		double dx = ((double)sizeX / time) * dt;
		double dy = ((double)sizeY / time) * dt;
		DrawExtendGraph((int)(x - dx / 2.0), (int)(y - dy / 2.0),
			(int)(x + dx / 2.0), (int)(y + dy / 2.0), graphID, TRUE);
	}
};