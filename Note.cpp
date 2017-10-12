#include <Math.h>
/*Noteデータ
int id, type; 1,2,3,4
int flag; 0=未消化、1=表示、-1=消化済
int first_x, end_x; 1,2,3,4,5
int x, y; 描画用
int rhythm_count; 何分音符1,4,8,16,32
int bar_number 何小節目
int longNoteID; ロングノーツの連結に使用
int linkNoteID; フリックの連結に使用
int sideNoteID; 同時押し用
double time; 処理される時間(バーに来る時間)
*/
class Note {
private:
	int id, type;
	int flag;//0=未消化、1=表示、-1=消化済
	int first_x, end_x;
	int x, y;
	int rhythm_count;
	int bar_number;
	int longNoteID;
	int linkNoteID;
	int sideNoteID;
	double time;
public:
	/*id = type = first_x = end_x = bar_number = -1;
		y = 0;*/
	Note() {
		id = type = first_x = end_x = bar_number = longNoteID = linkNoteID = sideNoteID = -1;
		x = y = 0; time = 0.0; flag = -1;
	}
	/*id = _id;
		type = first_x = end_x = bar_number = -1;
		y = 0;*/
	Note(int _id) {
		id = _id;
		type = first_x = end_x = bar_number = -1;
		x = y = 0; time = 0.0; flag = false;
	}
	void setID(int _id) {
		id = _id;
	}
	void setType(int _type) {
		type = _type;
	}
	/*1,2,3,4,5*/
	void setend_x(int _end_x) {
		end_x = _end_x;
	}
	/*1,2,3,4,5*/
	void setfirst_x(int _first_x) {
		first_x = _first_x;
	}
	/*1,2,3,4,5*/
	void setX(int _first_x, int _end_x) {
		setend_x(_end_x);
		setfirst_x(_first_x);
	}
	void setx(int _x) {
		x = _x;
	}
	void setbar_number(int _bar_number) {
		bar_number = _bar_number;
	}
	void setlongNoteID(int _id) {
		longNoteID = _id;
	}
	void setlinkNoteID(int _id) {
		linkNoteID = _id;
	}
	void setsideNoteID(int _id) {
		sideNoteID = _id;
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
	setflag() 0=未消化、1=表示、-1=消化済;
	*/
	void setNote(int id, int type, int _first_x, int _end_x, double _time) {
		setID(id); setType(type); setX(_first_x, _end_x); settime(_time);
		setflag(0);
	}
	/*
	int id, int noteの種類, int 出だし位置, int 終わり位置, int 何小節目, int 何分音符
	setflag(0) 0=未消化、1=表示、-1=消化済;
	*/
	/*void setNote(int id, int type, int _first_x, int _end_x, int _bar_number, int _rhythm_count) {
		setID(id); setType(type); setX(_first_x, _end_x); setbar_number(_bar_number); setrhythm_count(_rhythm_count);
		setflag(0);
	}*/
	/*0=未消化、1=表示、-1=消化済*/
	void setflag(int _flag) {
		flag = _flag;
	}
	int getID() {
		return id;
	}
	/*1,2,3,4*/
	int getType() {
		return type;
	}
	/*1,2,3,4,5*/
	int getend_x() {
		return end_x;
	}
	/*1,2,3,4,5*/
	int getfirst_x() {
		return first_x;
	}
	int getbar_number() {
		return bar_number;
	}
	double gettime() {
		return time;
	}
	/*0=未消化、1=表示、-1=消化済*/
	int getflag() {
		return flag;
	}
	void setY(int _y) {
		y = _y;
	}
	int getY() {
		return y;
	}
	int getX() {
		return x;
	}
	int getlongNoteID() {
		return longNoteID;
	}
	int getlinkNoteID() {
		return linkNoteID;
	}
	int getsideNoteID() {
		return sideNoteID;
	}
	/*
	_posX, _posY = 判定座標
	_dt = 経過時間
	_time = 移動時間
	double dx = (sqrt(_dt) * (end_x - first_x) / sqrt(_time));
		x = (int)((first_x + dx) * (_posX));
		y = (int)((_posY / _time) * _dt);
	*/
	void ToMove(double _posX, double _posY, double _dt, double _time) {
		double dx = sqrt(_dt) * (end_x - first_x) / sqrt(_time);
		x = (int)((first_x + dx) * _posX);
		y = (int)((_posY / _time) * _dt);
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
	/*bpmが変更された時のbar_numberの保存*/
	int d_bar_number = 0;
	/*それまでのbpmで計算された時間の保存*/
	double dt = 0;
public:
	Rhythm() {
		spb = rh1_1 = rh1_4 = rh1_8 = rh1_16 = rh1_32 = dt = 0;
	}
	void SetRhythm(float bpm) {
		spb = (double)(1.0 / (bpm / 60.0));
		rh1_1 = (double)(spb * 4);
		rh1_4 = (double)(spb);
		rh1_8 = (double)(spb / 2.0);
		rh1_16 = (double)(spb / 4.0);
		rh1_32 = (double)(spb / 8.0);
	}
	/*
	bpmが変更された時のbar_numberの保存
	　d_barnumber = _bar_number - d_bar_number;
	それまでのbpmで計算された時間の保存
	　dt += (double)(d_bar_number) * rh1_1;
	SetRhythm(bpm);	
	*/
	void ChangeRhythm(float bpm, int _bar_number) {
		d_bar_number = _bar_number - d_bar_number;
		dt += (double)(d_bar_number) * rh1_1;
		SetRhythm(bpm);
	}
	/*
	return dt //今までのbpmでの時間
			+ (double)(_bar_number - d_bar_number) * getRhythm(1) //以前のbpmを除いたbar分、小節分の時間を加算
			+ (double)_timing * getRhythm(_rhythm_note);//その小節上での細かい時間
			*/
	double CalculateTime(int _bar_number, int _timing, int _rhythm_note) {
		return dt //今までのbpmでの時間
			+ (double)(_bar_number - d_bar_number) * getRhythm(1) //以前のbpmを除いたbar分、小節分の時間を加算
			+ (double)_timing * getRhythm(_rhythm_note);//その小節上での細かい時間
	}
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