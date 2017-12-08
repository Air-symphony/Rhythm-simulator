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
	int ID;
	UI(){

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
	/*Graph::Draw(x, y, number);*/
	void Draw() {
		Graph::Draw(x, y, number);
	}
};