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
	
	/*char s1[] = "this is a pen. Hello-World...";
	char s2[] = " -.";   空白+ハイフン+ピリオド 
	char *tok;

	tok = strtok(s1, s2);
	while (tok != NULL) {
		printf("%s¥n", tok);
		tok = strtok(NULL, s2);   2回目以降 
	}*/
	int y = 0;

	int FileHandle;
	FileHandle = FileRead_open("C:\\Users\\admin\\Desktop\\ui\\Snow Wings.txt");
	char String[256];
	char *tok;
	char *ctx;

	while (FileRead_eof(FileHandle) == 0)
	{
		// 一行読み込み
		FileRead_gets(String, 256, FileHandle);
		tok = strtok_s(String, ":", &ctx);
		while (tok != NULL) {
			tok = strtok_s(NULL, ",", &ctx);
		}
		// 画面に描画
		DrawString(0, y, String, GetColor(255, 255, 255));

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