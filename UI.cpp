#include "Graphics.cpp"

class UI :public Graph{
private:
	void SetGraph(int graph) {
		Graph::setGraph(graph);
	}
public:
	/*x, y, テンキー*/
	int x, y, number;
	/*判定の幅の加算*/
	int correction;
	/*タッチID*/
	int ID;
	/*SE*/
	int sound;
	/*テキスト関係*/
	char Text[50] = "";
	int color;
	UI(){
		x = y = number = ID = -1;
		correction = sound = color = 0;
	}
	/*
	SetGraph(graph);
	x = _x; y = _y;
	number = _number;
	*/
	void SetUI(int graph, int _x, int _y, int _number = 5) {
		SetGraph(graph);
		SetPos(_x, _y, _number);
	}
	/*
	x = _x; y = _y;
	number = _number;
	*/
	void SetPos(int _x, int _y, int _number = 5) {
		x = _x; y = _y;
		number = _number;
	}

	void SetID(int id) {
		ID = id;
	}
	/*初期値 correction = 0*/
	void SetCorrection(int _correction) {
		correction = _correction;
	}
	void SetSE(int _sound) {
		sound = _sound;
	}
	void SetText(char text[], int _color = GetColor(255, 255, 255)) {
		strcpy_s(Text, text);
		color = _color;
	}
	/*Graph::Draw(x, y, number);*/
	void Draw() {
		Graph::Draw(x, y, number);
	}
	/*文字有*/
	void Draw(MyDrawString str, int size, int _number = 5) {
		Graph::Draw(x, y, number);
		str.Draw_String(x, y, size, color, Text, _number);
	}
};