#pragma once
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
	int graphID = -1;
public:
	int sizeX = 0, sizeY = 0;
	Graph() {
		graphID = NULL;
	}
	Graph(Display _display) {
		setDisplay(_display);
	}
	/*
	Graph(LoadGraph("\\00.png"))
	*/
	Graph(int _graphID) {
		setGraph(_graphID);
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
	void Draw(int x, int y, int number = 5) {
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
	int d = 10;
	DrawExtendGraph(x1, y1 - d, x2, y2 + d, graphID, TRUE);
	*/
	void Draw_LinkLine(int x1, int y1, int x2, int y2, double time, double dt) {
		if (time < dt) dt = time;
		int dy = (int)(((double)15.0 / sqrt(time)) * sqrt(dt) + 0.5);
		DrawExtendGraph(x1, y1 - dy, x2, y2 + dy, graphID, TRUE);
	}
	/*
	DrawLine(x1,y1,x2,y2, GetColor(255,255,255));
	*/
	void Draw_LongLine(int x1, int y1, int x2, int y2, double time, double dt) {
		if (time < dt) dt = time;
		int dx = (int)(((double)30.0 / sqrt(time)) * sqrt(dt) + 0.5);
		for (int i = -(dx / 2); i <= dx / 2; i++)
			DrawLine(x1 + i, y1, x2 + i, y2, GetColor(255, 255, 255));
	}
	/*
	DrawLine(x1,y1,x2,y2, GetColor(255,255,255));
	*/
	void Draw_FlickLine(int x1, int y1, int x2, int y2, double time, double dt) {
		if (time < dt) dt = time;
		int dy = (int)(((double)30.0 / sqrt(time)) * sqrt(dt) + 0.5);
		for (int i = -(dy / 2); i <= dy / 2; i++)
			DrawLine(x1, y1 + i, x2, y2 + i, GetColor(255, 255, 255));
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
		type == 0 PERFECT *1.2
		type == 1 GREAT *0.8
		type == 2 NICE *0.6
		DrawExtendGraph((int)(x - dx / 2.0), (int)(y - dy / 2.0),
			(int)(x + dx / 2.0), (int)(y + dy / 2.0), graphID, TRUE);
	*/
	void DrawHitEffect(int type, int x, int y, double time, double dt) {
		if (time < dt) dt = time;
		//type == 0 PERFECT
		double dx = ((double)(sizeX * 1.2) / time) * dt;
		double dy = ((double)(sizeY * 1.2) / time) * dt;
		//GREAT
		if (type == 1) {
			dx = ((double)(sizeX * 0.8) / time) * dt;
			dy = ((double)(sizeY * 0.8) / time) * dt;
		}
		//NICE
		else if (type == 2) {
			dx = ((double)(sizeX * 0.6) / time) * dt;
			dy = ((double)(sizeY * 0.6) / time) * dt;
		}
		DrawExtendGraph((int)(x - dx / 2.0), (int)(y - dy / 2.0),
			(int)(x + dx / 2.0), (int)(y + dy / 2.0), graphID, TRUE);
	}
	/*BackGround専用*/
	void Draw_BackGround() {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
		DrawExtendGraph(0, 0, display.GetScreenX(), display.GetScreenY(), graphID, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};

class MyDrawString{
private:
	int mainFontSize;
public:
	MyDrawString() {
		mainFontSize = 16;
	}
	/*mainFontSize = size;*/
	void SetMainFontSize(int size) {
		mainFontSize = size;
	}
	/*初期値　GetColor(255,255,255)*/
	void Draw_String(int x, int y, int fontsize, char str[], int number = 5) {//flip無し
		Draw_String(x, y, fontsize, GetColor(255,255,255), str, number);
	}
	/*色指定追加*/
	void Draw_String(int x, int y, int fontsize, int _color,  char str[], int number = 5) {//flip無し
		SetFontSize(fontsize);
		int StrWidth = GetDrawStringWidth(str, strlen(str));

		if (number == 1)
			DrawString(x, y, str, _color);
		else if (number == 2)
			DrawString(x - StrWidth / 2, y, str, _color);
		else if (number == 3)
			DrawString(x - StrWidth, y, str, _color);
		else if (number == 4)
			DrawString(x, y - fontsize / 2, str, _color);
		else if (number == 5)
			DrawString(x - StrWidth / 2, y - fontsize / 2, str, _color);
		else if (number == 6)
			DrawString(x - StrWidth, y - fontsize / 2, str, _color);
		else if (number == 7)
			DrawString(x, y - fontsize, str, _color);
		else if (number == 8)
			DrawString(x - StrWidth / 2, y - fontsize, str, _color);
		else if (number == 9)
			DrawString(x - StrWidth, y - fontsize, str, _color);

		SetFontSize(mainFontSize);
	}
};