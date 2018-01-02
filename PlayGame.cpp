#include "NoteFileReader.cpp"
#include "InputKey.cpp"
#include "InputTouch.cpp"

/*
game���
Display _display, char filename[], bool _debugMode, bool _autoMode
*/
class GameScreen {
private:
	Display display;
	MyDrawString MyStr;
	InputKey input;//�{�^������
	InputTouch inTouch;
	char judge_text[5][10] = { "PERFECT","GREAT", "NICE", "BAD", "MISS" };
private:
	const double StartY = 30;
	/*����ʒu�̍��W*/
	const double judgePos_y = display.GetScreenY() - 75.0;//����ʒu
	/*��ʒ[����ǂ̂��炢�̃X�y�[�X���J���邩*/
	const double ScreenSideSize = display.GetScreenX() / 7.0;
	/*�܂̔���ʒu�̊Ԋu�@judgePos_x * (1,2,3,4,5)*/
	const double judgePos_x = (display.GetScreenX() - ScreenSideSize * 2) / 4.0;
	
	Music music;
	Graph ring, Line, background;
	UI StopButton;
	UI PauseWindow, Continue, Finish;
	/*0=��, 1=����, 2=�E, 3=�����O, 4=�����O��*/
	Graph noteGraph[4];
	HitEffect effect;
	int SE[2], ButtonSE;
	double msec = 0;//�o�ߎ���(s)
	double d_msec = 0;//�f�o�b�O�p�A�ω��l

	/*���x�ύX��� 0 ~ 9*/
	int speedCount = 1, Max_speedCount = 9;
	/*Maxspeed = 0.3, Minspeed = 2.0;*/
	double Maxspeed = 0.3, Minspeed = 2.0;

	/*�Ȃ��n�܂�܂ł̎��� waitTime = 3000*/
	int waitTime = 3000;

	/*(Minspeed - Maxspeed) / 9.0;*/
	double d_speed = (Minspeed - Maxspeed) / (double)Max_speedCount;
	/*
	���b�����Ĕ���ʒu�ɂ��ǂ蒅����
	Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;
	*/
	double speed;

	int score = 0, combo = 0;
	int judge_number = 0;/*������e��ۑ��A���t���[���ŏ�����*/

	/*�e�G���A�ł́A�^�b�`ID�̕ۑ�*/
	int IDList[5];
	/*�t���b�N�̎n�_�ۑ�*/
	int Flick_X[5];

	/*�����O�m�[�c�̏ꏊ��ێ�*/
	class HoldKey {
	public:
		int noteID = 0;
		int pos = 0;
		int touchID = 0;
		void Set(int _noteID, int _pos, int _touchID) {
			noteID = _noteID;
			pos = _pos;
			touchID = _touchID;
		}
		void Receive(HoldKey key) {
			noteID = key.noteID;
			pos = key.pos;
			touchID = key.touchID;
		}
		void Reset() {
			noteID = pos = touchID = 0;
		}
	};
	int holdKeyCount = 0;

	HoldKey holdkey[2];

	/*AutoMode�̐^�U*/
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

		int NoteGraph[4];
		LoadDivGraph("materials\\Image\\RythemNote.png", 4, 2, 2,
			72, 73, NoteGraph);
		for (int i = 0; i < 4; i++) {
			noteGraph[i].setGraph(NoteGraph[i]);
			noteGraph[i].setDisplay(display);
		}
		SE[0] = LoadSoundMem("materials\\SE\\PERFECT SE.ogg");
		SE[1] = LoadSoundMem("materials\\SE\\FlickSE.ogg");
		ButtonSE = LoadSoundMem("materials\\SE\\Decision.ogg");
		ChangeVolumeSoundMem((int)(255 * 0.5), SE[0]);
		ChangeVolumeSoundMem((int)(255 * 0.5), SE[1]);
		
		Layout();

		for (int i = 0; i < 5; i++) IDList[i] = Flick_X[i] = -1;
		
