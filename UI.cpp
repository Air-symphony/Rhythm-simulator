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
<<<<<<< HEAD
	int correction, sound;
=======
	int correction;
>>>>>>> f7a934b4ac19c660b5154db7798081ec4119fd86
	/*タッチID*/
	int ID = -1;
	char Text[50] = {""};
	int color = 0;
	UI(){
		x = y = number = -1;
		correction = 0;
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
	/*初期値 correction = 0*/
	void SetCorrection(int _correction) {
		correction = _correction;
	}
<<<<<<< HEAD
	void SetSE(int _sound) {
		sound = _sound;
	}
=======
>>>>>>> f7a934b4ac19c660b5154db7798081ec4119fd86
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