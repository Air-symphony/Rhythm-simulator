#include "NoteFileReader.cpp"
#include "InputKey.cpp"
#include "InputTouch.cpp"

/*
game画面
Display _display, char filename[], bool _debugMode, bool _autoMode
*/
class GameScreen {
private:
	Display display;
	MyDrawString MyStr;
	InputKey input;//ボタン入力
	InputTouch inTouch;
	char judge_text[5][10] = { "PERFECT","GREAT", "NICE", "BAD", "MISS" };
private:
	const double StartY = 30;
	/*判定位置の座標*/
	const double judgePos_y = display.GetScreenY() - 75.0;//判定位置
	/*画面端からどのくらいのスペースを開けるか*/
	const double ScreenSideSize = display.GetScreenX() / 7.0;
	/*五つの判定位置の間隔　judgePos_x * (1,2,3,4,5)*/
	const double judgePos_x = (display.GetScreenX() - ScreenSideSize * 2) / 4.0;
	
	Music music;
	Graph ring, Line, background;
	UI StopButton;
	UI PauseWindow, Continue, Finish;
	/*0=左, 1=普通, 2=右, 3=ロング, 4=ロング先*/
	Graph noteGraph[5];
	HitEffect effect;
	int SE[2];
	double msec = 0;//経過時間(s)
	double d_msec = 0;//デバッグ用、変化値

	/*速度変更上限 0 ~ 9*/
	int speedCount = 1, Max_speedCount = 9;
	/*Maxspeed = 0.3, Minspeed = 2.0;*/
	double Maxspeed = 0.3, Minspeed = 2.0;

	/*曲が始まるまでの時間 waitTime = 3000*/
	int waitTime = 3000;

	/*(Minspeed - Maxspeed) / 9.0;*/
	double d_speed = (Minspeed - Maxspeed) / (double)Max_speedCount;
	/*何秒かけて判定位置にたどり着くか*/
	double speed;// = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

	int score = 0, combo = 0;
	int judge_number = 0;/*判定内容を保存、毎フレームで初期化*/

	/*各エリアでの、タッチIDの保存*/
	int IDList[5];
	/*フリックの始点保存*/
	int Flick_X[5];

	/*ロングノーツの場所を保持*/
	class HoldKey {
	public:
		int noteID = 0;
		int key = 0;
		void Set(int _noteID, int _key) {
			noteID = _noteID;
			key = _key;
		}
	};
	int holdKeyCount = 0;

	HoldKey holdkey[2];

	/*AutoModeの真偽*/
	bool autoMode = false, debugMode = false;

