#include <Math.h>
/*Note�f�[�^
int id, type; 1,2,3,4
int flag; 0=�������A1=�\���A-1=������
int first_x, end_x; 1,2,3,4,5
int x, y; �`��p
int rhythm_count; ��������1,4,8,16,32
int bar_number �����ߖ�
int longNoteID; �����O�m�[�c�̘A���Ɏg�p
int linkNoteID; �t���b�N�̘A���Ɏg�p
int sideNoteID; ���������p
double time; ��������鎞��(�o�[�ɗ��鎞��)
*/
class Note {
private:
	int id, type;
	int flag;//0=�������A1=�\���A-1=������
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
	int id, int note�̎��
	*/
	void setNote(int _id, int _type) {
		setID(id); setType(type);
	}
	/*
	int id, int note�̎��, int �o�����ʒu, int �I���ʒu, double ��������鎞��(�o�[�ɗ��鎞��)
	setflag() 0=�������A1=�\���A-1=������;
	*/
	void setNote(int id, int type, int _first_x, int _end_x, double _time) {
		setID(id); setType(type); setX(_first_x, _end_x); settime(_time);
		setflag(0);
	}
	/*
	int id, int note�̎��, int �o�����ʒu, int �I���ʒu, int �����ߖ�, int ��������
	setflag(0) 0=�������A1=�\���A-1=������;
	*/
	/*void setNote(int id, int type, int _first_x, int _end_x, int _bar_number, int _rhythm_count) {
		setID(id); setType(type); setX(_first_x, _end_x); setbar_number(_bar_number); setrhythm_count(_rhythm_count);
		setflag(0);
	}*/
	/*0=�������A1=�\���A-1=������*/
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
	/*0=�������A1=�\���A-1=������*/
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
	_posX, _posY = ������W
	_dt = �o�ߎ���
	_time = �ړ�����
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
BPM���甏���v�Z����
double spb = 1.0 / (bpm / 60.0)
�ꏬ�߁A�l���A����...����
double rh1_1, rh1_4, rh1_8, rh1_16, rh1_32;
Rhythm(float bpm)
*/
class Rhythm {
private:
	/*1��������̎���(s)*/
	double spb = 0;
	double rh1_1, rh1_4, rh1_8, rh1_16, rh1_32;
	/*bpm���ύX���ꂽ����bar_number�̕ۑ�*/
	int d_bar_number = 0;
	/*����܂ł�bpm�Ōv�Z���ꂽ���Ԃ̕ۑ�*/
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
	bpm���ύX���ꂽ����bar_number�̕ۑ�
	�@d_barnumber = _bar_number - d_bar_number;
	����܂ł�bpm�Ōv�Z���ꂽ���Ԃ̕ۑ�
	�@dt += (double)(d_bar_number) * rh1_1;
	SetRhythm(bpm);	
	*/
	void ChangeRhythm(float bpm, int _bar_number) {
		d_bar_number = _bar_number - d_bar_number;
		dt += (double)(d_bar_number) * rh1_1;
		SetRhythm(bpm);
	}
	/*
	return dt //���܂ł�bpm�ł̎���
			+ (double)(_bar_number - d_bar_number) * getRhythm(1) //�ȑO��bpm��������bar���A���ߕ��̎��Ԃ����Z
			+ (double)_timing * getRhythm(_rhythm_note);//���̏��ߏ�łׂ̍�������
			*/
	double CalculateTime(int _bar_number, int _timing, int _rhythm_note) {
		return dt //���܂ł�bpm�ł̎���
			+ (double)(_bar_number - d_bar_number) * getRhythm(1) //�ȑO��bpm��������bar���A���ߕ��̎��Ԃ����Z
			+ (double)_timing * getRhythm(_rhythm_note);//���̏��ߏ�łׂ̍�������
	}
	/*
	return 0.00�b
	count = ��������1,4,8,16,32
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