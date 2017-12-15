#include "HitEffect.cpp"
#include "Note.cpp"

/*�y�ȃf�[�^
BGM,BPM,Note�Ȃǂ��Ǘ�
char title[256];
float bpm;
int soundHandle;
int offset;
int notecount;
Note notes[1000];
Rhythm rhythm;
*/
class Music {
public:
	char title[256] = "";
	float bpm;
	int soundHandle;
	int backgroundHandle;
	int offset;//(ms)
	int notecount;
	Note notes[1000];
	Rhythm rhythm;

	Music() {
		soundHandle = offset = notecount = 0;
		bpm = 0.0f;
	}
	void SetPos(double sideSize, double _posX, double _posY) {
		for (int i = 0; i < notecount; i++) {
			notes[i].setx((int)(sideSize + (notes[i].getfirst_x() - 1) * _posX));
			notes[i].setY((int)(_posY));
		}
	}
};

/*�y�ȃf�[�^���X�g
id�ɑΉ�����.txt��ǂݍ��݁A���ʂ�y�ȃf�[�^��ԋp����
Music music;
char filepath[256] = "materials\\";
int FileHandle;
char *next;
char *ctx;//�������p
*/
class NoteFileReader {
	bool debugMode = false;
private:
	Music music;
	char filepath[20] = "materials\\";
	int FileHandle = 0;
	int fileSize = 0;
	char *next;
	char *ctx;//�������p
public:
	NoteFileReader() {

	}
	/*NoteFile�̓ǂݍ��݁ANote�̏���*/
	Music SelectReadFile(char filename[], bool _debugMode) {
		debugMode = _debugMode;
		try {
			char _filepath[256];
			strcpy_s(_filepath, filepath);
			strcat_s(_filepath, "NoteFile\\");
			strcat_s(_filepath, filename);
			strcat_s(_filepath, ".txt");
			fileSize = (int)FileRead_size(_filepath);
			FileHandle = FileRead_open(_filepath);
			if (FileHandle == 0) {
				char error[256];
				strcpy_s(error, _filepath);
				strcat_s(error, " is not found.");
				throw error;
			}
			else {
				DrawLoading();
				SetConfig();
				SetNote();
			}
		}
		catch (char* text) {
			ClearDrawScreen();
			clsDx();
			printfDx(text);
			ScreenFlip();
			WaitKey();
		}
		return music;
	}
	//���ݒ�ABPM��
	void SetConfig() {
		char string[256] = "";
		char errorMessage[512] = "";
		bool error = false;
		while (FileRead_eof(FileHandle) == 0)
		{
			DrawLoading();
			// ��s�ǂݍ���
			FileRead_gets(string, 256, FileHandle);
			//�擪���番�����Ă����@a,b,c = a + b,c
			next = strtok_s(string, " ", &ctx);
			//������̔�r
			if (strcmp(next, "#Title") == 0) {
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(music.title, 256, "%s", next);
			}
			else if (strcmp(next, "#BackGround") == 0) {
				char background_name[256];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(background_name, 256, "%s", next);
				char _filepath[256];
				strcpy_s(_filepath, filepath);
				strcat_s(_filepath, "Image\\background\\");
				strcat_s(_filepath, background_name);
				music.backgroundHandle = LoadGraph(_filepath);
				if (music.backgroundHandle == -1) {
					error = true;
					strcat_s(errorMessage, _filepath);
					strcat_s(errorMessage, " can not find.\n");
				}
			}
			else if (strcmp(next, "#Song") == 0) {
				char sound_name[256];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(sound_name, 256, "%s", next);
				char _filepath[256];
				strcpy_s(_filepath, filepath);
				strcat_s(_filepath, "Music\\");
				strcat_s(_filepath, sound_name);
				music.soundHandle = LoadSoundMem(_filepath);
				if (music.soundHandle == -1) {
					error = true;
					strcat_s(errorMessage, _filepath);
					strcat_s(errorMessage, " can not find.\n");
				}
			}
			else if (strcmp(next, "#BPM") == 0) {
				next = strtok_s(NULL, "", &ctx);
				char char_bpm[256];
				sprintf_s(char_bpm, 256, "%s", next);
				music.bpm = (float)atof(char_bpm);
				music.rhythm.SetRhythm(music.bpm);
			}
			else if (strcmp(next, "#Offset") == 0) {
				int start = 0, count = 4;
				next = strtok_s(NULL, "", &ctx);

				bool minus = false;
				/*next���Ɋ܂܂�邩�ǂ���*/
				if (strstr(next, "-") != NULL || strstr(next, "+") != NULL) {
					minus = (strstr(next, "-") != NULL);
					start++;
					count++;
				}
				for (int i = start; i < count; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < ((count - 1) - i); j++) {
						c *= 10;
					}
					music.offset += c;
				}
				if (minus) music.offset *= -1;
			}
			else if (strcmp(next, "#end") == 0) {
				if (error) {
					ClearDrawScreen();
					printfDx(errorMessage);
					ScreenFlip();
					WaitKey();
					clsDx();
				}
				DrawLoading();
				break;
			}
		}
	}

	void SetNote() {
		const int SIZE = 64;
		int noteID = 0;//�m�[�c�ɔԍ�������U��
		/*�f�o�b�O�p*/
		int readline = 0;
		
		DrawLoading();
		while (FileRead_eof(FileHandle) == 0) {
			DrawLoading();
			/*�ǂݍ��ݎ��̉��ۑ��p*/
			int _type[SIZE], _timing[SIZE], _first_x[SIZE], _end_x[SIZE];
			/*�����ߖڂ�*/
			int bar_number = 0;
			/*���̐߂̃m�[�c��*/
			int _notecount = 0;
			/*���̏��߂������q��*/
			int rhythm_note = 0;
			/*�E�u0�v���u1�v�̎��@�c�@�t���b�N�����̐�ɂ���t���b�N�m�[�g�Ɛڑ�
			�E�u2�v���u3�v�̎��@�c�@�t���b�N�����Ɋ֌W�Ȃ��ڑ�
			�i�������� <����p�F0,2> <�E��p�F1,3> �ł��B�j*/
			int channel = -1;

			/*�f�o�b�O�p*/
			readline++;

			char string[256];
			FileRead_gets(string, 256, FileHandle);
			next = strtok_s(string, ",", &ctx);
			/*������̔�r*/
			if (strcmp(next, "#0") == 0 || strcmp(next, "#1") == 0 ||
				strcmp(next, "#2") == 0 || strcmp(next, "#3") == 0) {
				channel = (int)(next[1] - '0');

				next = strtok_s(NULL, ":", &ctx);//�����ߖ�
				for (int i = 0; i < 3; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < (2 - i); j++) {
						c *= 10;
					}
					bar_number += c;
				}

				next = strtok_s(NULL, ":", &ctx);//rhythm & type note
				char _char[SIZE];
				sprintf_s(_char, SIZE, "%s", next);
				for (int i = 0; i < SIZE; i++) {
					/*�󕶎��ł���Γǂݍ��ݏI��*/
					if (strcmp(&_char[i], "") == 0) {
						rhythm_note = i;
						break;
					}
					/*�����R�[�h���琔�����o�� "0" = 0*/
					int _note = (int)(_char[i] - '0');
					if (_note > 0) {
						_timing[_notecount] = i;
						_type[_notecount] = _note;
						_notecount++;
					}
				}
				next = strtok_s(NULL, ":", &ctx);//first_x note
				sprintf_s(_char, SIZE, "%s", next);
				for (int i = 0; i < SIZE; i++) {
					if (strcmp(&_char[i], "") == 0) {
						break;
					}
					int _note = (int)(_char[i] - '0');
					_first_x[i] = _end_x[i] = _note;
				}
				next = strtok_s(NULL, ":", &ctx);//end_x note
				sprintf_s(_char, SIZE, "%s", next);
				for (int i = 0; i < SIZE; i++) {
					if (strcmp(&_char[i], "") == 0) {
						break;
					}
					int _note = (int)(_char[i] - '0');
					if (_note < 1 || 5 < _note) {
						break;
					}
					_end_x[i] = _note;
				}
			}
			/*#ChangeBPM:9 167.00*/
			else if (strcmp(next, "#ChangeBPM") == 0) {
				next = strtok_s(NULL, ":", &ctx);//�����ߖ�
				for (int i = 0; i < 3; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < (2 - i); j++) {
						c *= 10;
					}
					bar_number += c;
				}
				char _char[SIZE];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(_char, SIZE, "%s", next);
				music.bpm = (float)atof(_char);
				music.rhythm.ChangeRhythm(music.bpm, bar_number);
			}
			DrawLoading();

			for (int i = 0; i < _notecount; i++) {
				double time = music.rhythm.CalculateTime(bar_number, _timing[i], rhythm_note);
				music.notes[noteID].setNote(noteID, channel, _type[i], _first_x[i], _end_x[i], time, bar_number + 1);
				noteID++;
			}
			music.notecount += _notecount;
			/*�f�o�b�O�p*/
			/*
			if (debugMode){
				ClearDrawScreen();
				int _testID = noteID - _notecount;
				int y = 64 + 16 * 1;
				int x = 0;
				DrawFormatString(x, y, GetColor(255, 255, 255), "%d:", bar_number);
				x += 30;
				DrawFormatString(x, y, GetColor(255, 255, 255), "%d/", _notecount);
				x += 25;
				DrawFormatString(x, y, GetColor(255, 255, 255), "%d, ", rhythm_note);
				y += 16;
				DrawString(0, y, "ID  time     (num, ty, f-e) long", GetColor(255, 255, 255));
				y += 16;
				for (int i = 0; i < _notecount; i++) {
					int x = 0;
					DrawFormatString(x, y, GetColor(255, 255, 255), "%d", music.notes[_testID + i].getID());
					x += 30;
					DrawFormatString(x, y, GetColor(255, 255, 255), "%lf", music.notes[_testID + i].gettime());
					x += 100;
					DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", _timing[i]);
					x += 30;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d,", music.notes[_testID + i].getType());
					x += 20;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d-", music.notes[_testID + i].getfirst_x());
					x += 20;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d)", music.notes[_testID + i].getend_x());
					x += 35;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d", music.notes[_testID + i].getlongNoteID());
					x += 80;
					y += 16;
				}
			ScreenFlip();
			WaitKey();//53���ߖځA�����O�m�[�c���ڑ�
			}*/
		}
		FileRead_close(FileHandle);

		QSort(music.notes, 0, music.notecount - 1);

		/*���������A�����O�A�t���b�N�A��*/
		int bar_number = 0, channel = 0;
		for (noteID = 0; noteID < music.notecount; noteID++) {
			bar_number = music.notes[noteID].getbar_number();
			channel = music.notes[noteID].getchannel();
			/*��������*/
			for (int k = noteID + 1; k < music.notecount; k++) {
				if (bar_number < music.notes[k].getbar_number()) {
					break;
				}
				if(fabs(music.notes[noteID].gettime() - music.notes[k].gettime()) 
					<= DBL_EPSILON * 
					fmax(1, fmax(fabs(music.notes[noteID].gettime()), fabs(music.notes[k].gettime())))
					) {
					music.notes[noteID].setsideNoteID(k);
					music.notes[k].setsideNoteID(noteID);
				}
			}
			/*������*/
			if (music.notes[noteID].getType() == 4) {
				for (int k = noteID + 1; k < music.notecount; k++) {
					if (channel == music.notes[k].getchannel() && 
						music.notes[noteID].getfirst_x() == music.notes[k].getfirst_x() &&
						music.notes[noteID].getend_x() == music.notes[k].getend_x()
						) {
						music.notes[noteID].setlongNoteID(k);
						music.notes[k].setlongNoteID(noteID);
						break;
					}
				}
			}
			/*�t���b�N*/
			else if (music.notes[noteID].getType() == 1 || music.notes[noteID].getType() == 3) {
				for (int k = noteID + 1; k < music.notecount; k++) {
					if (music.notes[noteID].getend_x() == music.notes[k].getend_x()) {
						break;
					}
					if (channel == music.notes[k].getchannel()) {
						if (music.notes[k].getType() == 1 || music.notes[k].getType() == 3) {
							/*������*/
							if ((channel == 0 || channel == 1) &&
								music.notes[noteID].getType() == music.notes[k].getType()
								) {
								if (music.notes[noteID].getType() == 1 &&
									(music.notes[noteID].getend_x() - 1) == music.notes[k].getend_x()) {
									music.notes[noteID].setlinkNoteID(k);
									break;
								}
								else if (music.notes[noteID].getType() == 3 &&
									(music.notes[noteID].getend_x() + 1) == music.notes[k].getend_x()) {
									music.notes[noteID].setlinkNoteID(k);
									break;
								}
							}
							/*�W�O�U�O�t���b�N*/
							else if (channel == 2 || channel == 3) {
								music.notes[noteID].setlinkNoteID(k);
								break;
							}
						}
						else {
							break;
						}
					}
				}
			}
		}

		/*�f�o�b�O�p*/
		if (debugMode){
			int t = 0, mass = 15, fontsize = 20;
			SetFontSize(fontsize);
			while (ProcessMessage() == 0
				&& CheckHitKey(KEY_INPUT_ESCAPE) == 0
				&& CheckHitKey(KEY_INPUT_RETURN) == 0
				) {
				int y = 64;
				if (CheckHitKey(KEY_INPUT_UP)) {
					t--;
					if (t < 0) t = 0;
				}
				else if (CheckHitKey(KEY_INPUT_DOWN)) {
					t++;
					if (t + mass >= noteID) t = noteID - mass;
				}
				ClearDrawScreen();
				DrawString(0, y, "bar ID    time     (f-e) type side,long,flic", GetColor(255, 255, 255));
				for (int i = t; i < t + mass; i++) {
					y += fontsize;
					int x = 0;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d", music.notes[i].getbar_number());
					x += 45;
					DrawFormatString(x, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
					x += 45;
					DrawFormatString(x, y, GetColor(255, 255, 255), "%lf", music.notes[i].gettime());
					x += 120;
					DrawFormatString(x, y, GetColor(255, 255, 255), "(%d-", music.notes[i].getfirst_x());
					x += 25;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d)", music.notes[i].getend_x());
					x += 35;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d  ", music.notes[i].getType());
					x += 45;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d  ", music.notes[i].getsideNoteID());
					x += 50;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d  ", music.notes[i].getlongNoteID());
					x += 55;
					DrawFormatString(x, y, GetColor(255, 255, 255), " %d", music.notes[i].getlinkNoteID());
				}
				ScreenFlip();
			}
		}
		ScreenFlip();
	}

