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
	Graph noteGraph[4];
	InputKey input;//ボタン入力
	double msec;//経過時間

	/*一秒当たりの速度(ピクセル数)*/
	const double speed = 500.0;//判定位置がy = 500;
	double d_time = 2.0;//判定位置がy = 500;
	int score = 0;
	int combo = 0;
public:
	/*id = 楽曲データ
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		for (int i = 1; i <= 4; i++) {
			char _imagepath[256];
			strcpy_s(_imagepath, "materials\\Image\\Note");
			char _number[256];
			sprintf_s(_number, 256, "%d.png", i);
			strcat_s(_imagepath, _number);
			noteGraph[i - 1].setGraph(LoadGraph(_imagepath));
		}
		FileReader file;
		music = file.SelectReadFile(id);
		msec = 0;
		Start();
	}

	void Start() {
		/*ゲーム開始時間の取得*/
		double start_time = GetNowCount();
		PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK);

		/*消化し終わったのノーツをfor文から外す*/
		int process = 0;
		/*ゲーム内容*/
		while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
			ClearDrawScreen();
			clsDx();
			/*ゲーム進行時間*/
			msec = (double)((GetNowCount() - start_time) / 1000.0) - 0.6;
			//msec = (double)((GetNowCount() - start_time) / 1000.0) - 8.5f;
			DrawScreen();

			int debugcount = 0;
			/*各ノーツの操作*/
			for (int i = 0; i < music.notecount; i++) {
				/*処理済みの場合*/
				if (music.notes[i].getflag() == -1) {
					/*画面下の場合は、forに含めない*/
					if (music.notes[i].getY() > display.GetScreenY()) {
						//continue;
					}
					/*画面内及び画面上の場合*/
					else {
						/*画面内にいる時間の場合*/
						if (music.notes[i].gettime() <= (msec + d_time)) {
							double dt = (msec + d_time) - (music.notes[i].gettime());
							music.notes[i].ToMove(speed, dt);
						}
					}
				}
				/*画面上部で存在*/
				if (music.notes[i].getflag() == 0) {
					/*時間がきたら表示*/
					if (music.notes[i].gettime() <= (msec + d_time)) {
						music.notes[i].setflag(1);
					}
					else {
						//continue;
					}
				}
				/*画面内に表示されている場合*/
				if (music.notes[i].getflag() == 1) {
					/*画面内の場合は表示*/
					if (music.notes[i].getY() <= display.GetScreenY()) {

						double dt = (msec + d_time) - music.notes[i].gettime();
						music.notes[i].ToMove(speed, dt);

						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getend_x(), music.notes[i].getY(), 5, display);
						/*判定内の場合*/
						if (-200 + display.GetScreenY() <= music.notes[i].getY() && 
							music.notes[i].getY() <= display.GetScreenY()) {
							/*ボタンが押されていたら*/
							if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
								music.notes[i].setflag(-1);
								if (-150 <= music.notes[i].getY() - display.GetScreenY() && 
									music.notes[i].getY() - display.GetScreenY() <= 50) {
									combo++;
									score += 500;
								}
								else {
									score += 300;
								}
							}
						}
					}
					/*画面下の場合は処理*/
					else {
						music.notes[i].setflag(-1);
					}
				}
				/*デバッグ用*/
				/*
				if (music.notes[i].gettime() <= (msec + d_time) &&
					music.notes[i].getflag() != -1) {
					debugcount++;
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID [type,pos,time, Y]");
					int y = 96 + debugcount * 16;
					DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
					DrawFormatString(30, y, GetColor(255, 255, 255), "[%d ", music.notes[i].getType());
					DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[i].getend_x());
					DrawFormatString(70, y, GetColor(255, 255, 255), " %lf ", music.notes[i].gettime());
					DrawFormatString(160, y, GetColor(255, 255, 255), " %d]", music.notes[i].getY());
				}
				*/
			}
			ScreenFlip();// 裏画面の内容を表画面に反映させる 
			if (input.PushOneframe(KEY_INPUT_RETURN)) {
				StopSoundMem(music.soundHandle);
				DeleteSoundMem(music.soundHandle);
				break;
			}
		}
	}

	/*常に表示させるもの、スコアなど*/
	void DrawScreen() {
		printfDx("Game Screen\n");
		
		/*時間表示*/
		int _msec = (int)(msec * 1000);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d", _msec % 1000);
		//DrawFormatString(100, 64, GetColor(255, 255, 255), "BPM = %lf", music.bpm);
		DrawFormatString(0, 64, GetColor(255, 255, 255), "score = %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "combo = %d", combo);

		/*リングの表示*/
		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);
		ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		DrawLine(0, display.GetScreenY() - 200, display.GetScreenX(), display.GetScreenY() - 200, GetColor(0, 255, 0));
	}
};
