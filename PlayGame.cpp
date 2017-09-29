#include "FileReader.cpp"
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
	Music music;
	Graph ring;
	Graph noteGraph;
	InputKey input;
	double msec;
public:
	/*id = 楽曲データ
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		noteGraph.setGraph(LoadGraph("materials\\Image\\note1.png"));
		FileReader file;
		music = file.SelectReadFile(id);
		msec = 0;
		Start();
	}

	void Start() {
		/*ゲーム開始時間の取得*/
		double start_time = GetNowCount();
		/*ゲーム内容*/
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			/*ゲーム進行時間*/
			msec = (double)((GetNowCount() - start_time) / 1000.0);

			DrawScreen();

			/*各ノーツの操作*/
			for (int i = 0; i < music.notecount; i++) {
				if (music.notes[i].getflag() && msec >= music.notes[i].gettime()) {
					music.notes[i].Move(15);
					noteGraph.DrawNote(music.notes[i].getend_x(), music.notes[i].getY(), 5, display);
				}
				/*画面外に出た場合*/
				if (music.notes[i].getY() > display.GetScreenY()) {
					music.notes[i].setflag(false);
				}
				/*
				int y = 64 + i * 16;
				DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
				DrawFormatString(20, y, GetColor(255, 255, 255), "(%d,", music.notes[i].getType());
				DrawFormatString(40, y, GetColor(255, 255, 255), " %d,", music.notes[i].getend_x());
				DrawFormatString(70, y, GetColor(255, 255, 255), " %lf,", music.notes[i].gettime());
				DrawFormatString(130, y, GetColor(255, 255, 255), " %d)", music.notes[i].getY());
				*/
			}
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (input.PushOneframe(KEY_INPUT_RETURN))
				break;
		}
	}

	/*常に表示させるもの、スコアなど*/
	void DrawScreen() {
		printfDx("Game Screen\n");
		
		/*時間表示*/
		int _msec = (int)(msec * 1000);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d", _msec % 1000);

		/*リングの表示*/
		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
	}
};
