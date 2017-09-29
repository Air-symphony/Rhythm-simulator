/*Noteデータ
int id, type; 1,2,3,4
bool flag; //true = 表示、false = 非表示
int first_x, end_x; 1,2,3,4,5
int y; 描画用
int bar_number, time;//何小節目、表示開始の時間
*/
class Note {
private:
	int id, type;
	bool flag;
	int first_x, end_x;
	int y;
	int bar_number, time;
public:
	/*id = type = first_x = end_x = bar_number = -1;
		y = 0;*/
	Note() {
		id = type = first_x = end_x = bar_number = -1;
		y = time = 0; flag = false;
	}
	/*id = _id;
		type = first_x = end_x = bar_number = -1;
		y = 0;*/
	Note(int _id) {
		id = _id;
		type = first_x = end_x = bar_number = -1;
		y = time = 0; flag = false;
	}
	void setID(int _id) {
		id = _id;
	}
	void setType(int _type) {
		type = _type;
	}
	void setend_x(int _end_x) {
		end_x = _end_x;
	}
	void setfirst_x(int _first_x) {
		first_x = _first_x;
	}
	void setX(int _first_x, int _end_x) {
		setend_x(_end_x);
		setfirst_x(_first_x);
	}
	void setbar_number(int _bar_number) {
		bar_number = _bar_number;
	}
	/*
	int id, int noteの種類
	*/
	void setNote(int _id, int _type) {
		setID(id); setType(type);
	}
	/*
	int id, int noteの種類, int 出だし位置, int 終わり位置, int 何小節目
	*/
	void setNote(int id, int type, int _first_x, int _end_x, int _bar_number) {
		setID(id); setType(type); setX(_first_x, _end_x); setbar_number(_bar_number);
	}
	void setflag(bool _flag) {
		flag = _flag;
	}
	int getID() {
		return id;
	}
	int getType() {
		return type;
	}
	int getend_x() {
		return end_x;
	}
	int getfirst_x() {
		return first_x;
	}
	int getbar_number() {
		return bar_number;
	}
	bool getflag() {
		return flag;
	}
	void setY(int _y) {
		y = _y;
	}
	int getY() {
		return y;
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
	Rhythm() {
		default1_4 = rh1_1 = rh1_4 = rh1_8 = rh1_16 = 0;
	}
	void SetRhythm(float bpm) {
		double rh_per_time = 1.0 / (bpm / 60.0);
		default1_4 = rh_per_time * 60.0;
	}
	void SetRhythm(float bpm, int speed) {
		double rh_per_time = 1.0 / (bpm / 60.0);
		default1_4 = rh_per_time * 60.0;
		SetSpeed(speed);
	}
	void SetSpeed(int speed) {
		rh1_4 = default1_4 * (double)speed;
		rh1_8 = rh1_4 / 2.0;
		rh1_16 = rh1_4 / 4.0;
		rh1_1 = rh1_4 * 4.0;
	}
};