	const double PERFECTtime = 0.066;
	const double GREATtime = 0.110;
	const double NICEtime = 0.130;
	const double BADtime = 0.150;
public:
	/*
	filename = 0000.txt
	*/
	GameScreen(Display _display, char filename[], int _speed, bool _debugMode, bool _autoMode) {
		clsDx(); 
		display = _display;
		MyStr.SetMainFontSize(16);
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		ring.setDisplay(display);
		Line.setGraph(LoadGraph("materials\\Image\\note_cat.png"));
		Line.setDisplay(display);

		char _imagepath[256];
		for (int i = 1; i <= 5; i++) {
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
		
		Layout();

		for (int i = 0; i < 5; i++) IDList[i] = Flick_X[i] = -1;
		
		autoMode = _autoMode;
		debugMode = _debugMode;
		speedCount = _speed - 1;
		speed = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

		NoteFileReader file;
		music = file.SelectReadFile(filename, _debugMode);
		music.SetPos(ScreenSideSize, judgePos_x, StartY);
		strcpy_s(_imagepath, "materials\\Image\\background\\");
		strcat_s(_imagepath, music.backgroundFile);
		background.setGraph(LoadGraph(_imagepath));
		background.setDisplay(display);
		effect.SetGraph(LoadGraph("materials\\Image\\hit_circle.png"));
		effect.SetPos(ScreenSideSize, judgePos_x, judgePos_y);
		msec = 0;
		Start();
	}
private:
	/*UIの位置関係*/
	void Layout() {
		StopButton.SetUI(LoadGraph("materials\\Image\\ui\\StopButton.png"),
			display.GetScreenX() - 10, 20, 3);
		StopButton.setDisplay(display);
		PauseWindow.SetUI(LoadGraph("materials\\Image\\ui\\StopWindow.png"),
			display.GetScreenX() / 2, display.GetScreenY() / 2, 5);
		PauseWindow.setDisplay(display);
		Continue.SetUI(LoadGraph("materials\\Image\\ui\\Button.png"),
			display.GetScreenX() / 2 - 100, display.GetScreenY() / 2 + 80, 5);
		Continue.setDisplay(display);
		Continue.SetText("再開", GetColor(0, 0, 0));
		Finish.SetUI(LoadGraph("materials\\Image\\ui\\Button.png"),
			display.GetScreenX() / 2 + 100, display.GetScreenY() / 2 + 80, 5);
		Finish.setDisplay(display);
		Finish.SetText("リタイア", GetColor(0, 0, 0));
	}

	void Start() {
		SetFontSize(16);

		ClearDrawScreen();
		DrawScreen();
		MyStr.Draw_String(display.GetScreenX() / 2, display.GetScreenY() / 2, 40, "Push Space", 2);
		
		ScreenFlip();
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			if (input.PushOneframe_Decide()) {
				break;
			}
		}

		music.offset -= waitTime;
		int _waitTime = GetNowCount();
		/*曲が始まる時間　waitTime - music.offset*/
		double start_time = (double)(_waitTime - music.offset);
		/*止まり始めた時間(ms)*/
		double stop_time = 0.0;

		/*曲開始*/
		bool playMusic = false;
		/*処理が終わった小節番号の保存*/
		int start_bar_number = -1, end_bar_number = music.notes[music.notecount - 1].getbar_number();
		/*処理が終わっていないNoteID*/
		int start_noteID = 0;
		/*遠すぎるノーツは処理しない*/
		int end_noteID = music.notecount;

		bool printjudge = false;
		double printjudge_time = 0.0;//表示用
		int printjudge_number = 0;//表示用

		msec = ((double)GetNowCount() - start_time) / 1000.0;
		double Startmsec = msec;
		/*ゲーム内容*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			/*常に表示させるもの*/
			inTouch.SetTouch();
			ClearDrawScreen();
			DrawScreen();

			if (debugMode) {
				if (input.Push_LEFT()) {
					msec -= 0.030;
				}
				else if (input.Push_RIGHT()) {
					msec += 0.030;
				}
			}
			else {
				//if (GetNowCount() - _waitTime > waitTime && !playMusic) {
				if (msec - Startmsec >= waitTime / 1000.0 && !playMusic) {
					/*ゲーム開始時間の取得(ms)*/
					PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, TRUE);
					playMusic = true;
				}
				/*ゲーム進行時間(s)*/
				msec = ((double)GetNowCount() - start_time) / 1000.0;
			}

			/*一時停止*//*徐々にノーツが速くなるかも？*/
			if (stop_time <= 0 && 
				(input.PushOneframe_Stop() || inTouch.TapBox(StopButton, 10))
				) {
				stop_time = (double)GetNowCount();
				StopSoundMem(music.soundHandle);
			}

