#include "HitEffect.cpp"
#include "Note.cpp"

/*楽曲データ
BGM,BPM,Noteなどを管理
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

/*楽曲データリスト
idに対応する.txtを読み込み、譜面や楽曲データを返却する
Music music;
char filepath[256] = "materials\\";
int FileHandle;
char *next;
char *ctx;//内部利用
*/
class NoteFileReader {
	bool debugMode = false;
private:
	Music music;
	char filepath[20] = "materials\\";
	int FileHandle = 0;
	int fileSize = 0;
	char *next;
	char *ctx;//内部利用
public:
	NoteFileReader() {

	}
	/*NoteFileの読み込み、Noteの準備*/
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
	//環境設定、BPM等
	void SetConfig() {
		char string[256] = "";
		char errorMessage[512] = "";
		bool error = false;
		while (FileRead_eof(FileHandle) == 0)
		{
			DrawLoading();
			// 一行読み込み
			FileRead_gets(string, 256, FileHandle);
			//先頭から分割していく　a,b,c = a + b,c
			next = strtok_s(string, " ", &ctx);
			//文字列の比較
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
				/*next内に含まれるかどうか*/
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
		int noteID = 0;//ノーツに番号を割り振る
		/*デバッグ用*/
		int readline = 0;
		
		DrawLoading();
		while (FileRead_eof(FileHandle) == 0) {
			DrawLoading();
			/*読み込み時の仮保存用*/
			int _type[SIZE], _timing[SIZE], _first_x[SIZE], _end_x[SIZE];
			/*何小節目か*/
			int bar_number = 0;
			/*この節のノーツ数*/
			int _notecount = 0;
			/*その小節が何拍子か*/
			int rhythm_note = 0;
			/*・「0」か「1」の時　…　フリック方向の先にあるフリックノートと接続
			・「2」か「3」の時　…　フリック方向に関係なく接続
			（※推奨は <左手用：0,2> <右手用：1,3> です。）*/
			int channel = -1;

			/*デバッグ用*/
			readline++;

			char string[256];
			FileRead_gets(string, 256, FileHandle);
			next = strtok_s(string, ",", &ctx);
			/*文字列の比較*/
			if (strcmp(next, "#0") == 0 || strcmp(next, "#1") == 0 ||
				strcmp(next, "#2") == 0 || strcmp(next, "#3") == 0) {
				channel = (int)(next[1] - '0');

				next = strtok_s(NULL, ":", &ctx);//何小節目
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
					/*空文字であれば読み込み終了*/
					if (strcmp(&_char[i], "") == 0) {
						rhythm_note = i;
						break;
					}
					/*文字コードから数字を出す "0" = 0*/
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
				next = strtok_s(NULL, ":", &ctx);//何小節目
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
			/*デバッグ用*/
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
			WaitKey();//53小節目、ロングノーツ未接続
			}*/
		}
		FileRead_close(FileHandle);

		QSort(music.notes, 0, music.notecount - 1);

		/*同時押し、ロング、フリック連結*/
		int bar_number = 0, channel = 0;
		for (noteID = 0; noteID < music.notecount; noteID++) {
			bar_number = music.notes[noteID].getbar_number();
			channel = music.notes[noteID].getchannel();
			/*同時押し*/
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
			/*長押し*/
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
			/*フリック*/
			else if (music.notes[noteID].getType() == 1 || music.notes[noteID].getType() == 3) {
				for (int k = noteID + 1; k < music.notecount; k++) {
					if (music.notes[noteID].getend_x() == music.notes[k].getend_x()) {
						break;
					}
					if (channel == music.notes[k].getchannel()) {
						if (music.notes[k].getType() == 1 || music.notes[k].getType() == 3) {
							/*同方向*/
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
							/*ジグザグフリック*/
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

		/*デバッグ用*/
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
	/*ファイル読み込みのload時間表示*/
	void DrawLoading() {
		ClearDrawScreen();
		int nowload = (int)FileRead_tell(FileHandle);
		DrawFormatString(0, 0, GetColor(255, 255, 255), "%lf", ((double)nowload) * 100.0 / fileSize);
		DrawString(20 * 6, 0,  "%", GetColor(255, 255, 255));
		ScreenFlip();
	}

	/*時間順にソート*/
	void QSort(Note x[], int left, int right)
	{
		int i, j;
		double pivot;

		i = left;                      /* ソートする配列の一番小さい要素の添字 */
		j = right;                     /* ソートする配列の一番大きい要素の添字 */

		pivot = x[(left + right) / 2].gettime(); /* 基準値を配列の中央付近にとる */

		while (1) {                    /* 無限ループ */

			while (x[i].gettime() < pivot)       /* pivot より大きい値が */
				i++;                   /* 出るまで i を増加させる */

			while (pivot < x[j].gettime())       /* pivot より小さい値が */
				j--;                   /*  出るまで j を減少させる */
			if (i >= j)                /* i >= j なら */
				break;                 /* 無限ループから抜ける */

			Swap(x, i, j);             /* x[i] と x[j]を交換 */
			i++;                       /* 次のデータ */
			j--;
		}

		if (left < i - 1)              /* 基準値の左に 2 以上要素があれば */
			QSort(x, left, i - 1);     /* 左の配列を Q ソートする */
		if (j + 1 <  right)            /* 基準値の右に 2 以上要素があれば */
			QSort(x, j + 1, right);    /* 右の配列を Q ソートする */
	}

	/* 配列の要素を交換する */
	void Swap(Note x[], int i, int j)
	{
		Note temp;

		temp = x[i];
		x[i] = x[j];
		x[j] = temp;
	}
};