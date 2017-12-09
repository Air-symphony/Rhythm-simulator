#include "Graphics.cpp"

class UI :public Graph{
private:
	void SetGraph(int graph) {
		Graph::setGraph(graph);
	}
	void SetPos(int _x, int _y) {
		x = _x; y = _y;
	}
public:
	/*x, y, テンキー*/
	int x, y, number;
	/*タッチID*/
	int ID = -1;
	char Text[50] = {""};
	int color = 0;
	UI(){
		x = y = number = -1;
	}
	/*
	SetGraph(graph);
	SetPos(_x, _y);
	number = _number;
	*/
	void SetUI(int graph, int _x, int _y, int _number = 5) {
		SetGraph(graph);
		SetPos(_x, _y);
		number = _number;
	}

	void SetID(int id) {
		ID = id;
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