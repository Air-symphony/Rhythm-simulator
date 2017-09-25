#include "ReadNoteFile.cpp"
#include "InputKey.cpp"
/*
game画面
Display display;
Music music;
Graph ring;
InputKey input;
GameScreen(Display _display, int id) 
id = 楽曲データ
*/
class GameScreen {
private:
	Display display;
	MusicList music;
	Graph ring;
	InputKey input;
public:
	/*id = 楽曲データ
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("C:\\Users\\admin\\Desktop\\ui\\ring.png"));
		music.ReadFile(id);
		Start();
	}

	void Start() {
		int frame = 0, second = 0;
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			printfDx("Game Screen\n");
			char c[256];
			//文字列の結合
			sprintf_s(c, 256, "title = %s\n", music.title);
			printfDx(c); 
			char b[256];
			sprintf_s(b, 256, "bpm = %f\n", music.bpm);
			printfDx(b);

			frame += 1;
			if (frame >= 60) {
				frame = 0;
				second += 1;
			}
			//for (int i = 0; i < music.notecount; i++) {}

			DrawScreen();
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	void DrawScreen() {
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
	}
};
