/*Noteデータ
int id, type; 1,2,3,4
bool flag; //true = 表示、false = 非表示
int first_x, end_x; 1,2,3,4,5
int y; 描画用
int rhythm_count; 何分音符1,4,8,16,32
int bar_number 何小節目
double time; 処理される時間(バーに来る時間)
*/
class Note {
private:
	int id, type;
	bool flag;
	int first_x, end_x;
	int y;
	int rhythm_count;
	int bar_number;
	double time;
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
	void setrhythm_count(int _rhythm_count) {
		rhythm_count = _rhythm_count;
	}
	void settime(double _time) {
		time = _time;
	}
	/*
	int id, int noteの種類
	*/
	void setNote(int _id, int _type) {
		setID(id); setType(type);
	}
	/*
	int id, int noteの種類, int 出だし位置, int 終わり位置, double 処理される時間(バーに来る時間)
	setflag(true);
	*/
	void setNote(int id, int type, int _first_x, int _end_x, double _time) {
		setID(id); setType(type); setX(_first_x, _end_x); settime(_time);
		setflag(true);
	}
	/*
	int id, int noteの種類, int 出だし位置, int 終わり位置, int 何小節目, int 何分音符
	setflag(true);
	*/
	void setNote(int id, int type, int _first_x, int _end_x, int _bar_number, int _rhythm_count) {
		setID(id); setType(type); setX(_first_x, _end_x); setbar_number(_bar_number); setrhythm_count(_rhythm_count);
		setflag(true);
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
	double gettime() {
		return time;
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
		y += speed;
	}
};

/*
BPMから拍を計算する
double spb = 1.0 / (bpm / 60.0)
一小節、四分、八分...音符
double rh1_1, rh1_4, rh1_8, rh1_16, rh1_32;
Rhythm(float bpm)
*/
class Rhythm {
private:
	/*1拍当たりの時間(s)*/
	double spb = 0;
	double rh1_1, rh1_4, rh1_8, rh1_16, rh1_32;
public:
	Rhythm() {
		spb = rh1_1 = rh1_4 = rh1_8 = rh1_16 = 0;
	}
	void SetRhythm(float bpm) {
		spb = (double)(1.0 / (bpm / 60.0));
		rh1_1 = (double)(spb * 4);
		rh1_4 = (double)(spb);
		rh1_8 = (double)(spb / 2.0);
		rh1_16 = (double)(spb / 4.0);
		rh1_32 = (double)(spb / 8.0);
	}
	/*不要*/
	/*void SetRhythm(float bpm, int speed) {
		spb = 1.0 / (bpm / 60.0);
	}*/

	/*
	return 0.00秒
	count = 何分音符1,4,8,16,32
	error = -1*/
	double getRhythm(int count) {
		if (count == 1) {
			return rh1_1;
		}
		else if (count == 4) {
			return rh1_4;
		}
		else if (count == 8) {
			return rh1_8;
		}
		else if (count == 16) {
			return rh1_16;
		}
		else if (count == 32) {
			return rh1_32;
		}
		return -1;
	}
};