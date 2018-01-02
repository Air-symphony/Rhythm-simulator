#include "UI.cpp"

/*
int shadowDist = 5;
Graph shadow;
*/
class ButtonUI :public UI {
private:
	int shadowDist = 5;
	Graph shadow;
public:
	ButtonUI() {
		UI();
	}
	/*
	SetGraph(graph);
	shadow.setGraph(shadowGraph);
	x = _x; y = _y;
	number = _number;
	*/
	void SetUI(int graph, int shadowGraph, int _x, int _y, int _number = 5) {
		setGraph(graph);
		shadow.setGraph(shadowGraph);
		SetPos(_x, _y, _number);
	}
	/*
	if (push) Graph::Draw(x, y, number);
	else {
		shadow.Draw(x, y, number);
		Graph::Draw(x, y - shadowDist, number);
	}
	*/
	void Draw(bool push = true) {
		if (push) Graph::Draw(x, y, number);
		else {
			shadow.Draw(x, y, number);
			Graph::Draw(x, y - shadowDist, number);
		}
	}
	/*•¶Žš—L*/
	void Draw(bool push, MyDrawString str, int size, int _number = 5) {
		if (push) {
			Graph::Draw(x, y, number);
			str.Draw_String(x, y, size, color, Text, _number);
		}
		else {
			shadow.Draw(x, y, number);
			Graph::Draw(x, y - shadowDist, number);
			str.Draw_String(x, y - shadowDist, size, color, Text, _number);
		}
	}
};