private:
	/*�t�@�C���ǂݍ��݂�load���ԕ\��*/
	void DrawLoading() {
		ClearDrawScreen();
		int nowload = (int)FileRead_tell(FileHandle);
		DrawFormatString(0, 0, GetColor(255, 255, 255), "%lf", ((double)nowload) * 100.0 / fileSize);
		DrawString(20 * 6, 0,  "%", GetColor(255, 255, 255));
		ScreenFlip();
	}

	/*���ԏ��Ƀ\�[�g*/
	void QSort(Note x[], int left, int right)
	{
		int i, j;
		double pivot;

		i = left;                      /* �\�[�g����z��̈�ԏ������v�f�̓Y�� */
		j = right;                     /* �\�[�g����z��̈�ԑ傫���v�f�̓Y�� */

		pivot = x[(left + right) / 2].gettime(); /* ��l��z��̒����t�߂ɂƂ� */

		while (1) {                    /* �������[�v */

			while (x[i].gettime() < pivot)       /* pivot ���傫���l�� */
				i++;                   /* �o��܂� i �𑝉������� */

			while (pivot < x[j].gettime())       /* pivot ��菬�����l�� */
				j--;                   /*  �o��܂� j ������������ */
			if (i >= j)                /* i >= j �Ȃ� */
				break;                 /* �������[�v���甲���� */

			Swap(x, i, j);             /* x[i] �� x[j]������ */
			i++;                       /* ���̃f�[�^ */
			j--;
		}

		if (left < i - 1)              /* ��l�̍��� 2 �ȏ�v�f������� */
			QSort(x, left, i - 1);     /* ���̔z��� Q �\�[�g���� */
		if (j + 1 <  right)            /* ��l�̉E�� 2 �ȏ�v�f������� */
			QSort(x, j + 1, right);    /* �E�̔z��� Q �\�[�g���� */
	}

	/* �z��̗v�f���������� */
	void Swap(Note x[], int i, int j)
	{
		Note temp;

		temp = x[i];
		x[i] = x[j];
		x[j] = temp;
	}
};