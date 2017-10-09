#include "Graphics.cpp"
#include "Note.cpp"

/*楽曲データ
BGM,BPM,Noteなどを管理
char title[256];
float bpm;
int soundHandle;
int notecount;
Note notes[1000];
Rhythm rhythm;
*/
class Music{
public:
	char title[256];
	float bpm;
	int soundHandle;
	int notecount;
	Note notes[1000];
	Rhythm rhythm;

	Music() {
		notecount = 0;
		bpm = 0.0f;
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
class FileReader{
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
			else if (strcmp(next, "#end") == 0) {
				break;
			}
		}
	}

	void SetNote() {
		const int cher_size = 64;
		int noteID = 0;//ノーツに番号を割り振る
		/*デバッグ用*/
		int readline = 0;

		while (FileRead_eof(FileHandle) == 0) {
			/*読み込み時の仮保存用*/
			int _type[32], _timing[32], _first_x[32], _end_x[32];
			/*何小節目か*/
			int bar_number = 0;
			/*この節のノーツ数*/
			int _notecount = 0;
			/*その小節が何拍子か*/
			int rhythm_note = 0;

			/*デバッグ用*/
			readline++;

			char string[256];
			FileRead_gets(string, 256, FileHandle);
			next = strtok_s(string, ",", &ctx);
			/*文字列の比較*/
			if (strcmp(next, "#0") == 0 || strcmp(next, "#1") == 0) {


				next = strtok_s(NULL, ":", &ctx);//何小節目
				for (int i = 0; i < 3; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < (2 - i); j++) {
						c *= 10;
					}
					bar_number += c;
				}

				next = strtok_s(NULL, ":", &ctx);//rhythm & type note
				char _char[cher_size];
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
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
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
					if (strcmp(&_char[i], "") == 0) {
						break;
					}
					int _note = (int)(_char[i] - '0');
					_first_x[i] = _end_x[i] = _note;
				}
				next = strtok_s(NULL, ":", &ctx);//end_x note
				sprintf_s(_char, cher_size, "%s", next);
				for (int i = 0; i < cher_size; i++) {
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
			/*#ChangeBPM,167.00*/
			else if (strcmp(next, "#ChangeBPM") == 0) {
				next = strtok_s(NULL, ":", &ctx);//何小節目
				for (int i = 0; i < 3; i++) {
					int c = (int)(next[i] - '0');
					for (int j = 0; j < (2 - i); j++) {
						c *= 10;
					}
					bar_number += c;
				}
				char _char[cher_size];
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(_char, cher_size, "%s", next);
				music.bpm = (float)atof(_char);
				music.rhythm.ChangeRhythm(music.bpm, bar_number);
			}
			for (int i = 0; i < _notecount; i++) {
				double time = music.rhythm.CalculateTime(bar_number, _timing[i], rhythm_note);
				//double time = (double)bar_number * music.rhythm.getRhythm(1) + (double)_timing[i] * music.rhythm.getRhythm(rhythm_note);
				music.notes[noteID].setNote(noteID, _type[i], _first_x[i], _end_x[i], time);
				noteID++;
			}
			music.notecount += _notecount;

			/*デバッグ用*/
			int _testID = noteID - _notecount;
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
				DrawFormatString(x, y, GetColor(255, 255, 255), "%d", music.notes[_testID + i].getID());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %lf", music.notes[_testID + i].gettime());
				x += 100;
				/*DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", _timing[i]);
				x += 20;
				//DrawFormatString(x, y, GetColor(255, 255, 255), "(%d,", music.notes[_testID + i].getType());
				//x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d,", music.notes[_testID + i].getfirst_x());
				x += 20;
				DrawFormatString(x, y, GetColor(255, 255, 255), " %d)", music.notes[_testID + i].getend_x());
				x += 35;*/
			}
		}
		FileRead_close(FileHandle);
		ScreenFlip();
		WaitKey();
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