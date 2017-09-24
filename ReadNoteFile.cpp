#include "DxLib.h"

/*楽曲データリスト
idに対応する.txtを読み込み、譜面や楽曲データを返却する
未実証
*/
class MusicList {
public:
	char title[256];
	char background_name[256];
	char sound_name[256];
	float bpm;
	int sound, notecount;

	MusicList() {
		sound = notecount = NULL;
		bpm = NULL;
	}
	void setMusicList(int id) {
		if (id == 1) {
			int y = 0;
			int FileHandle;
			char string[256];
			char *next;
			char *ctx;//内部利用

			try {
				FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\Snow Wings.txt");
				if (FileHandle == 0) {
					throw "Not File";
				}
				while (FileRead_eof(FileHandle) == 0)
				{
					// 一行読み込み
					FileRead_gets(string, 256, FileHandle);
					//先頭から分割していく　a,b,c = a + b,c
					next = strtok_s(string, " ", &ctx);

					while (next) {
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
						//else if (strcmp(next, "#BPM") == 0) {
							/*next = strtok_s(NULL, "", &ctx);
							char char_bpm[256];
							sprintf_s(char_bpm, 256, "%d", next);
							bpm = (float)char_bpm;*/
							//}

							//char型を描画
							//int i = 0;
							//char c[256];
							//文字列の結合
							//sprintf_s(c, 256, "#%d", i);
							//if (strcmp(next, c) == 0) {
								//DrawFormatString(0, y, GetColor(255, 255, 255), "%s", "改行");
							//}
							//DrawFormatString(0, y, GetColor(255, 255, 255), "%s", next);
						next = strtok_s(NULL, ":", &ctx);
					}
				}
				FileRead_close(FileHandle);
			}
			catch (char* text) {
				clsDx();
				printfDx(text);
			}
			//sound = NULL;
			bpm = 138;
			notecount = 466;
		}
	}
};