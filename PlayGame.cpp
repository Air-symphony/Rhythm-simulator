#include "NoteFileReader.cpp"
#include "InputKey.cpp"
/*
game画面
Display _display, char filename[], bool _debugMode, bool _autoMode
*/
class GameScreen {
private:
	Display display;
	Music music;
	Graph ring, Line;
	/*0=左, 1=普通, 2=右, 3=ロング, 4=ロング先*/
	Graph noteGraph[5];
	HitEffect effect;
	int SE[2];
	InputKey input;//ボタン入力
	double msec = 0;//経過時間(s)
	double d_msec = 0;//デバッグ用、変化値

	/*判定位置の座標*/
	const double judgePos_y = display.GetScreenY() - 75.0;//判定位置
	/*五つの判定位置の間隔　judgePos_x * (1,2,3,4,5)*/
	const double judgePos_x = display.GetScreenX() / 6.0;
	/*速度変更上限 0 ~ 9*/
	int speedCount = 1, Max_speedCount = 9;
	/*Maxspeed = 0.3, Minspeed = 2.0;*/
	double Maxspeed = 0.3, Minspeed = 2.0;

	/*
	曲が始まるまでの時間
	waitTime = 3000;
	*/
	int waitTime = 3000;

	/*(Minspeed - Maxspeed) / 9.0;*/
	double d_speed = (Minspeed - Maxspeed) / (double)Max_speedCount;
	/*何秒かけて判定位置にたどり着くか*/
	double speed;// = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

	int score = 0, combo = 0;
	char judge_text[5][10] = { "PERFECT","GREAT", "NICE", "BAD", "MISS" };
	bool printjudge = false;
	int judge_number = 0;/*判定内容を保存、毎フレームで初期化*/
	double printjudge_time = 0.0;
	int printjudge_number = 0;

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
	bool autoMode = false;
	bool debugMode = false;

	const double PERFECTtime = 0.066;
	const double GREATtime = 0.090;
	const double NICEtime = 0.115;
	const double BADtime = 0.130;
public:
	/*
	filename = 0000.txt
	*/
	GameScreen(Display _display, char filename[], int _speed, bool _debugMode, bool _autoMode) {
		display = _display;
		ring.setGraph(LoadGraph("materials\\Image\\ring.png"));
		ring.setDisplay(display);
		Line.setDisplay(display);
		for (int i = 1; i <= 5; i++) {
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

		autoMode = _autoMode;
		debugMode = _debugMode;
		speedCount = _speed - 1;
		speed = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

		NoteFileReader file;
		music = file.SelectReadFile(filename, _debugMode);
		music.SetPos(judgePos_x, 50.0);
		effect.SetPos(judgePos_x, judgePos_y);
		msec = 0;
		Start();
	}

	void Start() {
		SetFontSize(16);

		ClearDrawScreen();
		DrawScreen();
		DrawString(display.GetScreenX() / 2 - 50, display.GetScreenY() / 2,
			"Push Space", GetColor(255, 255, 255));
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

		/*ゲーム内容*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			if (GetNowCount() - _waitTime > waitTime && !playMusic) {
				/*ゲーム開始時間の取得(ms)*/
				PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, TRUE);
				playMusic = true;
			}

			/*ゲーム進行時間(s)*/
			msec = ((double)GetNowCount() - start_time) / 1000.0;

			/*一時停止*//*徐々にノーツが速くなるかも？*/
			if (stop_time <= 0 && input.PushOneframe_Stop()) {
				stop_time = (double)GetNowCount();
				StopSoundMem(music.soundHandle);
			}

