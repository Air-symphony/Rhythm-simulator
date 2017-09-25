#include "Graphics.cpp"
#include "Note.cpp"

/*楽曲データ
BGM,BPM,Noteなどを管理
int sound, bpm, notecount;
Graph notegraph[5];
Note notes[1000];
*/
class Music{
public:
	char title[256];
	char background_name[256];
	char sound_name[256];
	float bpm;
	int sound, notecount;
	Graph notegraph[5];
	Note notes[1000];
	Rhythm rhythm;

	Music() {

	}
};

/*楽曲データリスト
idに対応する.txtを読み込み、譜面や楽曲データを返却する
未実証
*/
class MusicList :public Music{
private:
	int FileHandle;
	char string[256];
	char *next;
	char *ctx;//内部利用
public:
	MusicList() {
		sound = notecount = NULL;
		bpm = NULL;
	}

	//環境設定、BPM等
	void SetConfig() {
		while (FileRead_eof(FileHandle) == 0)
		//while (strcmp(next, "#end") != 0)
		{
			// 一行読み込み
			FileRead_gets(string, 256, FileHandle);
			//先頭から分割していく　a,b,c = a + b,c
			next = strtok_s(string, " ", &ctx);
			//文字列の比較
			if (strcmp(next, "#Title") == 0) {
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(title, 256, "%s", next);
			}
			else if (strcmp(next, "#BackGround") == 0) {
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(background_name, 256, "%s", next);
			}
			else if (strcmp(next, "#Song") == 0) {
				next = strtok_s(NULL, "", &ctx);
				sprintf_s(sound_name, 256, "%s", next);
			}
			else if (strcmp(next, "#BPM") == 0) {
				next = strtok_s(NULL, "", &ctx);
				char char_bpm[256];
				sprintf_s(char_bpm, 256, "%s", next);
				bpm = (float)atof(char_bpm);
			}
			else if (strcmp(next, "#end") == 0) {
				break;
			}
		}
	}

	void SetMusic() {
		int y = 64;
		while (FileRead_eof(FileHandle) == 0) {
			FileRead_gets(string, 256, FileHandle);
			next = strtok_s(string, ",", &ctx);

			int note[32];
			//文字列の比較
			if (strcmp(next, "#0") == 0) {
				next = strtok_s(NULL, ":", &ctx);
				next = strtok_s(NULL, ":", &ctx);
				int i = 0;
				DrawFormatString(0, y, GetColor(255, 255, 255), "%s", next);
				while (false) {
					char char_bpm[256];
					sprintf_s(char_bpm, 256, "%s", next);
					note[i] = (int)atof(char_bpm);
					i++;
					if (strcmp(next, ":") == 0) {
						break;
					}
				}
				//DrawFormatString(0, y, GetColor(255, 255, 255), "%s", i);
				y += 16;
			}
			/*else if (strcmp(next, "#1") == 0) {
				next = strtok_s(NULL, ":", &ctx);
				next = strtok_s(NULL, ":", &ctx);
				char char_bpm[256];
				sprintf_s(char_bpm, 256, "%s", next);
				bpm = (float)atof(char_bpm);
			}*/

			//char型を描画
			//int i = 0;
			//char c[256];
			//文字列の結合
			//sprintf_s(c, 256, "#%d", i);
			//if (strcmp(next, c) == 0) {
			//DrawFormatString(0, y, GetColor(255, 255, 255), "%s", "改行");
			//}
			//DrawFormatString(0, y, GetColor(255, 255, 255), "%s", next);
		}
		FileRead_close(FileHandle);
		//sound = NULL;
		notecount = 466;
		ScreenFlip();// 裏画面の内容を表画面に反映させる
		WaitKey();
	}

	void ReadFile(int id) {
		try {
			if (id == 1) {
				FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\Snow Wings.txt");
			}
			else if (id == 2) {
				//FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\Snow Wings.txt");
			}
			if (FileHandle == 0) {
				throw "Not File";
			}
			else {
				SetConfig();
				SetMusic();
			}
		}
		catch (char* text) {
			clsDx();
			printfDx(text);
			WaitKey();
		}
		//ScreenFlip();// 裏画面の内容を表画面に反映させる 
		//WaitKey();
	}
};