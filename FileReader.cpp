#include "Graphics.cpp"
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
	char title[256];
	float bpm;
	int soundHandle;
	int offset;//(ms)
	int notecount;
	Note notes[1000];
	Rhythm rhythm;

	Music() {
		offset = notecount = 0;
		bpm = 0.0f;
	}
	void setX(double _posX) {
		for (int i = 0; i < notecount; i++) {
			notes[i].setx((int)(notes[i].getfirst_x() * _posX));
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
class FileReader {
private:
	Music music;
	char filepath[256] = "materials\\";
	int FileHandle;
	char *next;
	char *ctx;//内部利用
public:
	FileReader() {

	}

	//環境設定、BPM等
	void SetConfig() {
		char string[256];
		while (FileRead_eof(FileHandle) == 0)
		{
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
			}
			else if (strcmp(next, "#Song") == 0) {
				char sound_name[256];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(sound_name, 256, "%s", next);
				try {
					char _filepath[256];
					strcpy_s(_filepath, filepath);
					strcat_s(_filepath, "Music\\");
					strcat_s(_filepath, sound_name);
					music.soundHandle = LoadSoundMem(_filepath);
					if (music.soundHandle == -1) {
						char error[256];
						strcpy_s(error, _filepath);
						strcat_s(error, " is not found.");
						throw error;
					}
				}
				catch (char* text) {
					ClearDrawScreen();
					clsDx();
					printfDx(text);
					ScreenFlip();
					WaitKey();
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
				int count = 4;
				next = strtok_s(NULL, "", &ctx);
				for (int i = 0; i < count; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < ((count - 1) - i); j++) {
						c *= 10;
					}
					music.offset += c;
				}
			}
			else if (strcmp(next, "#end") == 0) {
				break;
			}
		}
	}

	void SetNote() {
		const int SIZE = 64;
		int noteID = 0;//ノーツに番号を割り振る
					   /*デバッグ用*/
		int readline = 0;

		/*同一小節上での処理判定に使用*/
		bool same_bar_number = false;

		/*同一小節上、もしくはロングノーツの保存*/
		class Memory {
		public:
			int bar_number;
			/*同一小節上での処理に使用*/
			int _timing[SIZE];
			/*その小節が何拍子かの保存*/
			int _rhythm_note;
			/*その小節のノーツ数*/
			int _notecount;
			/*noteIDの保存*/
			int noteID[SIZE];
			/*ロングノーツのID保存*/
			int longNoteID[32];
			int count = 0;
			Memory() {
				bar_number = _rhythm_note = 0;
				for (int i = 0; i < 32; i++) {
					longNoteID[i] = -1;
				}
			}
			void reset(int _bar_number) {
				bar_number = _bar_number;
				for (int i = 0; i < SIZE; i++) {
					noteID[i] = -1;
					_timing[i] = -1;
				}
				_rhythm_note = 0;
				_notecount = 0;
			}
			void setLongNoteID(int _id) {
				longNoteID[count] = _id;
				count++;
			}
			void CompressionlongNoteID() {
				for (int i = 0; i < 32 - 1; i++) {
					if (longNoteID[i] == -1 && longNoteID[i + 1] > 0) {
						longNoteID[i] = longNoteID[i + 1];
						longNoteID[i + 1] = -1;
						count = i + 1;
					}
					else if (longNoteID[i] == -1 && longNoteID[i + 1] == -1) {
						count = i;
						break;
					}
				}
			}
		};

		Memory memory;

		while (FileRead_eof(FileHandle) == 0) {
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
				same_bar_number = (memory.bar_number == bar_number);
				/*同一小節でない場合*/
				if (!same_bar_number) {
					/*memoryの初期化*/
					memory.reset(bar_number);
				}

				next = strtok_s(NULL, ":", &ctx);//rhythm & type note
				char _char[SIZE];
				sprintf_s(_char, SIZE, "%s", next);
				for (int i = 0; i < SIZE; i++) {
					/*空文字であれば読み込み終了*/
					if (strcmp(&_char[i], "") == 0) {
						/*同一小節でない場合、memoryの改ざん*/
						if (!same_bar_number) {
							memory._rhythm_note = i;
							memory._notecount = _notecount;
						}
						rhythm_note = i;
						break;
					}
					/*文字コードから数字を出す "0" = 0*/
					int _note = (int)(_char[i] - '0');
					if (_note > 0) {
						_timing[_notecount] = i;
						_type[_notecount] = _note;
						if (_note == 4) {
							memory.setLongNoteID(noteID + _notecount);
						}
						/*同一小節でない場合、memoryの改ざん*/
						if (!same_bar_number) {
							memory._timing[_notecount] = _timing[_notecount];
						}
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

			for (int i = 0; i < _notecount; i++) {
				double time = music.rhythm.CalculateTime(bar_number, _timing[i], rhythm_note);
				//double time = (double)bar_number * music.rhythm.getRhythm(1) + (double)_timing[i] * music.rhythm.getRhythm(rhythm_note);
				music.notes[noteID].setNote(noteID, _type[i], _first_x[i], _end_x[i], time);

				/*同時押し*/
				if (same_bar_number) {
					for (int j = 0; j < memory._notecount; j++) {
						float _memory = (float)memory._timing[j] / (float)memory._rhythm_note;
						float d = (float)_timing[i] / (float)rhythm_note;
						if (_memory == d) {
							music.notes[noteID].setsideNoteID(memory.noteID[j]);
							music.notes[memory.noteID[j]].setsideNoteID(noteID);
						}
					}
				}
				else {
					memory.noteID[i] = noteID;
				}
				/*ロングノーツの連結*/
				for (int i = 0; i < memory.count; i++) {
					if (0 < memory.longNoteID[i] && memory.longNoteID[i] < noteID) {
						if (music.notes[memory.longNoteID[i]].getfirst_x() == music.notes[noteID].getfirst_x() &&
							music.notes[memory.longNoteID[i]].getend_x() == music.notes[noteID].getend_x()
							) {
							music.notes[memory.longNoteID[i]].setlongNoteID(noteID);
							memory.longNoteID[i] = -1;
							memory.CompressionlongNoteID();
						}
					}
				}
				/*フリックの連結*/
				if ((_type[i] == 1 || _type[i] == 3) && ((i - 1) >= 0)) {
					/*同じ方向のフリックの連結*/
					if (channel == 0 || channel == 1) {
						if (_type[i] == _type[i - 1]) {
							music.notes[noteID - 1].setlinkNoteID(noteID);
						}
					}
					/*ジグザグなフリックの連結*/
					else if (channel == 2 || channel == 3) {
						if (_type[i - 1] == 1 || _type[i - 1] == 3) {
							music.notes[noteID - 1].setlinkNoteID(noteID);
						}
					}
				}
				noteID++;
			}
			music.notecount += _notecount;

			/*デバッグ用*/
			/*int _testID = noteID - _notecount;
			int y = 64 + 16 * readline;
			int x = 0;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d:", bar_number);
			x += 30;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d/", _notecount);
			x += 25;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%d, ", rhythm_note);
			x += 30;
			DrawFormatString(x, y, GetColor(255, 255, 255), "%lf, ", music.bpm);
			x += 100;
			for (int i = 0; i < _notecount; i++) {
				//DrawFormatString(x, y, GetColor(255, 255, 255), "%d", music.notes[_testID + i].getID());
				//x += 20;
				//DrawFormatString(x, y, GetColor(255, 255, 255), " %lf", music.notes[_testID + i].gettime());
				//x += 100;
				/*DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", _timing[i]);
				x += 20;
				//DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", music.notes[_testID + i].getType());
				//x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d,", music.notes[_testID + i].getfirst_x());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d)", music.notes[_testID + i].getend_x());
				x += 35;
				DrawFormatString(x, y, GetColor(255, 255, 255), "long:%d", music.notes[_testID + i].getlongNoteID());
				x += 80;
			}*/
		}
		FileRead_close(FileHandle);
		//ScreenFlip();
		//WaitKey();
	}

	/*1. snow wings 2 tokimeki*/
	Music SelectReadFile(int id) {
		try {
			char _filepath[256];
			strcpy_s(_filepath, filepath);
			strcat_s(_filepath, "NoteFile\\");
			if (id == 1) {
				strcat_s(_filepath, "Snow Wings.txt");
			}
			else if (id == 2) {
				strcat_s(_filepath, "TOKIMEKI.txt");
			}
			FileHandle = FileRead_open(_filepath);
			if (FileHandle == 0) {
				char error[256];
				strcpy_s(error, _filepath);
				strcat_s(error, " is not found.");
				throw error;
			}
			else {
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
};