		autoMode = _autoMode;
		debugMode = _debugMode;
		speedCount = _speed - 1;
		speed = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;

		NoteFileReader file;
		music = file.SelectReadFile(filename, _debugMode);
		music.SetPos(ScreenSideSize, judgePos_x, StartY);
		background.setGraph(music.backgroundHandle);
		background.setDisplay(display);
		effect.SetGraph(LoadGraph("materials\\Image\\hit_circle.png"));
		effect.SetPos(ScreenSideSize, judgePos_x, judgePos_y);
		msec = 0;
		Start();
	}

private:
	/*UI�̈ʒu�֌W*/
	void Layout() {
		StopButton.SetUI(LoadGraph("materials\\Image\\ui\\StopButton.png"),
			display.GetScreenX() - 10, 20, 3);
		StopButton.setDisplay(display);
		StopButton.SetCorrection(10);
		StopButton.SetSE(ButtonSE);
		PauseWindow.SetUI(LoadGraph("materials\\Image\\ui\\StopWindow.png"),
			display.GetScreenX() / 2, display.GetScreenY() / 2, 5);
		PauseWindow.setDisplay(display);
		Continue.SetUI(LoadGraph("materials\\Image\\ui\\Button.png"),
			display.GetScreenX() / 2 - 100, display.GetScreenY() / 2 + 80, 5);
		Continue.setDisplay(display);
		Continue.SetText("�ĊJ", GetColor(0, 0, 0));
		Continue.SetCorrection(10);
		Continue.SetSE(ButtonSE);
		Finish.SetUI(LoadGraph("materials\\Image\\ui\\Button.png"),
			display.GetScreenX() / 2 + 100, display.GetScreenY() / 2 + 80, 5);
		Finish.setDisplay(display);
		Finish.SetText("���^�C�A", GetColor(0, 0, 0));
		Finish.SetCorrection(10);
		Finish.SetSE(ButtonSE);
	}

	void Start() {
		SetFontSize(16);

		ClearDrawScreen();
		DrawScreen();
		MyStr.Draw_String(display.GetScreenX() / 2, display.GetScreenY() / 2, 40, "Push Space", 2);
		inTouch.Update();

		ScreenFlip();
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			inTouch.Update();
			if (input.PushOneframe_Decide() ||
				inTouch.PressRangeBoxOneFrame(0, 0, display.GetScreenX(), display.GetScreenY(), 1)) {
				break;
			}
		}

		music.offset -= waitTime;
		int _waitTime = GetNowCount();
		/*�Ȃ��n�܂鎞�ԁ@waitTime - music.offset*/
		double start_time = (double)(_waitTime - music.offset);
		/*�~�܂�n�߂�����(ms)*/
		double stop_time = 0.0;

		/*�ȊJ�n*/
		bool playMusic = false;
		/*�������I����Ă��Ȃ�NoteID*/
		int startIndex = 0, endIndex = music.notecount;

		bool printjudge = false;
		double printjudge_time = 0.0;//�\���p
		int printjudge_number = 0;//�\���p

		msec = ((double)GetNowCount() - start_time) / 1000.0;
		double Startmsec = msec;
		/*�Q�[�����e*/
		while (ProcessMessage() == 0 && input.ForcedTermination()) {
			/*��ɕ\�����������*/
			inTouch.Update();
			ClearDrawScreen();
			DrawScreen();

			if (debugMode) {
				if (input.Push_LEFT()) {
					msec -= 0.080;
				}
				else if (input.Push_RIGHT()) {
					msec += 0.080;
				}
			}
			else {
				//if (GetNowCount() - _waitTime > waitTime && !playMusic) {
				if (msec - Startmsec >= waitTime / 1000.0 && !playMusic) {
					/*�Q�[���J�n���Ԃ̎擾(ms)*/
					PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, TRUE);
					playMusic = true;
				}
				/*�Q�[���i�s����(s)*/
				msec = ((double)GetNowCount() - start_time) / 1000.0;
			}

			/*�ꎞ��~*//*���X�Ƀm�[�c�������Ȃ邩���H*/
			if (stop_time <= 0 && 
				(input.PushOneframe_Stop() || inTouch.ReleasedRangeBox(StopButton))
				) {
				stop_time = (double)GetNowCount();
				StopSoundMem(music.soundHandle);
			}
			StopButton.SetID(inTouch.GetID_RangeBoxOneFrame(StopButton));

			/*timestop��*/
			if (stop_time > 0) {
				msec = (stop_time - start_time) / 1000.0;
				if (input.PushOneframe_Decide() || 
					inTouch.ReleasedRangeBox(Continue)) {
					if (playMusic) {
						PlaySoundMem(music.soundHandle, DX_PLAYTYPE_BACK, FALSE);
					}
					double d_time = (double)GetNowCount();

					start_time += (d_time - stop_time);
					msec = ((double)GetNowCount() - start_time) / 1000.0;
					stop_time = 0.0;
				}
				else if (inTouch.ReleasedRangeBox(Finish)){
					break;
				}
				Continue.SetID(inTouch.GetID_RangeBoxOneFrame(Continue));
				Finish.SetID(inTouch.GetID_RangeBoxOneFrame(Finish));
			}

			/*���x�ύX*/
			if (input.PushOneframe_DOWN()) {
				speedCount -= 1;
				if (speedCount < 0)speedCount = Max_speedCount;
			}
			else if (input.PushOneframe_UP()) {
				speedCount += 1;
				if (speedCount > Max_speedCount)speedCount = 0;
			}
			speed = Maxspeed + (double)(Max_speedCount - speedCount) * d_speed;
			/*���艺�����ʊO�܂ł̈ړ�����*/
			double leaveSpeed = speed * ((display.GetScreenY() - judgePos_y) / (judgePos_y - StartY));

			/*������e��ۑ��A���t���[���ŏ�����*/
			judge_number = -1;
			int debugcount = 0;

			/*�e�m�[�c�̑���*/
			for (int index = startIndex; index < music.notecount; index++) {
				/*for���̏������y��*/
				if (music.notes[index].gettime() + leaveSpeed < msec) {
					startIndex = index;
					//continue;
				}
				if (music.notes[index].gettime() > msec + speed) {
					endIndex = index;
					if (index == music.notecount - 2) endIndex = index + 1;
					break;
				}

				for (int j = 0; j < holdKeyCount; j++) {
					if (holdkey[j].pos == 0) break;

					/*���������Ȃ����\��*/
					if (input.LongHoldKey(holdkey[j].pos) ||
						inTouch.Press(IDList[holdkey[j].pos - 1]) || 
						autoMode
						) {
						double dt = (msec + speed) - music.notes[index].gettime();
						Line.Draw_LongLine(
							(int)(ScreenSideSize + (music.notes[holdkey[j].noteID].getend_x() - 1) * judgePos_x),
							(int)judgePos_y,
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getX(),
							music.notes[music.notes[holdkey[j].noteID].getlongNoteID()].getY(),
							speed, dt);
					}
				}
				/*�����ς݂̏ꍇ*/
				if (music.notes[index].getflag() == -1) {
					/*��ʉ��̏ꍇ�́Afor�Ɋ܂߂Ȃ�*/
					if (music.notes[index].getY() > display.GetScreenY()) {
						if (music.notes[index].getType() == 2 &&
							music.notes[index].getlongNoteID() < 0) {
							continue;
						}
					}
					/*��ʓ��y�щ�ʏ�̏ꍇ*/
					else {
						/*��ʓ��ɂ��鎞�Ԃ̏ꍇ*/
						if (music.notes[index].gettime() <= (msec + speed)) {
							double dt = (msec + speed) - (music.notes[index].gettime());
							music.notes[index].ToMove(judgePos_x, judgePos_y, dt, speed, ScreenSideSize, StartY);
						}
					}
				}
				/*��ʏ㕔�ő���*/
				if (music.notes[index].getflag() == 0) {
					/*���Ԃ�������\��*/
					if (music.notes[index].gettime() <= (msec + speed)) {
						music.notes[index].setflag(1);
					}
				}
				/*��ʓ��ɕ\������Ă���ꍇ*/
				if (music.notes[index].getflag() == 1) {
					/*��ʓ��̏ꍇ�͕\��*/
					if (music.notes[index].getY() <= display.GetScreenY()) {
						double dt = (msec + speed) - music.notes[index].gettime();
						music.notes[index].ToMove(judgePos_x, judgePos_y, speed, dt, ScreenSideSize, StartY);
						
						//�����O�m�[�c�A��������ꍇ
						if (music.notes[index].getlongNoteID() > 0) {
							/*��ʏ�ɂ���ꍇ*/
							if (music.notes[index].getflag() >= 0 && music.notes[index].getType() == 4) {
								Line.Draw_LongLine(music.notes[index].getX(), music.notes[index].getY(),
									music.notes[music.notes[index].getlongNoteID()].getX(),
									music.notes[music.notes[index].getlongNoteID()].getY(),
									speed, dt);
							}
						}
						//�t���b�N�̘A��������ꍇ
						if (music.notes[index].getlinkNoteID() > 0) {
							Line.Draw_FlickLine(music.notes[index].getX(), music.notes[index].getY(),
								music.notes[music.notes[index].getlinkNoteID()].getX(),
								music.notes[music.notes[index].getlinkNoteID()].getY(),
								speed, dt);
						}
						//��������������ꍇ
						if (music.notes[index].getsideNoteID() > 0) {
							//�������\�����̏ꍇ���A������id�̕����������ꍇ(�����I��������)
							if (music.notes[music.notes[index].getsideNoteID()].getflag() == 1 &&
								index > music.notes[index].getsideNoteID()
								) {
								Line.Draw_LinkLine(music.notes[index].getX(), music.notes[index].getY(),
									music.notes[music.notes[index].getsideNoteID()].getX(),
									music.notes[music.notes[index].getsideNoteID()].getY(),
									speed, dt);
							}
						}
						/*������e*/
						if (autoMode) AutoMode(index);
						else Playable(index);
					}
					/*��ʉ��̏ꍇ�͏���*/
					else {
						music.notes[index].setflag(-1);
						if (music.notes[index].getType() == 4) {
							music.notes[music.notes[index].getlongNoteID()].setflag(-1);
						}
						for (int j = 0; j < holdKeyCount; j++) {
							if (music.notes[holdkey[j].noteID].getlongNoteID() == index) {
								holdkey[j].Reset();
								holdKeyCount--;
								if (j == 0 && holdkey[1].pos > 0) {
									holdkey[0].Receive(holdkey[1]);
									holdkey[1].Reset();
									j--;
								}
							}
						}
						combo = 0;
						judge_number = 4;
					}
				}
				/*�f�o�b�O�p*/
				if (debugMode) {
					DrawFormatString(0, 96, GetColor(255, 255, 255), "ID type pos time (x,y)");
					if (music.notes[index].gettime() <= (msec + speed) &&
						music.notes[index].getflag() != -1) {
						debugcount++;
						int y = 96 + debugcount * 16;
						DrawFormatString(0, y, GetColor(255, 255, 255), "%d", music.notes[index].getID());
						DrawFormatString(30, y, GetColor(255, 255, 255), " %d ", music.notes[index].getType());
						DrawFormatString(50, y, GetColor(255, 255, 255), " %d ", music.notes[index].getfirst_x());
						DrawFormatString(70, y, GetColor(255, 255, 255), "-%d ", music.notes[index].getend_x());
						DrawFormatString(90, y, GetColor(255, 255, 255), " %lf ", music.notes[index].gettime());
						DrawFormatString(180, y, GetColor(255, 255, 255), "(%d", music.notes[index].getX());
						DrawFormatString(220, y, GetColor(255, 255, 255), ",%d)", music.notes[index].getY());
					}
				}
			}
			
			/*�m�[�c�̕`��*/
			for (int index = endIndex; index >= startIndex; index--) {
				if (music.notes[index].getflag() == 1) {
					double dt = (msec + speed) - music.notes[index].gettime();
					if (music.notes[index].getlongNoteID() > 0 && music.notes[index].getType() == 2) {
						noteGraph[3].DrawNote(music.notes[index].getX(), music.notes[index].getY(), speed, dt);
					}
					else {
						noteGraph[music.notes[index].getType() - 1].DrawNote(music.notes[index].getX(), music.notes[index].getY(), speed, dt);
					}
				}
			}
			/*�f�o�b�O�p*/
			if (debugMode) {
				int longnote_y = 300;
				DrawFormatString(0, longnote_y, GetColor(255, 255, 255), "holdKey : %d", holdKeyCount);
				for (int j = 0; j < 2; j++) {
					DrawFormatString(0, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), "(%d,", holdkey[j].noteID);
					DrawFormatString(40, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), " %d,", holdkey[j].pos);
					DrawFormatString(80, longnote_y + (j + 1) * 16, GetColor(255, 255, 255), " %d)", holdkey[j].touchID);
				}
				/*IDList ��Flick_x�̕\��*/
				DrawString(display.GetScreenX() - 300, 30,"IDList", GetColor(255, 255, 255));
				for (int i = 0; i < 5; i++) {
					DrawFormatString(display.GetScreenX() - 300 + 40 * i, 50, GetColor(255, 255, 255), "%d, ", IDList[i]);
					DrawFormatString(display.GetScreenX() - 300 + 40 * i, 70, GetColor(255, 255, 255), "%d, ", Flick_X[i]);
				}
				//DrawFormatString(display.GetScreenX() - 100, 200, GetColor(255, 255, 255), "speed: %lf", speed);

				/*for���̉�*/
				
				DrawFormatString(display.GetScreenX() - 100, 82, GetColor(255, 255, 255), "for: %d", endIndex - startIndex);
				DrawFormatString(display.GetScreenX() - 100, 98, GetColor(255, 255, 255), "ID : %d", startIndex);
				DrawFormatString(display.GetScreenX() - 100, 114, GetColor(255, 255, 255), "ID : %d", endIndex);
				
				inTouch.PrintTouch(display.GetScreenX() - 250, display.GetScreenY() - 300);
				inTouch.PrintLog(display.GetScreenX() - 250, display.GetScreenY() - 200);
			}

			/*�����\������K�v�����������Ƃ�*/
			if (judge_number != -1) {
				printjudge_time = msec;
				printjudge_number = judge_number;
				printjudge = true;
			}
			/*���茋�ʂ̕\��(1�b)*/
			if (printjudge) {
				MyStr.Draw_String(display.GetScreenX() / 2, display.GetScreenY() / 2, 30, judge_text[printjudge_number], 2);
				printjudge = ((msec - printjudge_time) <= 1.0);
			}

			/*�|�[�Y��ʏo��*/
			if (stop_time > 0) {
				PauseWindow.Draw();
				Continue.Draw(MyStr, 20, 5);
				Finish.Draw(MyStr, 20, 5);
			}
			ScreenFlip();// ����ʂ̓��e��\��ʂɔ��f������ 

			/*���Ԍo�ߌ�A�Q�[����ʂ̋����I��*/
			if (input.PushOneframe_Debug() || msec >= music.notes[music.notecount - 1].gettime() + 3.0) {
				break;
			}
		}
		
		StopSoundMem(music.soundHandle);
		DeleteSoundMem(music.soundHandle);
		DeleteSoundMem(ButtonSE);
		DeleteSoundMem(SE[0]);
		DeleteSoundMem(SE[1]);
	}

	/*��ɕ\����������́A�X�R�A�Ȃ�*/
	void DrawScreen() {
		background.Draw_BackGround();
		DrawFormatString(0, 0, GetColor(255, 255, 255), "Title : %s", music.title);
		DrawFormatString(display.GetScreenX() - 120, 0, GetColor(255, 255, 255), "SPEED : %d.0", (speedCount + 1));
		
		/*���ԕ\��*/
		int _msec = (int)(msec * 1000.0);
		DrawFormatString(0, 48, GetColor(255, 255, 255), "%d:", _msec / 1000);
		DrawFormatString(35, 48, GetColor(255, 255, 255), "%d(s)", _msec % 1000);
		/*�X�R�A�\��*/
		DrawFormatString(0, 64, GetColor(255, 255, 255), "SCORE : %d", score);
		DrawFormatString(0, 80, GetColor(255, 255, 255), "COMBO : %d", combo);

		if (debugMode) {
			/*1�t���[��������̕b��*/
			DrawFormatString(100, 48, GetColor(255, 255, 255), "%d(ms)", (int)((msec - d_msec) * 1000.0));
			d_msec = msec;
		}

		/*�I�[�g���[�h�̐؂�ւ�*/
		if (input.PushOneframe_ChangeAutoMode()) {
			autoMode = !autoMode;
		}
		if (autoMode) {
			MyStr.Draw_String(0, display.GetScreenY(), 20, "AutoMode", 7);
		}
		/*�����O�̕\��*/
		//ring.Draw(display.GetScreenX() / 2, display.GetScreenY(), 8);
		/*����͈͂̕\��*/
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
		/*�G�t�F�N�g�̕\��*/
		effect.PrintEffect();
	}

	/*�{�^������̋L�q*/
	void Playable(int noteID) {
		/*�����O�m�[�c�I�[�̔���*/
		for (int j = 0; j < holdKeyCount; j++) {
			/*�������𗣂��Ă��܂�����*/
			if (music.notes[holdkey[j].noteID].getlongNoteID() == noteID) {
				if (/*type 2 && �����Ă����*/
					music.notes[noteID].getType() == 2 &&
					inTouch.Release(holdkey[j].touchID)
					) {// input.LongHoldKey(holdkey[j].key) == false)
					if (abs(music.notes[noteID].gettime() - msec) <= BADtime / 2.0) {
						Judge(noteID, music.notes[noteID].getType());
					}
					else {
						judge_number = 4;
					}
					music.notes[noteID].setflag(-1);
					holdkey[j].Reset();
					holdKeyCount--;
					if (j == 0 && holdkey[1].pos > 0) {
						holdkey[0].Receive(holdkey[1]);
						holdkey[1].Reset();
						j--;
					}
				}
				/*type 1 3 && �t���b�N�����s����Ă����*/
				if ((music.notes[noteID].getType() == 1 || music.notes[noteID].getType() == 3)){
					if (abs(music.notes[noteID].gettime() - msec) <= BADtime / 2.0) {
						if (PlayTouchFlick(music.notes[noteID].getend_x()) &&
							isFlick(music.notes[noteID].getend_x(), music.notes[noteID].getType())
							) {
							Judge(noteID, music.notes[noteID].getType());

							music.notes[noteID].setflag(-1);
							holdkey[j].Reset();
							holdKeyCount--;
							if (j == 0 && holdkey[1].pos > 0) {
								holdkey[0].Receive(holdkey[1]);
								holdkey[1].Reset();
								j--;
							}
						}
						else if (inTouch.Release(holdkey[j].touchID)) {
							judge_number = 4;
							music.notes[noteID].setflag(-1);
							holdkey[j].Reset();
							holdKeyCount--;
							if (j == 0 && holdkey[1].pos > 0) {
								holdkey[0].Receive(holdkey[1]);
								holdkey[1].Reset();
								j--;
							}
						}
					}
					else if (inTouch.Release(holdkey[j].touchID)) {
						judge_number = 4;
						music.notes[noteID].setflag(-1);
						holdkey[j].Reset();
						holdKeyCount--;
						if (j == 0 && holdkey[1].pos > 0) {
							holdkey[0].Receive(holdkey[1]);
							holdkey[1].Reset(); 
							j--;
						}
					}
				}
			}
		}
		/*BAD������̏ꍇ*/
		if (abs(music.notes[noteID].gettime() - msec) <= BADtime / 2.0) {
			int type = music.notes[noteID].getType();
			if (type == 2 || type == 4) {
				if (input.PushOneframe_PlayGame(music.notes[noteID].getend_x()) ||
					PlayTouchOneFrame(music.notes[noteID].getend_x())
					) {
					Judge(noteID, type);
					music.notes[noteID].setflag(-1);
					/*�����O�m�[�c�n�_�̔���*/
					if (type == 4) {
						double _clear_time = abs(music.notes[noteID].gettime() - msec);
						if (_clear_time <= NICEtime / 2.0) {
							holdkey[holdKeyCount].Set(noteID, music.notes[noteID].getend_x(), GetID_RangePlayable(music.notes[noteID].getend_x()));
							holdKeyCount++;
						}
						else {
							music.notes[music.notes[noteID].getlongNoteID()].setflag(-1);
						}
					}
				}
			}
			/*�t���b�N����*/
			else if (type == 1 || type == 3) {
				if (input.PushOneframe_PlayGame(music.notes[noteID].getend_x()) ||
					(PlayTouchFlick(music.notes[noteID].getend_x()) &&
					 isFlick(music.notes[noteID].getend_x(), type))
					) {
					Judge(noteID, type);
					music.notes[noteID].setflag(-1);
				}
			}
		}
	}

	/*�^�b�`����̎w��A1�t���[���ł̔���*/
	bool PlayTouchOneFrame(int x) {
		if (inTouch.touchCount <= 0) return false;
		IDList[x - 1] = GetID_RangePlayable(x);

		return (inTouch.GetTime(IDList[x - 1]) == 1);
	}

	/*�^�b�`����̎w��*/
	bool PlayTouchFlick(int x) {
		if (inTouch.touchCount <= 0) return false;

		if (Flick_X[x - 1] <= 0 || inTouch.Release(IDList[x - 1])) {
			IDList[x - 1] = GetID_RangePlayable(x);
			Flick_X[x - 1] = inTouch.GetX(IDList[x - 1]);
		}

		return (inTouch.GetTime(IDList[x - 1]) > 0);
	}
	/*�^�b�`����̎w��*/
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

	/*�e������ɑ��݂���ID�̎擾*/
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
		/*�����ꂽ���Ԃ̎擾*/
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

	/*�I�[�g���[�h�̓��e*/
	void AutoMode(int i) {
		/*���S�o�[���z�����炷��*/
		if (music.notes[i].gettime() <= msec) {
			/*�t���b�N��*/
			if (music.notes[i].getType() == 1 || music.notes[i].getType() == 3) {
				PlaySoundMem(SE[1], DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				PlaySoundMem(SE[0], DX_PLAYTYPE_BACK, TRUE);
			}
			Judge(i, music.notes[i].getType());
			music.notes[i].setflag(-1);
			/*�����O�m�[�c�Ȃ�*/
			if (music.notes[i].getType() == 4) {
				holdkey[holdKeyCount].Set(i, music.notes[i].getend_x(), GetID_RangePlayable(music.notes[i].getend_x()));
				holdKeyCount++;
			}
			for (int j = 0; j < holdKeyCount; j++) {
				if (music.notes[holdkey[j].noteID].getlongNoteID() == i) {
					holdkey[j].Reset();
					holdKeyCount--;
					if (j == 0 && holdkey[1].pos > 0) {
						holdkey[0].Receive(holdkey[1]);
						holdkey[1].Reset();
						j--;
					}
				}
			}
		}
	}

	/*��Βl�̎擾*/
	double abs(double t) {
		if (t < 0) return (-t);
		return t;
	}
};