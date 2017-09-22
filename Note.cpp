/*Noteデータ
int id, type;//0,1,2,3,4
	bool flag;
	int first_x;//出てくる場所
	int x; // 1, 2, 3, 4, 5
	int y;
*/
class Note {
private:
	int id, type;
	bool flag;
	int first_x;
	int x, y;
public:
	Note() {
		id = type = x = -1;
	}
	Note(int _id) {
		id = _id;
	}
	void setID(int _id) {
		id = _id;
	}
	void setType(int _type) {
		type = _type;
	}
	void setX(int _x) {
		x = _x;
	}
	void setY(int _y) {
		y = _y;
	}
	void setXY(int x, int y) {
		setX(x); setY(y);
	}
	void setNote(int _id, int _type) {
		setID(id); setType(type);
	}
	void setNote(int id, int type, int x, int y) {
		setID(id); setType(type); setXY(x, y);
	}
	void Setflag(bool _flag) {
		flag = _flag;
	}
	void Move(int speed) {
		y -= speed;
	}
};

/*
BPMから拍を計算する
double default1_4 = 0;
double rh1_1, rh1_4, rh1_8, rh1_16;
Rhythm(int bpm, int speed)
*/
class Rhythm {
private:
	double default1_4 = 0;
	double rh1_1, rh1_4, rh1_8, rh1_16;
public:
	Rhythm(int bpm) {
		default1_4 = rh1_1 = rh1_4 = rh1_8 = rh1_16 = 0;
		double rh_per_time = 1.0 / (bpm / 60.0);
		default1_4 = rh_per_time * 60.0;
	}
	Rhythm(int bpm, int speed) {
		default1_4 = rh1_1 = rh1_4 = rh1_8 = rh1_16 = 0;
		double rh_per_time = 1.0 / (bpm / 60.0);
		default1_4 = rh_per_time * 60.0;
		setSpeed(speed);
	}
	void setSpeed(int speed) {
		rh1_4 = default1_4 * (double)speed;
		rh1_8 = rh1_4 / 2.0;
		rh1_16 = rh1_4 / 4.0;
		rh1_1 = rh1_4 * 4.0;
	}
};