#include "DxLib.h"

#include <stdio.h>
#include <string.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(500, 500, 16);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)
		return -1;
	//SetDrawScreen(DX_SCREEN_BACK);

	// ＤＸライブラリの初期化
	if (DxLib_Init() == -1) return -1;
	
	int y = 0;
	int FileHandle;
	//FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\Snow Wings.txt");
	FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\sample.txt");
	char string[256];
	char *next;
	char *ctx;//内部利用

	while (FileRead_eof(FileHandle) == 0)
	{
		// 一行読み込み
		FileRead_gets(string, 256, FileHandle);
		//先頭から分割していく　a,b,c = a + b,c
		next = strtok_s(string, ",", &ctx);

		//nextが存在すれば表示
		while (next) {
			//char型を描画
			int i = 0;
			char c[20];
			//文字列の結合
			sprintf_s(c, 20, "#%d", i);
			//文字列の比較
			if (strcmp(next, c) == 0) {
				DrawFormatString(0, y, GetColor(255, 255, 255), "%s", "改行");
				y += 16;
			}
			DrawFormatString(0, y, GetColor(255, 255, 255), "%s", next);
			next = strtok_s(NULL, ":", &ctx);
			y += 16;
		}

		// 画面に描画
		//DrawString(0, y, string, GetColor(255, 255, 255));

		// 表示Ｙ座標を下にずらす
		y += 16;
	}

	// キー入力を待つ
	WaitKey();

	// ファイルを閉じる
	FileRead_close(FileHandle);

	DxLib_End();				// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了 
}