			/*timestop時*/
			if (stop_time > 0) {
				msec = (stop_time - start_time) / 1000.0;
				//MyStr.Draw_String(display.GetScreenX() / 2, 50, 40, "Pause", 2);
				PauseWindow.Draw();
				Continue.Draw(MyStr, 20, 5);
				Finish.Draw(MyStr, 20, 5);

				if (input.PushOneframe_Decide() || 
					inTouch.TapBox(Continue, 10)) {
					if (playMusic) {
						PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, FALSE);
					}
					double d_time = (double)GetNowCount();

					start_time += (d_time - stop_time);
					msec = ((double)GetNowCount() - start_time) / 1000.0;
					stop_time = 0.0;
				}
				else if (inTouch.TapBox(Finish, 10)){
					break;
				}
			}

			/*速度変更*/
			if (input.PushOneframe_DOWN()) {
				speedCount -= 1;
				if (speedCount < 0)speedCount = Max_speedCount;
			}
			else if (input.PushOneframe_UP()) {
				speedCount += 1;
				if (speedCount > Max_speedCount)speedCount = 0;
			}
			speed = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

			/*判定内容を保存、毎フレームで初期化*/
			judge_number = -1;
			int debugcount = 0;

			/*各ノーツの操作*/
			for (int i = start_noteID; i < music.notecount; i++) {
				/*for文の処理数軽減*/
				if (music.notes[i].getbar_number() <= (start_bar_number - 1)) {
					start_noteID = music.notes[i].getID();
					continue;
				}
				else if (music.notes[i].getbar_number() == music.notes[music.notecount - 1].getbar_number()) {
					end_noteID = music.notes[i].getID();
				}
				else if ((end_bar_number + 2) < music.notes[i].getbar_number()){
					end_noteID = music.notes[i].getID();
					break;
				}
				for (int j = 0; j < holdKeyCount; j++) {
					if (holdkey[j].key == 0) break;

					/*長押し中なら線を表示*/
					if (input.LongHoldKey(holdkey[j].key) ||
						inTouch.GetPressed(IDList[holdkey[j].key - 1]) || 
						autoMode
						) {
						double dt = (msec + speed) - music.notes[i].gettime();
						Line.Draw_LongLine(
							(int)(ScreenSideSize + (music.notes[holdkey[j].noteID].getend_x() - 1) * judgePos_x),
							(int)judgePos_y,
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getX(),
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getY(),
							speed, dt);
					}
					//
				}
				/*処理済みの場合*/
				if (music.notes[i].getflag() == -1) {
					/*画面下の場合は、forに含めない*/
					if (music.notes[i].getY() > display.GetScreenY()) {
						if (music.notes[i].getType() == 2 &&
							music.notes[i].getlongNoteID() < 0) {
							start_bar_number = music.notes[i].getbar_number();
							continue;
						}
					}
					/*画面内及び画面上の場合*/
					else {
						/*画面内にいる時間の場合*/
						if (music.notes[i].gettime() <= (msec + speed)) {
							double dt = (msec + speed) - (music.notes[i].gettime());
							music.notes[i].ToMove(judgePos_x, judgePos_y, dt, speed, ScreenSideSize, StartY);
						}
					}
				}
				/*画面上部で存在*/
				if (music.notes[i].getflag() == 0) {
					/*時間がきたら表示*/
					if (music.notes[i].gettime() <= (msec + speed)) {
						music.notes[i].setflag(1);
					}
				}
				/*画面内に表示されている場合*/
				if (music.notes[i].getflag() == 1) {
					end_bar_number = music.notes[i].getbar_number();
					/*画面内の場合は表示*/
					if (music.notes[i].getY() <= display.GetScreenY()) {
						double dt = (msec + speed) - music.notes[i].gettime();
						music.notes[i].ToMove(judgePos_x, judgePos_y, speed, dt, ScreenSideSize, StartY);
						
						//ロングノーツ連結がある場合
						if (music.notes[i].getlongNoteID() > 0) {
							/*画面上にいる場合*/
							if (music.notes[i].getflag() >= 0 && music.notes[i].getType() == 4) {
								Line.Draw_LongLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getlongNoteID()].getX(),
									music.notes[music.notes[i].getlongNoteID()].getY(),
									speed, dt);
							}
						}
						//フリックの連結がある場合
						if (music.notes[i].getlinkNoteID() > 0) {
							Line.Draw_FlickLine(music.notes[i].getX(), music.notes[i].getY(),
								music.notes[music.notes[i].getlinkNoteID()].getX(),
								music.notes[music.notes[i].getlinkNoteID()].getY(),
								speed, dt);
						}
						//同時押しがある場合
						if (music.notes[i].getsideNoteID() > 0) {
							//相方も表示中の場合かつ、相方のidの方が小さい場合(動き終わった状態)
							if (music.notes[music.notes[i].getsideNoteID()].getflag() == 1 &&
								i > music.notes[i].getsideNoteID()
								) {
								Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getsideNoteID()].getX(),
									music.notes[music.notes[i].getsideNoteID()].getY(),
									speed, dt);
							}
						}
						/*判定内容*/
						if (autoMode) AutoMode(i);
						else Playable(i);
					}
					/*画面下の場合は処理*/
					else {
						music.notes[i].setflag(-1);
						if (music.notes[i].getType() == 4) {
							music.notes[music.notes[i].getlongNoteID()].setflag(-1);
						}
						for (int j = 0; j < holdKeyCount; j++) {
							if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
								holdkey[j].noteID = holdkey[j].key = 0;
								holdKeyCount--;
								if (j == 0 && holdkey[1].key > 0) {
									holdkey[0].noteID = holdkey[1].noteID;
									holdkey[0].key = holdkey[1].key;
									holdkey[1].noteID = holdkey[1].key = 0;
									j--;
								}
							}
						}
						combo = 0;
						judge_number = 4;
					}
				}
				/*デバッグ用*/
				if (debugMode) {
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID type pos time (x,y)");
					if (music.notes[i].gettime() <= (msec + speed) &&
						music.notes[i].getflag() != -1) {
						debugcount++;
						int y = 96 + debugcount * 16;
						DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[i].getID());
						DrawFormatString(30, y, GetColor(255, 255, 255), " %d ", music.notes[i].getType());
						DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[i].getfirst_x());
						DrawFormatString(70, y, GetColor(255, 255, 255), "-%d ", music.notes[i].getend_x());
						DrawFormatString(90, y, GetColor(255, 255, 255), " %lf ", music.notes[i].gettime());
						DrawFormatString(180, y, GetColor(255, 255, 255), "(%d", music.notes[i].getX());
						DrawFormatString(220, y, GetColor(255, 255, 255), ",%d)", music.notes[i].getY());
					}
				}
			}
			
			/*ノーツの描画*/
			for (int i = end_noteID; i >= start_noteID; i--) {
				if (music.notes[i].getflag() == 1) {
					double dt = (msec + speed) - music.notes[i].gettime();
					if (music.notes[i].getlongNoteID() > 0 && music.notes[i].getType() == 2) {
						noteGraph[4].DrawNote(music.notes[i].getX(), music.notes[i].getY(), speed, dt);
					}
					else {
						noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getX(), music.notes[i].getY(), speed, dt);
					}
				}
			}
			/*デバッグ用*/
			if (debugMode) {
				int longnote_y = 300;
				DrawFormatString(0, longnote_y, GetColor(255, 255, 255), "holdKey : %d", holdKeyCount);
				for (int j = 0; j < 2; j++) {
					DrawFormatString(0, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), "(%d", holdkey[j].noteID);
					DrawFormatString(40, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), ",%d)", holdkey[j].key);
				}
				/*IDList とFlick_xの表示*/
				DrawString(display.GetScreenX() - 300, 30,"IDList", GetColor(255, 255, 255));
				for (int i = 0; i < 5; i++) {
					DrawFormatString(display.GetScreenX() - 300 + 40 * i, 50, GetColor(255, 255, 255), "%d, ", IDList[i]);
					DrawFormatString(display.GetScreenX() - 300 + 40 * i, 70, GetColor(255, 255, 255), "%d, ", Flick_X[i]);
				}
				/*for文の回数*/
				/*
				DrawFormatString(display.GetScreenX() - 100, 32, GetColor(255, 255, 255), "for: %d", end_noteID - start_noteID);
				DrawFormatString(display.GetScreenX() - 100, 48, GetColor(255, 255, 255), "ID : %d", start_noteID);
				DrawFormatString(display.GetScreenX() - 100, 64, GetColor(255, 255, 255), "ID : %d", end_noteID);
				DrawFormatString(display.GetScreenX() - 100, 80, GetColor(255, 255, 255), "bar: %d", start_bar_number);
				DrawFormatString(display.GetScreenX() - 100, 96, GetColor(255, 255, 255), "bar: %d", end_bar_number);
				*/
				inTouch.PrintTouch(display.GetScreenX() - 150, display.GetScreenY() - 300);
				inTouch.PrintLog(display.GetScreenX() - 150, display.GetScreenY() - 150);
			}

			/*判定を表示する必要性があったとき*/
			if (judge_number != -1) {
				printjudge_time = msec;
				printjudge_number = judge_number;
				printjudge = true;
			}
			/*判定結果の表示(1秒)*/
			if (printjudge) {
				MyStr.Draw_String(display.GetScreenX() / 2, display.GetScreenY() / 2, 30, judge_text[printjudge_number], 2);
				printjudge = ((msec - printjudge_time) <= 1.0);
			}

			ScreenFlip();// 裏画面の内容を表画面に反映させる 

			/*時間経過後、ゲーム画面の強制終了*/
			if (input.PushOneframe_Debug() || msec >= music.notes[music.notecount - 1].gettime() + 3.0) {
				break;
			}
		}
		
		StopSoundMem(music.soundHandle);
		DeleteSoundMem(music.soundHandle);
		DeleteSoundMem(SE[0]);
		DeleteSoundMem(SE[1]);
	}

	/*常に表示させるもの、スコアなど*/
	void DrawScreen() {
		background.Draw_BackGround();
		DrawFormatString(0, 0, GetColor(255, 255, 255), "Title : %s", music.title);
		DrawFormatString(display.GetScreenX() - 120, 0, GetColor(255, 255, 255), "SPEED : %d.0", (speedCount + 1));
		
		/*時間表示*/
		int _msec = (int)(msec * 1000.0);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d(s)", _msec % 1000);
		/*スコア表示*/
		DrawFormatString(0, 64, GetColor(255, 255, 255), "SCORE : %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "COMBO : %d", combo);

		if (debugMode) {
			/*1フレーム当たりの秒数*/
			DrawFormatString(100, 48, GetColor(255, 255, 255), "%d(ms)", (int)((msec - d_msec) * 1000.0));
			d_msec = msec;
		}

		/*オートモードの切り替え*/
		if (input.PushOneframe_ChangeAutoMode()) {
			autoMode = !autoMode;
		}
		if (autoMode) {
			MyStr.Draw_String(0, display.GetScreenY(), 20, "AutoMode", 7);
		}
		/*リングの表示*/
		//ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		/*判定範囲の表示*/
		DrawLine(0, (int)judgePos_y, display.GetScreenX(), (int)judgePos_y, GetColor(255, 0, 0));
		int y = (int)((judgePos_y / speed) * (PERFECTtime / 2.0));
		DrawLine(0, (int)judgePos_y - y, display.GetScreenX(), (int)judgePos_y - y, GetColor(0, 255, 0));
		DrawLine(0, (int)judgePos_y + y, display.GetScreenX(), (int)judgePos_y + y, GetColor(0, 255, 0));
		y = (int)((judgePos_y / speed) * (GREATtime / 2.0));
		DrawLine(0, (int)(judgePos_y - y), display.GetScreenX(), (int)(judgePos_y - y), GetColor(0, 0, 255));
		DrawLine(0, (int)(judgePos_y + y), display.GetScreenX(), (int)(judgePos_y + y), GetColor(0, 0, 255));
		y = (int)((judgePos_y / speed) * (NICEtime / 2.0));
		DrawLine(0, (int)judgePos_y - y, display.GetScreenX(), (int)judgePos_y - y, GetColor(255, 255, 0));
		DrawLine(0, (int)judgePos_y + y, display.GetScreenX(), (int)judgePos_y + y, GetColor(255, 255, 0));
		y = (int)((judgePos_y / speed) * (BADtime / 2.0));
		DrawLine(0, (int)judgePos_y - y, display.GetScreenX(), (int)judgePos_y - y, GetColor(255, 0, 255));
		DrawLine(0, (int)judgePos_y + y, display.GetScreenX(), (int)judgePos_y + y, GetColor(255, 0, 255));

		StopButton.Draw();
		/*エフェクトの表示*/
		effect.PrintEffect();
	}

	/*ボタン判定の記述*/
	void Playable(int i) {
		/*ロングノーツ終端の判定*/
		for (int j = 0; j < holdKeyCount; j++) {
			/*長押しを離してしまった時*/
			if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
				if (/*type 2 && 離していれば*/
					music.notes[i].getType() == 2 &&
					inTouch.GetRelease(IDList[holdkey[j].key - 1])
					) {// input.LongHoldKey(holdkey[j].key) == false)
					if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
						Judge(i, music.notes[i].getType());
					}
					else {
						judge_number = 4;
					}
					music.notes[i].setflag(-1);
					holdkey[j].noteID = holdkey[j].key = 0;
					holdKeyCount--;
					if (j == 0 && holdkey[1].key > 0) {
						holdkey[0].noteID = holdkey[1].noteID;
						holdkey[0].key = holdkey[1].key;
						holdkey[1].noteID = holdkey[1].key = 0;
						j--;
					}
				}
				/*type 1 3 && フリックが実行されていれば*/
				if ((music.notes[i].getType() == 1 || music.notes[i].getType() == 3)){
					if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
						if (PlayTouchFlick(music.notes[i].getend_x()) &&
							isFlick(music.notes[i].getend_x(), music.notes[i].getType())
							) {
							Judge(i, music.notes[i].getType());

							music.notes[i].setflag(-1);
							holdkey[j].noteID = holdkey[j].key = 0;
							holdKeyCount--;
							if (j == 0 && holdkey[1].key > 0) {
								holdkey[0].noteID = holdkey[1].noteID;
								holdkey[0].key = holdkey[1].key;
								holdkey[1].noteID = holdkey[1].key = 0;
								j--;
							}
						}
					}
					else if (inTouch.GetRelease(IDList[holdkey[j].key - 1])) {
						judge_number = 4;
						music.notes[i].setflag(-1);
						holdkey[j].noteID = holdkey[j].key = 0;
						holdKeyCount--;
						if (j == 0 && holdkey[1].key > 0) {
							holdkey[0].noteID = holdkey[1].noteID;
							holdkey[0].key = holdkey[1].key;
							holdkey[1].noteID = holdkey[1].key = 0;
							j--;
						}
					}
				}
			}
		}
		/*BAD判定内の場合*/
		if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
			int type = music.notes[i].getType();
			if (type == 2 || type == 4) {
				if (input.PushOneframe_PlayGame(music.notes[i].getend_x()) ||
					PlayTouchOneFrame(music.notes[i].getend_x())
					) {
					Judge(i, type);
					music.notes[i].setflag(-1);
					/*ロングノーツ始点の判定*/
					if (type == 4) {
						double _clear_time = abs(music.notes[i].gettime() - msec);
						if (_clear_time <= NICEtime / 2.0) {
							holdkey[holdKeyCount].Set(i, music.notes[i].getend_x());
							holdKeyCount++;
						}
						else {
							music.notes[music.notes[i].getlongNoteID()].setflag(-1);
						}
					}
				}
			}
			/*フリック判定*/
			else if (type == 1 || type == 3) {
				if (input.PushOneframe_PlayGame(music.notes[i].getend_x()) ||
					(PlayTouchFlick(music.notes[i].getend_x()) &&
					 isFlick(music.notes[i].getend_x(), type))
					) {
					Judge(i, type);
					music.notes[i].setflag(-1);
				}
			}
		}
	}

	/*タッチ判定の指定、1フレームでの判定*/
	bool PlayTouchOneFrame(int x) {
		if (inTouch.touchCount <= 0) return false;
		IDList[x - 1] = GetID_RangePlayable(x);

		return (inTouch.GetTime(IDList[x - 1]) == 1);
	}

	/*タッチ判定の指定*/
	bool PlayTouchFlick(int x) {
		if (inTouch.touchCount <= 0) return false;

		if (Flick_X[x - 1] < 0 || inTouch.GetRelease(IDList[x - 1])) {
			IDList[x - 1] = GetID_RangePlayable(x);
			Flick_X[x - 1] = inTouch.GetX(IDList[x - 1]);
		}

		return (inTouch.GetTime(IDList[x - 1]) > 0);
	}
	/*タッチ判定の指定*/
	bool isFlick(int x, int type) {
		if (inTouch.touchCount <= 0) return false;

		int FlickSize = 30;
		if (type == 1) {
			if (inTouch.GetX(IDList[x - 1]) <= Flick_X[x - 1] - FlickSize) {
				Flick_X[x - 1] = -1;
				return true;
			}
		}
		else if (type == 3) {
			if (inTouch.GetX(IDList[x - 1]) >= Flick_X[x - 1] + FlickSize) {
				Flick_X[x - 1] = -1;
				return true;
			}
		}
		return false;
	}

	/*各判定内に存在するIDの取得*/
	int GetID_RangePlayable(int x) {
		int posX, width,
			posY = (int)(display.GetScreenY() + .5),
			height = (int)(display.GetScreenY() * 0.8 + .5);
		if (x == 1) {
			posX = 0;
			width = (int)(ScreenSideSize + judgePos_x * 0.5);
		}
		else if (x == 5) {
			posX = (int)(ScreenSideSize + judgePos_x * (x - 1.5));;
			width = (int)(ScreenSideSize + judgePos_x * 0.5);
		}
		else {
			posX = (int)(ScreenSideSize + judgePos_x * (x - 1.5));
			width = (int)(judgePos_x);
		}
		return inTouch.GetID_RangeBox(posX, posY, width, height, 7);
	}

	void Judge(int i, int type) {
		/*押された時間の取得*/
		double _clear_time = abs(music.notes[i].gettime() - msec);

		if (_clear_time <= PERFECTtime / 2.0) {
			combo++;
			score += 300;
			judge_number = 0;
			effect.Hit(music.notes[i].getend_x(), judge_number);
			if (type == 1 || type == 3) {
				PlaySoundMem(SE[1], DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			}
			return;
		}
		else if (_clear_time <= GREATtime / 2.0) {
			combo++;
			score += 150;
			judge_number = 1;
			effect.Hit(music.notes[i].getend_x(), judge_number);
			return;
		}
		else if (_clear_time <= NICEtime / 2.0) {
			combo = 0;
			score += 100;
			judge_number = 2;
			effect.Hit(music.notes[i].getend_x(), judge_number);
			return;
		}
		else {
			combo = 0;
			score += 50;
			judge_number = 3;
			return;
		}
	}

	/*オートモードの内容*/
	void AutoMode(int i) {
		/*中心バーを越えたらすぐ*/
		if (music.notes[i].gettime() <= msec) {
			/*フリック音*/
			if (music.notes[i].getType() == 1 || music.notes[i].getType() == 3) {
				PlaySoundMem(SE[1], DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			}
			Judge(i, music.notes[i].getType());
			/*
			effect.Hit(music.notes[i].getend_x(), 0);
			combo++;
			score += 300;
			judge_number = 0;*/
			music.notes[i].setflag(-1);
			/*ロングノーツなら*/
			if (music.notes[i].getType() == 4) {
				holdkey[holdKeyCount].Set(i, music.notes[i].getend_x());
				holdKeyCount++;
			}
			for (int j = 0; j < holdKeyCount; j++) {
				if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
					holdkey[j].noteID = holdkey[j].key = 0;
					holdKeyCount--;
					if (j == 0 && holdkey[1].key > 0) {
						holdkey[0].noteID = holdkey[1].noteID;
						holdkey[0].key = holdkey[1].key;
						holdkey[1].noteID = holdkey[1].key = 0;
						j--;
					}
				}
			}
		}
	}

	/*絶対値の取得*/
	double abs(double t) {
		if (t < 0) return (-t);
		return t;
	}
};