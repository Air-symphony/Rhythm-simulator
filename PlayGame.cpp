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
	Graph Line;
	int SE[2];
	InputKey input;//ボタン入力
	double msec;//経過時間
	double d_msec;//デバッグ用、変化値

	/*判定位置のy座標*/
	const double judgePos = display.GetScreenY() - 75.0;//判定位置
	/*何秒かけて判定位置にたどり着くか*/
	double speed = 0.6;

	int score = 0, combo = 0;

	char judge_text[5][10] = { "PERFECT","GREAT", "NICE", "BAD", "MISS" };
	bool printjudge = false;
	int judge_number = 0;/*判定内容を保存、毎フレームで初期化*/
	double printjudge_time = 0.0;
	int printjudge_number = 0;

	bool autoMode = false;

	const double PERFECTtime = 0.066;
	const double GREATtime = 0.090;
	const double NICEtime = 0.115;
	const double BADtime = 0.130;
public:
	/*id = 楽曲データ
	*/
	GameScreen(Display _display, int id) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		ring.setDisplay(display);
		Line.setDisplay(display);
		for (int i = 1; i <= 4; i++) {
			char _imagepath[256];
			strcpy_s(_imagepath, "materials\\Image\\Note");
			char _number[256];
			sprintf_s(_number, 256, "%d.png", i);
			strcat_s(_imagepath, _number);
			noteGraph[i - 1].setGraph(LoadGraph(_imagepath));
			noteGraph[i - 1].setDisplay(display);
		}
		SE[0] = LoadSoundMem("materials\\SE\\PERFECT SE.ogg");
		SE[1] = LoadSoundMem("materials\\SE\\フリック SE.wav");
		ChangeVolumeSoundMem(255 * 50 / 100, SE[0]);
		ChangeVolumeSoundMem(255 * 50 / 100, SE[1]);

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

		/*AutoModeの真偽*/
		autoMode = true;
		/*ゲーム内容*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			ClearDrawScreen();
			clsDx();
			/*ゲーム進行時間*/
			msec = (double)((GetNowCount() - start_time) / 1000.0) + 0.5;//Snow wings合わせ
			//msec = (double)((GetNowCount() - start_time) / 1000.0) - 8.5f;
			
			/*常に表示させるもの*/
			DrawScreen();
			
			/*判定内容を保存、毎フレームで初期化*/
			judge_number = -1;
			int debugcount = 0;

			/*各ノーツの操作*/
			for (int i = 0; i < music.notecount; i++) {
				/*処理済みの場合*/
				if (music.notes[i].getflag() == -1) {
					/*画面下の場合は、forに含めない*/
					if (music.notes[i].getY() > display.GetScreenY()) {
						continue;
					}
					/*画面内及び画面上の場合*/
					else {
						/*画面内にいる時間の場合*/
						if (music.notes[i].gettime() <= (msec + speed)) {
							double dt = (msec + speed) - (music.notes[i].gettime());
							music.notes[i].ToMove(judgePos, dt, speed);
						}
					}
				}
				/*画面上部で存在*/
				if (music.notes[i].getflag() == 0) {
					/*時間がきたら表示*/
					if (music.notes[i].gettime() <= (msec + speed)) {
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
						double dt = (msec + speed) - music.notes[i].gettime();
						music.notes[i].ToMove(judgePos, dt, speed);
						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getend_x(), music.notes[i].getY());
						
						if (music.notes[i].getlinkNoteID() > 0) {
							Line.Draw_LinkLine(music.notes[i].getend_x(), music.notes[i].getY(),
								music.notes[music.notes[i].getlinkNoteID()].getend_x(), 
								music.notes[music.notes[i].getlinkNoteID()].getY()
							);
						}
						/*判定内容*/
						if (autoMode) {
							AutoMode(i);
						}
						else {
							Judge(i);
						}
					}
					/*画面下の場合は処理*/
					else {
						music.notes[i].setflag(-1);
						combo = 0;
						judge_number = 4;
					}
				}
				/*デバッグ用*/
				if (music.notes[i].gettime() <= (msec + speed) &&
					music.notes[i].getflag() != -1) {
					debugcount++;
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID type,pos,time, Y");
					int y = 96 + debugcount * 16;
					DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
					DrawFormatString(30, y, GetColor(255, 255, 255), "[%d ", music.notes[i].getType());
					DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[i].getend_x());
					DrawFormatString(70, y, GetColor(255, 255, 255), " %lf ", music.notes[i].gettime());
					DrawFormatString(160, y, GetColor(255, 255, 255), " %d]", music.notes[i].getY());
				}
			}
			/*判定を表示する必要性があったとき*/
			if (judge_number != -1) {
				printjudge_time = msec;
				printjudge_number = judge_number;
				printjudge = true;
			}
			/*判定結果の表示(1秒)*/
			if (printjudge) {
				DrawFormatString(350, display.GetScreenY() / 2, GetColor(255, 255, 255), "%s", judge_text[printjudge_number]);
				printjudge = ((msec - printjudge_time) <= 1.0);
			}

			ScreenFlip();// 裏画面の内容を表画面に反映させる 

			/*ゲーム画面の終了*/
			if (input.PushOneframe_Debug()) {
				StopSoundMem(music.soundHandle);
				DeleteSoundMem(music.soundHandle);
				DeleteSoundMem(SE[0]);
				DeleteSoundMem(SE[1]);
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
		/*スコア表示*/
		DrawFormatString(0, 64, GetColor(255, 255, 255), "score = %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "combo = %d", combo);

		/*1フレーム当たりの秒数*/
		DrawFormatString(70, 48, GetColor(255, 255, 255), "%lf:", msec - d_msec);
		d_msec = msec;

		char c[256];
		sprintf_s(c, 256, "title = %s\n", music.title);
		printfDx(c);

		/*オートモードの切り替え*/
		if (input.PushOneframe_ChangeAutoMode()) {
			autoMode = !autoMode;
		}
		if (autoMode) {
			DrawString(0, display.GetScreenY() - 20, "AutoMode", GetColor(255, 255, 255));
		}

		/*リングの表示*/
		//ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		/*判定範囲の表示*/
		DrawLine(0, (int)judgePos, display.GetScreenX(), (int)judgePos, GetColor(255, 0, 0));
		int y = (int)((judgePos / speed) * (PERFECTtime / 2.0));
		DrawLine(0, (int)judgePos - y, display.GetScreenX(), (int)judgePos - y, GetColor(0, 255, 0));
		DrawLine(0, (int)judgePos + y, display.GetScreenX(), (int)judgePos + y, GetColor(0, 255, 0));
		y = (int)((judgePos / speed) * (GREATtime / 2.0));
		DrawLine(0, (int)(judgePos - y), display.GetScreenX(), (int)(judgePos - y), GetColor(0, 0, 255));
		DrawLine(0, (int)(judgePos + y), display.GetScreenX(), (int)(judgePos + y), GetColor(0, 0, 255));
		y = (int)((judgePos / speed) * (NICEtime / 2.0));
		DrawLine(0, (int)judgePos - y, display.GetScreenX(), (int)judgePos - y, GetColor(255, 255, 0));
		DrawLine(0, (int)judgePos + y, display.GetScreenX(), (int)judgePos + y, GetColor(255, 255, 0));
	}

	/*ボタン判定の記述*/
	/*return -1, 0,1,2,3*/
	void Judge(int i) {
		/*BAD判定内の場合*/
		if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
			/*押された時間の取得*/
			double _clear_time = abs(music.notes[i].gettime() - msec);

			/*ボタンが押されていたら*/
			if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
				music.notes[i].setflag(-1);
				if (_clear_time <= PERFECTtime / 2.0) {
					combo++;
					score += 300;
					PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
					judge_number = 0;
					return;
				}
				else if (_clear_time <= GREATtime / 2.0) {
					combo++;
					score += 150;
					judge_number = 1;
					return;
				}
				else if (_clear_time <= NICEtime / 2.0) {
					combo = 0;
					score += 100;
					judge_number = 2;
					return;
				}
				else {
					combo = 0;
					score += 50;
					judge_number = 3;
					return;
				}
			}
		}
	}

	/*オートモードの内容*/
	void AutoMode(int i) {
		/*中心バーを越えたらすぐ*/
		if (music.notes[i].gettime() <= msec) {
			if (music.notes[i].getType() == 1 || music.notes[i].getType() == 3) {
				PlaySoundMem(SE[1], DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			}
			combo++;
			score += 300;
			judge_number = 0;
			music.notes[i].setflag(-1);
		}
	}

	/*絶対値の取得*/
	double abs(double t) {
		if (t < 0) return (-t);
		return t;
	}
};