			/*timestop時*/
			if (stop_time > 0) {
				msec = (stop_time - start_time) / 1000.0;
				DrawFormatString(display.GetScreenX() - 100, 10, GetColor(255, 255, 255), "%s", "停止中");
				
				if (input.PushOneframe_Decide()) {
					PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, FALSE);
					double d_time = (double)GetNowCount();

					start_time += (d_time - stop_time);
					msec = ((double)GetNowCount() - start_time) / 1000.0;
					stop_time = 0.0;
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

			/*常に表示させるもの*/
			ClearDrawScreen();
			DrawScreen();

			/*判定内容を保存、毎フレームで初期化*/
			judge_number = -1;
			int debugcount = 0;

			/*各ノーツの操作*/
			for (int i = start_noteID; i < music.notecount; i++) {
				/*ロングノーツの保持状態*/
				for (int j = 0; j < holdKeyCount; j++) {
					if (holdkey[j].key == 0) break;

					/*長押し中なら線を表示*/
					if (input.LongHoldKey(holdkey[j].key) || autoMode) {
						Line.Draw_LinkLine(
							(int)(music.notes[holdkey[j].noteID].getend_x() * judgePos_x),
							(int)judgePos_y,
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getX(),
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getY()
						);
					}
					/*長押しを離してしまった時*/
					else{
						music.notes[holdkey[j].noteID].setflag(-1);
						holdkey[j].noteID = holdkey[j].key = 0;
						holdKeyCount--;
						if (j == 0 && holdkey[1].key > 0) {
							holdkey[0].noteID = holdkey[1].noteID;
							holdkey[0].key = holdkey[1].key;
							holdkey[1].noteID = holdkey[1].key = 0;
							j--;
						}
						judge_number = 4;
					}
				}
				
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
							music.notes[i].ToMove(judgePos_x, judgePos_y, dt, speed);
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
						music.notes[i].ToMove(judgePos_x, judgePos_y, dt, speed);
						
						//ロングノーツ連結がある場合
						if (music.notes[i].getlongNoteID() > 0) {
							/*画面上にいる場合*/
							if (music.notes[i].getflag() >= 0 && music.notes[i].getType() == 4) {
								Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getlongNoteID()].getX(),
									music.notes[music.notes[i].getlongNoteID()].getY()
								);
							}
						}
						//フリックの連結がある場合
						if (music.notes[i].getlinkNoteID() > 0) {
							Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
								music.notes[music.notes[i].getlinkNoteID()].getX(),
								music.notes[music.notes[i].getlinkNoteID()].getY()
							);
						}
						//同時押しがある場合
						if (music.notes[i].getsideNoteID() > 0) {
							//相方も表示中の場合かつ、相方のidの方が小さい場合(動き終わった状態)
							if (music.notes[music.notes[i].getsideNoteID()].getflag() == 1 &&
								i > music.notes[i].getsideNoteID()
								) {
								Line.Draw_LinkLine(music.notes[i].getX(), music.notes[i].getY(),
									music.notes[music.notes[i].getsideNoteID()].getX(),
									music.notes[music.notes[i].getsideNoteID()].getY()
								);
								/*ノーツの描画*/
								/*noteGraph[music.notes[music.notes[i].getsideNoteID()].getType() - 1]
									.DrawNote(music.notes[music.notes[i].getsideNoteID()].getX(), music.notes[music.notes[i].getsideNoteID()].getY(), speed, dt);
								if (music.notes[music.notes[i].getsideNoteID()].getlongNoteID() > 0 && 
									music.notes[music.notes[i].getsideNoteID()].getType() == 2) {
									noteGraph[4].DrawNote(music.notes[music.notes[i].getsideNoteID()].getX(), 
										music.notes[music.notes[i].getsideNoteID()].getY(), speed, dt);
								}*/
							}
						}
						/*ノーツの描画*/
						/*if (music.notes[i].getlongNoteID() > 0 && music.notes[i].getType() == 2) {
							noteGraph[4].DrawNote(music.notes[i].getX(), music.notes[i].getY(), speed, dt);
						}
						else {
							noteGraph[music.notes[i].getType() - 1].DrawNote(music.notes[i].getX(), music.notes[i].getY(), speed, dt);
						}*/

						/*判定内容*/
						if (autoMode) AutoMode(i);
						else PlayKey(i);
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
				DrawFormatString(0, longnote_y, GetColor(255, 255, 255), "holdKeyCount : %d", holdKeyCount);
				for (int j = 0; j < 2; j++) {
					DrawFormatString(0, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), "(%d", holdkey[j].noteID);
					DrawFormatString(40, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), ",%d)", holdkey[j].key);
				}
				/*for文の回数*/
				DrawFormatString(display.GetScreenX() - 100, 32, GetColor(255, 255, 255), "for: %d", end_noteID - start_noteID);
				DrawFormatString(display.GetScreenX() - 100, 48, GetColor(255, 255, 255), "ID : %d", start_noteID);
				DrawFormatString(display.GetScreenX() - 100, 64, GetColor(255, 255, 255), "ID : %d", end_noteID);
				DrawFormatString(display.GetScreenX() - 100, 80, GetColor(255, 255, 255), "bar: %d", start_bar_number);
				DrawFormatString(display.GetScreenX() - 100, 96, GetColor(255, 255, 255), "bar: %d", end_bar_number);
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
			DrawString(0, display.GetScreenY() - 20, "AutoMode", GetColor(255, 255, 255));
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

		/*エフェクトの表示*/
		effect.PrintEffect();
	}

	/*ボタン判定の記述*/
	void PlayKey(int i) {
		/*BAD判定内の場合*/
		if (abs(music.notes[i].gettime() - msec) <= BADtime / 2.0) {
			/*ロングノーツ終点の判定*/
			for (int j = 0; j < holdKeyCount; j++) {
				if (holdkey[j].key == 0) break;
				
				if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
					/*ボタンが離されていたら*/
					if (!input.LongHoldKey(holdkey[j].key)) {
						Judge(i);

						holdkey[j].noteID = holdkey[j].key = 0;
						holdKeyCount--;
						if (j == 0 && holdkey[1].key > 0) {
							holdkey[0].noteID = holdkey[1].noteID;
							holdkey[0].key = holdkey[1].key;
							holdkey[1].noteID = holdkey[1].key = 0;
						}
					}
				}
			}
			/*ノーツの判定*/
			if (input.PushOneframe_PlayGame(music.notes[i].getend_x())) {
				Judge(i);
				music.notes[i].setflag(-1);
			}
		}
	}

	void Judge(int i) {
		/*押された時間の取得*/
		double _clear_time = abs(music.notes[i].gettime() - msec);

		if (_clear_time <= GREATtime / 2.0) {
			/*ロングノーツなら*/
			if (music.notes[i].getType() == 4) {
				holdkey[holdKeyCount].Set(i, music.notes[i].getend_x());
				holdKeyCount++;
			}
		}
		if (_clear_time <= PERFECTtime / 2.0) {
			combo++;
			score += 300;
			effect.Hit(music.notes[i].getend_x());
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

	/*オートモードの内容*/
	void AutoMode(int i) {
		/*中心バーを越えたらすぐ*/
		if (music.notes[i].gettime() <= msec) {
			/*フリック音*/
			effect.Hit(music.notes[i].getend_x());
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