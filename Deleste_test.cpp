#include "DxLib.h"

int GraphX = 640;
int GraphY = 360;
//640.360
const float SPEED = 8.0;
const int note = 466 + 1;
int SingFlag[note][5]{ { 0, 0, 0, 0, 0 } };
int SingX[note] = { 0 };
int SingY[note] = { 0 };
double P[6];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow){
	SetGraphMode(GraphX, GraphY, 16);

	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1)
		return -1;

	SetDrawScreen(DX_SCREEN_BACK);

	double Bar = 0;
	int TouchX[2], TouchY[2], TouchC[6][2], TouchBC[6][2], Touch[6],
		PlaceN[6][2], Place[6];
	int Game, i, I, A, ID, x, SEp, SEf, limit, Z;
	int judge, score, combo, Mcombo, Frame, second;
	int Longjudge[6] = { 0 };

	int Graph[4][3];
	Graph[0][0] = LoadGraph("materials\\Image\\Normal.png");
	GetGraphSize(Graph[0][0], &Graph[0][1], &Graph[0][2]);
	Graph[1][0] = LoadGraph("materials\\Image\\Long.png");
	GetGraphSize(Graph[1][0], &Graph[1][1], &Graph[1][2]);
	Graph[2][0] = LoadGraph("materials\\Image\\Right.png");
	GetGraphSize(Graph[2][0], &Graph[2][1], &Graph[2][2]);
	Graph[3][0] = LoadGraph("materials\\Image\\Left.png");
	GetGraphSize(Graph[3][0], &Graph[3][1], &Graph[3][2]);

	for (int a = 0; a < 4; a++) {
		if (Graph[a][0] == -1) {
			Graph[a][1] = 10;
			Graph[a][2] = 10;
		}
	}

	SEp = LoadSoundMem("materials\\SE\\PERFECT SE.wav");
	SEf = LoadSoundMem("materials\\SE\\フリック SE.wav");

	void Judge1(int SingY, int& judge, int& combo, int& Mcombo, int SE);
	void JudgeRL(int SingY, int& judge, int& combo, int& Mcombo, int SE);
	int SetupType(int i);
	int SetupX(int i, int GraphX);
	int SetupY(int start);
	void DrawNote(int x, int y, char r);

	char Red = GetColor(255, 0, 0);
	char White = GetColor(255, 255, 255);

	Game = 0;
	while (1) {
		ClearDrawScreen();
		{
			if (Game == 0) {
				clsDx();
				printfDx("Please Touch screen");
			}
			if (Game == 0 && GetTouchInputNum() >= 1) {
				//PlaySoundFile("デレステ「Trancing Pulse」MV.mp3", DX_PLAYTYPE_BACK);
				Frame = 0;
				second = 0;
				score = 0;
				Mcombo = 0;
				combo = 0;
				limit = 1;
				//譜面内容
				PlaySoundFile("materials\\Music\\ススメ オトメ～jewel parade～ 【MV】.mp3",
					DX_PLAYTYPE_BACK);
				//PlaySoundFile("TOKIMEKIエスカレート.mp3", DX_PLAYTYPE_BACK);
				int start = int(-(SPEED * 380) + GraphY * 0.831);//380=音源frame
				for (i = 1; i < note; i++){
					SingFlag[i][SetupType(i)] = -1;
					SingX[i] = SetupX(i, GraphX);
				}
				SetupY(start);
				Bar = double(start) + 0.5;
				Game = 1;
			}
			//ゲーム開始
			else if (Game == 1) {
				//ボタン配置
				DrawBox((int)(GraphX * 0.126), (int)(GraphY * 0.831 - GraphY * 0.013),
					(int)(GraphX * 0.874), (int)(GraphY * 0.831 + GraphY * 0.013), White,
					TRUE);
				for (x = -2; x <= 2; x++) {
					DrawCircle((int)(GraphX * 0.5 + x * GraphX * 0.158), (int)(GraphY * 0.831),
						(int)(GraphX * 0.093 / 2), White, TRUE);
					DrawCircle((int)(GraphX * 0.5 + x * GraphX * 0.158), (int)(GraphY * 0.831),
						(int)(GraphX * 0.08 / 2), Red, TRUE);
				}
				Frame += 1;
				if (Frame >= 60) {
					Frame = 0;
					second += 1;
				}
				Bar += double(SPEED);
				if (Bar >= 0)
					DrawBox(0, int(Bar + 0.5) - 1, GraphX, int(Bar + 0.5) + 1,
					White, TRUE);
				if (Bar >= GraphY * 0.831)
					Bar -= P[0];

				for (i = limit; i < note; i++) {
					SingY[i] += int(SPEED);				//譜面速度
					if (SingY[i] > GraphY)
						limit = i;
				}
				for (i = limit; i < note; i++) {
					//画面内で表示
					for (A = 1; A <= 4; A++) {
						if (SingFlag[i][A] == -1 && -Graph[A][2] / 2 <= SingY[i])
							SingFlag[i][A] = 1;
					}
					if (SingY[i] == SingY[i + 1]) {
						for (A = 1; A <= 4; A++) {
							for (int B = 1; B <= 4; B++) {
								if (SingFlag[i][A] >= 1
									&& SingFlag[i + 1][B] >= 1)
									DrawBox(SingX[i], SingY[i] - 2,
									SingX[i + 1], SingY[i] + 2, White,
									TRUE);				//同時押し
							}
						}
					}
					if (SingFlag[i + 1][3] == 1 && SingFlag[i][3] >= 0) {
						if (SingX[i + 1] == SingX[i] + 100
							|| SingX[i + 1] == SingX[i] + 100) {
							for (I = 0; I < Graph[1][1]; I++)
								DrawLine(SingX[i],
								SingY[i] - Graph[1][1] / 2 + I,
								SingX[i + 1],
								SingY[i + 1] - Graph[1][1] / 2 + I,
								White, TRUE);				//連続フリック
						}
					}
					if (SingFlag[i + 1][4] == 1 && SingFlag[i][4] >= 0) {
						if (SingX[i + 1] == SingX[i] + 100
							|| SingX[i + 1] == SingX[i] + 100) {
							for (I = 0; I < Graph[1][1]; I++)
								DrawLine(SingX[i], SingY[i] + I, SingX[i + 1],
								SingY[i + 1] + I, White, TRUE);	//連続フリック
						}
					}
					if (SingFlag[i][1] == 1) {
						DrawGraph(SingX[i] - Graph[0][1] / 2,
						SingY[i] - Graph[0][2] / 2, Graph[0][0], FALSE);
					}
					else if (SingFlag[i][3] == 1) {
						DrawGraph(SingX[i] - Graph[2][1] / 2 + 8,SingY[i] - Graph[2][2] / 2, Graph[2][0], FALSE);
					}
					else if (SingFlag[i][4] == 1) {
						DrawGraph(SingX[i] - Graph[3][1] / 2 - 10,SingY[i] - Graph[3][2] / 2, Graph[3][0], FALSE);
					}
					else if (SingFlag[i][2] == 1) {
						for (I = i + 1; I < note; I++) {
							if (SingX[i] == SingX[I]) {
								if (SingFlag[I][1] == -1
									|| SingFlag[I][1] == 1) {
									SingFlag[I][1] = 0;
									SingFlag[I][2] = 2;
									DrawBox(SingX[i] - Graph[1][1] / 2,
										SingY[i],
										SingX[i] + Graph[1][1] / 2,
										SingY[I], White, TRUE);
									break;
								}
								else if (SingFlag[I][3] == -1
									|| SingFlag[I][3] == 1) {
									SingFlag[I][2] = 0;
									SingFlag[I][3] = 2;
									DrawBox(SingX[i] - Graph[1][1] / 2,
										SingY[i],
										SingX[i] + Graph[1][1] / 2,
										SingY[I], White, TRUE);
									break;
								}
								else if (SingFlag[I][4] == -1
									|| SingFlag[I][4] == 1) {
									SingFlag[I][2] = 0;
									SingFlag[I][4] = 2;
									DrawBox(SingX[i] - Graph[1][1] / 2,
										SingY[i],
										SingX[i] + Graph[1][1] / 2,
										SingY[I], White, TRUE);
									break;
								}
							}
						}
						DrawGraph(SingX[i] - Graph[1][1] / 2,
							SingY[i] - Graph[1][2] / 2, Graph[1][0], FALSE);
					}
					for (A = 2; A <= 4; A++) {
						if (SingFlag[i][A] == 2) {
							for (int L = i; 0 < L; L--) {
								if (SingFlag[L][2] == 1
									&& SingX[i] == SingX[L]) {
									if (A == 2)
										DrawGraph(SingX[i] - Graph[1][1] / 2,
										SingY[i] - Graph[1][2] / 2,
										Graph[1][0], FALSE);
									else if (A == 3)
										DrawGraph(
										SingX[i] - Graph[2][1] / 2 + 8,
										SingY[i] - Graph[2][2] / 2,
										Graph[2][0], FALSE);
									else if (A == 4)
										DrawGraph(
										SingX[i] - Graph[3][1] / 2 - 10,
										SingY[i] - Graph[3][2] / 2,
										Graph[3][0], FALSE);
									break;
								}
							}
						}
					}
				}

				//タッチした時
				for (ID = 0; ID < GetTouchInputNum(); ID++)
					GetTouchInput(ID, &TouchX[ID], &TouchY[ID], NULL, NULL);//座標習得
				if (GetTouchInputNum() == 0) {
					for (x = 1; x <= 5; x++) {
						for (ID = 0; ID < 2; ID++) {
							TouchC[x][ID] = 0;
							TouchX[ID] = 0;
							TouchY[ID] = 0;
							PlaceN[x][ID] = 0;
						}
					}
				}
				if (GetTouchInputNum() == 1) {
					for (x = 1; x < 6; x++) {
						TouchX[1] = 0;
						TouchY[1] = 0;
						TouchC[x][0] += TouchC[x][1];
						TouchC[x][1] = 0;
					}
				}
				for (ID = 0; ID < 2; ID++) {
					for (x = 1; x <= 5; x++) {
						TouchBC[x][ID] = TouchC[x][ID];
						if (((GraphX * 0.5 + (x - 3) * GraphX * 0.158)
							- TouchX[ID])
							* ((GraphX * 0.5 + (x - 3) * GraphX * 0.158)
							- TouchX[ID])
							+ (310 - TouchY[ID]) * (310 - TouchY[ID])
							<= 35 * 35) {				//タッチ判定範囲
							TouchC[x][ID]++;				//場所でのカウント
							PlaceN[x][ID] = x - 3;				//場所情報
						}
						if (TouchBC[x][ID] == TouchC[x][ID]) {
							TouchC[x][ID] = 0;
							PlaceN[x][ID] = 0;
						}
					}
				}
				for (x = 1; x <= 5; x++) {
					Place[x] = PlaceN[x][0] + PlaceN[x][1];
					Touch[x] = TouchC[x][0] + TouchC[x][1];
				}

				clsDx();				//printfDxの更新
				printfDx("Time:%03d.%02d \nscore:%d \n", second, Frame, score);
				printfDx("Mcombo:%d \ncombo:%d \n", Mcombo, combo);
				for (int c = limit; c < note; c++) {
					for (int d = 0; d < 5; d++) {
						printfDx("[%d]", SingFlag[c][d]);
					}
					printfDx("%d\n", SingY[c]);
					if (SingY[c] < -300) {
						break;
					}
				}
				//for (x = 1; x <= 5; x++)printfDx("Touch[%d]:%d \n", x, Touch[x]);

				//判定と処理
				for (i = limit; i < note; i++) {
					if (SingY[i] <= GraphY * 0.5)
						break;
					for (x = 1; x <= 5; x++) {
						if ((GraphX * 0.5 + Place[x] * GraphX * 0.158)
							== SingX[i] && GraphY * 0.665 <= SingY[i]
							&& SingY[i] <= GraphY * 0.914) {

							//普通　ロング
							if (Touch[x] == 1) {
								for (A = 1; A <= 2; A++) {
									if (SingFlag[i][A] == 1) {
										SingFlag[i][A] = 0;
										Judge1(SingY[i], judge, combo, Mcombo,
											SEp);
										if (judge == 1 || judge == 2
											|| judge == 3) {
											Longjudge[x] = 1;
										}
									}
								}
							}
							//右フリック
							if (SingFlag[i][3] == 1 || SingFlag[i][3] == 2) {
								for (ID = 0; ID < 2; ID++) {
									if (Touch[x] >= 1
										&& SingX[i] + 5 <= TouchX[ID]
										&& TouchX[ID] <= SingX[i] + 35
										&& SingY[i] - 5 <= TouchY[ID]
										&& TouchY[ID] <= SingY[i] + 50) {
										SingFlag[i][3] = 0;
										JudgeRL(SingY[i] + 15, judge, combo,
											Mcombo, SEf);
									}
								}
							}
							//左フリック
							else if (SingFlag[i][4] == 1
								|| SingFlag[i][4] == 2) {
								for (ID = 0; ID < 2; ID++) {
									if (Touch[x] >= 1
										&& SingX[i] - 40 <= TouchX[ID]
										&& TouchX[ID] <= SingX[i] - 20
										&& SingY[i] - 20 <= TouchY[ID]
										&& TouchY[ID] <= SingY[i] + 50) {
										SingFlag[i][4] = 0;
										JudgeRL(SingY[i] + 15, judge, combo,
											Mcombo, SEf);
									}
								}
							}
						}
						//ロング中
						if (Longjudge[x] >= 1
							&& (GraphX * 0.5 + Place[x] * GraphX * 0.158)
							== SingX[i]) {
							Z = 0;
							for (I = i + 1; I < note; I++) {
								if (Z == 1) {
									break;
								}
								for (A = 2; A <= 4; A++) {
									if (SingFlag[I][A] == 2
										&& SingFlag[i][2] == 0
										&& SingX[i] == SingX[I]) {
										Z = 1;
										DrawBox(SingX[i] - Graph[1][1] / 2, 310,
											SingX[i] + Graph[1][1] / 2,
											SingY[I], White, TRUE);
										if (A == 2)
											DrawGraph(
											SingX[I] - Graph[1][1] / 2,
											SingY[I] - Graph[1][2] / 2,
											Graph[1][0], FALSE);
										else if (A == 3)
											DrawGraph(
											SingX[I] - Graph[2][1] / 2
											+ 8,
											SingY[I] - Graph[2][2] / 2,
											Graph[2][0], FALSE);
										else if (A == 4)
											DrawGraph(
											SingX[I] - Graph[3][1] / 2
											- 10,
											SingY[I] - Graph[3][2] / 2,
											Graph[3][0], FALSE);
										break;
									}
								}
							}
							//ロング判定
							for (I = i + 1; I < note; I++) {
								if (Longjudge[x] == 0)
									break;
								if (Longjudge[x] == 2 && SingX[i] == SingX[I]) {
									if (SingFlag[I][2] == 2 && Touch[x] == 0) {	//成功時
										SingFlag[I][2] = 0;
										Judge1(SingY[I], judge, combo, Mcombo,
											SEp);
										Longjudge[x] = 0;
										break;
									}
									for (A = 2; A <= 4; A++) {
										if (SingFlag[I][A] == 2
											&& GraphY < SingY[I])
											Longjudge[x] = 0;	//画面外
									}
								}
								for (A = 2; A <= 4; A++) {
									if (Longjudge[x] == 1 && SingFlag[I][A] == 2
										&& SingX[i] == SingX[I]) {
										if (Touch[x] >= 1) {
											Longjudge[x] = 1;
											SingY[i] -= int(SPEED);		//維持状態
										}
										else if (Touch[x] < 1)
											Longjudge[x] = 0;		//維持失敗
										if (Longjudge[x] == 1
											&& GraphY * 0.748 <= SingY[I]
											&& SingY[I] <= GraphY * 0.95)
											Longjudge[x] = 2;		//維持成功
										if (GraphY < SingY[I])
											Longjudge[x] = 0;		//画面外
									}
									//失敗時の判定
									if (Longjudge[x] == 0) {
										SingFlag[I][A] = 0;
										judge = 5;
										combo = 0;
										break;
									}
								}
							}
						}
					}
					//画面外の判定
					if (360 <= SingY[i] && SingY[i] <= 460 - Graph[0][2] / 2) {
						if (SingFlag[i][2] == 1) {
							for (I = i + 1; I < note; I++) {
								for (int A = 2; A <= 4; A++) {
									if (SingFlag[I][A] == 2
										&& SingX[i] == SingX[I]) {
										SingFlag[I][A] = 0;
										judge = 5;
										combo = 0;
										break;
									}
								}
							}
						}
						for (A = 1; A <= 4; A++) {
							if (SingFlag[i][A] == 1) {
								SingFlag[i][A] = 0;
								combo = 0;
								judge = 5;
							}
						}
					}
				}
				//判定表示
				if (judge >= 1) {
					if (judge == 1)
						DrawString(310, 260, "PERFECT", White);
					else if (judge == 2)
						DrawString(310, 260, "GREAT", White);
					else if (judge == 3)
						DrawString(310, 260, "NICE", White);
					else if (judge == 4)
						DrawString(310, 260, "BAD", White);
					else if (judge == 5)
						DrawString(310, 260, "MISS", White);
				}
			}
		}
		ScreenFlip();
		if (ProcessMessage() < 0)
			break;
		if (CheckHitKey(KEY_INPUT_ESCAPE))
			break;
	}
	DxLib_End();
	return 0;
}

void Judge1(int SingY, int& judge, int& combo, int& Mcombo, int SEp) {
	if (265 <= SingY && SingY <= 355) {
		PlaySoundMem(SEp, DX_PLAYTYPE_BACK);
		judge = 1;
		combo += 1;
	}
	else if (260 <= SingY && SingY <= 360) {
		judge = 2;
		combo += 1;
	}
	else if (255 <= SingY && SingY <= 365) {
		judge = 3;
		combo = 0;
	}
	else if (250 <= SingY && SingY <= 370) {
		judge = 4;
		combo = 0;
	}
	else if (245 <= SingY && SingY <= 375) {
		judge = 5;
		combo = 0;
	}
	if (Mcombo <= combo)
		Mcombo = combo;
}
void JudgeRL(int SingY, int& judge, int& combo, int& Mcombo, int SEf) {
	if (245 <= SingY && SingY <= 375) {
		PlaySoundMem(SEf, DX_PLAYTYPE_BACK);
		judge = 1;
		combo += 1;
	}
	else if (240 <= SingY && SingY <= 380) {
		judge = 2;
		combo += 1;
	}
	else if (235 <= SingY && SingY <= 385) {
		judge = 3;
		combo = 0;
	}
	else if (230 <= SingY && SingY <= 390) {
		judge = 4;
		combo = 0;
	}
	else if (225 <= SingY && SingY <= 395) {
		judge = 5;
		combo = 0;
	}
	if (Mcombo <= combo)
		Mcombo = combo;
}
int SetupType(int i){
	int A;
	//普通:1、ロング:2、右フリック:3、左フリック:4
	if (i == 5 || i == 7 || i == 9 || i == 10 || i == 17
		|| i == 19 || i == 22 || i == 36 || i == 37
		|| i == 41 || i == 43 || i == 50 || i == 66
		|| i == 74 || i == 78 || i == 93 || i == 101
		|| i == 106 || i == 110 || i == 114 || i == 121
		|| i == 123 || i == 125 || i == 127 || i == 129
		|| i == 130 || i == 135 || i == 139 || i == 143
		|| i == 150 || i == 152 || i == 154 || i == 156
		|| i == 158 || i == 159 || i == 179 || i == 201
		|| i == 202 || i == 211 || i == 237 || i == 240
		|| i == 246 || i == 260 || i == 266 || i == 286
		|| i == 300 || i == 311 || i == 312 || i == 316
		|| i == 317 || i == 320 || i == 322 || i == 323
		|| i == 326 || i == 327 || i == 330 || i == 332
		|| i == 334 || i == 336 || i == 337 || i == 342
		|| i == 343 || i == 346 || i == 347 || i == 350
		|| i == 351 || i == 354 || i == 355 || i == 358
		|| i == 361 || i == 363 || i == 367 || i == 369
		|| i == 373 || i == 387 || i == 404 || i == 405
		|| i == 414 || i == 420 || i == 425 || i == 429
		|| i == 433 || i == 457 || i == 460 || i == 463
		|| i == 464)
		A = 2;
	else if (i == 6 || i == 48 || i == 49 || i == 102
		|| i == 132 || i == 161 || i == 187 || i == 188
		|| i == 232 || i == 233 || i == 239 || i == 274
		|| i == 275 || i == 305 || i == 306 || i == 307
		|| i == 308 || i == 309 || i == 319 || i == 328
		|| i == 331 || i == 335 || i == 345 || i == 348
		|| i == 349 || i == 426 || i == 436 || i == 437
		|| i == 440 || i == 441 || i == 444 || i == 445
		|| i == 446 || i == 447 || i == 466)
		A = 3;
	else if (i == 103 || i == 131 || i == 160 || i == 162
		|| i == 163 || i == 261 || i == 262 || i == 264
		|| i == 265 || i == 268 || i == 269 || i == 271
		|| i == 272 || i == 314 || i == 318 || i == 329
		|| i == 344 || i == 434 || i == 435 || i == 438
		|| i == 439 || i == 442 || i == 443 || i == 465)
		A = 4;
	else
		A = 1;
	return A;
}
int SetupX(int i, int GraphX){//1-5の左右配置
	int x;
		if (i == 2 || i == 4 || i == 9 || i == 11 || i == 19
			|| i == 23 || i == 27 || i == 29 || i == 32
			|| i == 36 || i == 39 || i == 40 || i == 50
			|| i == 64 || i == 78 || i == 91 || i == 94
			|| i == 96 || i == 99 || i == 104 || i == 105
			|| i == 108 || i == 109 || i == 112 || i == 113
			|| i == 116 || i == 119 || i == 120 || i == 123
			|| i == 124 || i == 127 || i == 128 || i == 130
			|| i == 131 || i == 159 || i == 160 || i == 162
			|| i == 166 || i == 169 || i == 172 || i == 174
			|| i == 183 || i == 191 || i == 199 || i == 205
			|| i == 207 || i == 209 || i == 221 || i == 223
			|| i == 231 || i == 243 || i == 249 || i == 252
			|| i == 255 || i == 258 || i == 261 || i == 264
			|| i == 268 || i == 271 || i == 278 || i == 280
			|| i == 289 || i == 299 || i == 300 || i == 303
			|| i == 306 || i == 310 || i == 311 || i == 314
			|| i == 315 || i == 317 || i == 318 || i == 323
			|| i == 325 || i == 327 || i == 329 || i == 337
			|| i == 339 || i == 340 || i == 342 || i == 344
			|| i == 363 || i == 366 || i == 368 || i == 374
			|| i == 378 || i == 381 || i == 385 || i == 402
			|| i == 411 || i == 418 || i == 422 || i == 424
			|| i == 427 || i == 429 || i == 434 || i == 437
			|| i == 438 || i == 441 || i == 442 || i == 445
			|| i == 450 || i == 452 || i == 455 || i == 457
			|| i == 459)
			x = 2;

		else if (i == 3 || i == 5 || i == 6 || i == 14 || i == 28
			|| i == 30 || i == 42 || i == 51 || i == 52
			|| i == 54 || i == 55 || i == 57 || i == 58
			|| i == 65 || i == 72 || i == 74 || i == 75
			|| i == 77 || i == 79 || i == 81 || i == 82
			|| i == 84 || i == 85 || i == 87 || i == 92
			|| i == 95 || i == 97 || i == 165 || i == 168
			|| i == 170 || i == 171 || i == 175 || i == 182
			|| i == 185 || i == 190 || i == 194 || i == 195
			|| i == 197 || i == 202 || i == 204 || i == 234
			|| i == 236 || i == 238 || i == 240 || i == 241
			|| i == 246 || i == 248 || i == 253 || i == 257
			|| i == 259 || i == 273 || i == 277 || i == 283
			|| i == 286 || i == 288 || i == 293 || i == 298
			|| i == 301 || i == 307 || i == 330 || i == 331
			|| i == 334 || i == 335 || i == 351 || i == 353
			|| i == 355 || i == 356 || i == 359 || i == 362
			|| i == 380 || i == 387 || i == 389 || i == 390
			|| i == 393 || i == 394 || i == 397 || i == 398
			|| i == 401 || i == 404 || i == 406 || i == 423
			|| i == 425 || i == 426 || i == 451 || i == 453
			|| i == 461)
			x = 3;

		else if (i == 13 || i == 17 || i == 20 || i == 21 || i == 22
			|| i == 24 || i == 31 || i == 35 || i == 46
			|| i == 48 || i == 53 || i == 56 || i == 59
			|| i == 60 || i == 62 || i == 63 || i == 67
			|| i == 69 || i == 71 || i == 73 || i == 80
			|| i == 83 || i == 86 || i == 88 || i == 90
			|| i == 98 || i == 100 || i == 106 || i == 107
			|| i == 110 || i == 111 || i == 114 || i == 115
			|| i == 117 || i == 118 || i == 121 || i == 122
			|| i == 125 || i == 126 || i == 129 || i == 132
			|| i == 158 || i == 161 || i == 164 || i == 167
			|| i == 176 || i == 178 || i == 184 || i == 187
			|| i == 192 || i == 200 || i == 206 || i == 208
			|| i == 210 || i == 212 || i == 214 || i == 216
			|| i == 218 || i == 222 || i == 224 || i == 232
			|| i == 235 || i == 237 || i == 239 || i == 244
			|| i == 254 || i == 260 || i == 263 || i == 266
			|| i == 267 || i == 274 || i == 276 || i == 281
			|| i == 284 || i == 291 || i == 294 || i == 297
			|| i == 302 || i == 308 || i == 312 || i == 313
			|| i == 316 || i == 319 || i == 326 || i == 328
			|| i == 336 || i == 338 || i == 341 || i == 343
			|| i == 345 || i == 365 || i == 367 || i == 370
			|| i == 371 || i == 379 || i == 382 || i == 386
			|| i == 403 || i == 410 || i == 414 || i == 415
			|| i == 432 || i == 433 || i == 446 || i == 454
			|| i == 458 || i == 464 || i == 466)
			x = 4;

		else if (i == 12 || i == 15 || i == 16 || i == 26 || i == 33
			|| i == 37 || i == 38 || i == 41 || i == 44
			|| i == 45 || i == 49 || i == 61 || i == 68
			|| i == 70 || i == 89 || i == 101 || i == 102
			|| i == 135 || i == 136 || i == 139 || i == 140
			|| i == 143 || i == 144 || i == 146 || i == 147
			|| i == 150 || i == 151 || i == 154 || i == 155
			|| i == 177 || i == 186 || i == 188 || i == 196
			|| i == 198 || i == 213 || i == 215 || i == 217
			|| i == 220 || i == 226 || i == 228 || i == 230
			|| i == 233 || i == 242 || i == 245 || i == 251
			|| i == 270 || i == 275 || i == 282 || i == 285
			|| i == 292 || i == 295 || i == 304 || i == 309
			|| i == 322 || i == 324 || i == 346 || i == 349
			|| i == 354 || i == 357 || i == 361 || i == 364
			|| i == 373 || i == 375 || i == 377 || i == 384
			|| i == 391 || i == 395 || i == 399 || i == 405
			|| i == 407 || i == 408 || i == 412 || i == 417
			|| i == 419 || i == 420 || i == 430 || i == 431
			|| i == 447 || i == 449 || i == 460 || i == 462)
			x = 5;
		else
			x = 1;
	//tokimeki
	/*int xx;
	note =256
	int	yy = 50;
	int zz = 50;
	int countz[2] = { 0 }; int county[2] = { 0 };
	for (i = 1; i < note; i++){
	xx = GetRand(99);
	if (i % 2 == 1){
	if (xx < zz){
	zz = 17;
	x = 1;
	countz[0]++;
	countz[1] = 0;
	if (countz[0] == 3){
	countz[0] = 0;
	x = 2;
	}
	}
	else if (xx >= zz){
	zz = 83;
	x = 2;
	countz[1]++;
	countz[0] = 0;
	if (countz[1] == 3){
	countz[1] = 0;
	x = 1;
	}
	}
	}
	else if (i % 2 == 0){
	if (xx < yy){
	yy = 17;
	x = 4;
	county[0]++;
	county[1] = 0;
	if (county[0] == 3){
	county[0] = 0;
	x = 5;
	}
	}
	else if (xx >= yy){
	yy = 83;
	x = 5;
	county[1]++;
	county[0] = 0;
	if (county[1] == 3){
	county[1] = 0;
	x = 4;
	}
	}
	}
	for (int v = 1; v < 4; v++){
	if (64 * v - 15 <= i&&i <= 64 * v)x = 3;
	if (64 * v - 3 <= i&&i <= 64 * v - 2)x = 4;
	if (64 * v - 5 <= i&&i <= 64 * v - 4)x = 2;
	}
	if (i == 241 || i == 242)x = 4;
	if (i == 243 || i == 244)x = 2;
	if (i == 245 || i == 246)x = 4;
	if (i == 247 || i == 248)x = 2;
	if (i == 249 || i == 250)x = 5;
	if (i == 251 || i == 252)x = 4;
	if (i == 253 || i == 254)x = 2;
	if (i == 255 || i == 256)x = 1;

	SingX[i] = GraphX * 0.5 + (x - 3) * GraphX * 0.158;
	}*/
	return int(GraphX * 0.5 + (x - 3) * GraphX * 0.158);
}
int SetupY(int start){
	float BPM = 130.0; //ススメ　130.0 TOKIMEKI 140.0
	//double P[6];
	int i;

	//縦配置
	//0123
	P[5] = 1.0 / (BPM / 60.0);				//一拍にP[5]秒かかる
	P[1] = P[5] * 60.0 * SPEED;				//四分音符、1秒 = 60frame
	P[2] = P[1] / 2;				//八分音符
	P[3] = P[1] / 4;				//十六分音符
	P[4] = P[1] / 8;				//フリック代用
	P[0] = P[1] * 4;				//一小節分
	//for (i = 1; i <= 256; i++)SingY[i] = start - int(P[3] * (i - 1) + 0.5);//tokimeki

	//例:for (i = 1; i <= 5; i++)SingY[i] = start - int(P[0] * (a - 1) + P[1] * (b - 1) + P[2] * (c - 1) + P[3] * (d - 1) + 0.5);
	//左一列目
	//SingY[0] = start - 300;
	for (i = 1; i <= 5; i++)
		SingY[i] = start
		- int(P[0] * (1 - 1) + P[2] * (i - 1) + 0.5);
	for (i = 6; i <= 7; i++)
		SingY[i] = start
		- int(P[0] * (1 - 1) + P[2] * ((i + 1) - 1) + 0.5);
	for (i = 8; i <= 9; i++)
		SingY[i] = start
		- int(P[0] * (2 - 1) + P[2] * (2 - 1) + 0.5);
	for (i = 10; i <= 11; i++)
		SingY[i] = start
		- int(P[0] * (2 - 1) + P[2] * (4 - 1) + 0.5);
	for (i = 12; i <= 14; i++)
		SingY[i] = start
		- int(P[0] * (2 - 1) + P[1] * (i - 10) + 0.5);
	for (i = 15; i <= 17; i++)
		SingY[i] = start
		- int(P[0] * (3 - 1) + P[2] * (i - 14) + 0.5);
	for (i = 18; i <= 19; i++)
		SingY[i] = start
		- int(P[0] * (3 - 1) + P[1] * (i - 16) + 0.5);
	for (i = 20; i <= 22; i++)
		SingY[i] = start
		- int(
		P[0] * (4 - 1) + P[1] * (i - 20)
		+ P[2] * (0 - 1) + 0.5);
	for (i = 23; i <= 24; i++)
		SingY[i] = start
		- int(P[0] * (4 - 1) + P[1] * (4 - 1) + 0.5);
	for (i = 25; i <= 26; i++)
		SingY[i] = start - int(P[0] * (5 - 1) + 0.5);
	for (i = 27; i <= 30; i++)
		SingY[i] = start
		- int(P[0] * (5 - 1) + P[2] * (i - 26) + 0.5);
	SingY[31] = start - int(P[0] * (5 - 1) + P[1] * (4 - 1) + 0.5);
	for (i = 32; i <= 33; i++)
		SingY[i] = start
		- int(P[0] * (5 - 1) + P[2] * (8 - 1) + 0.5);
	for (i = 34; i <= 35; i++)
		SingY[i] = start
		- int(P[0] * (6 - 1) + P[2] * (2 - 1) + 0.5);
	for (i = 36; i <= 37; i++)
		SingY[i] = start
		- int(P[0] * (6 - 1) + P[2] * (4 - 1) + 0.5);
	for (i = 38; i <= 39; i++)
		SingY[i] = start
		- int(P[0] * (6 - 1) + P[1] * (i - 36) + 0.5);
	for (i = 40; i <= 41; i++)
		SingY[i] = start
		- int(P[0] * (6 - 1) + P[2] * (8 - 1) + 0.5);
	SingY[42] = start - int(P[0] * (7 - 1) + P[1] * (4 - 1) + 0.5);
	for (i = 43; i <= 44; i++)
		SingY[i] = start
		- int(P[0] * (7 - 1) + P[2] * (8 - 1) + 0.5);
	for (i = 45; i <= 47; i++)
		SingY[i] = start
		- int(P[0] * (8 - 1) + P[1] * (i - 44) + 0.5);
	for (i = 48; i <= 49; i++)
		SingY[i] = start
		- int(
		P[0] * (8 - 1) + P[1] * (4 - 1)
		+ P[3] * (i - 48) + 0.5);	//連続フリック
	for (i = 50; i <= 51; i++)
		SingY[i] = start
		- int(P[0] * (8 - 1) + P[2] * (8 - 1) + 0.5);
	//左二列目
	for (i = 52; i <= 53; i++)
		SingY[i] = start
		- int(P[0] * (9 - 1) + P[2] * (i - 52) + 0.5);
	for (i = 54; i <= 56; i++)
		SingY[i] = start
		- int(
		P[0] * (9 - 1) + P[1] * (2 - 1)
		+ P[2] * (i - 53) + 0.5);
	for (i = 57; i <= 59; i++)
		SingY[i] = start
		- int(
		P[0] * (9 - 1) + P[1] * (4 - 1)
		+ P[2] * (i - 56) + 0.5);
	for (i = 60; i <= 63; i++)
		SingY[i] = start
		- int(
		P[0] * (10 - 1) + P[1] * (2 - 1)
		+ P[2] * (i - 59) + 0.5);
	for (i = 64; i <= 65; i++)
		SingY[i] = start
		- int(
		P[0] * (10 - 1) + P[1] * (4 - 1)
		+ P[2] * (2 - 1) + 0.5);
	for (i = 66; i <= 67; i++)
		SingY[i] = start - int(P[0] * (11 - 1) + 0.5);
	for (i = 68; i <= 74; i++)
		SingY[i] = start
		- int(P[0] * (11 - 1) + P[2] * (i - 67) + 0.5);
	SingY[75] = start - int(P[0] * (12 - 1) + P[1] * (4 - 1) + 0.5);
	for (i = 76; i <= 77; i++)
		SingY[i] = start
		- int(
		P[0] * (12 - 1) + P[1] * (4 - 1)
		+ P[2] * (2 - 1) + 0.5);
	for (i = 78; i <= 79; i++)
		SingY[i] = start - int(P[0] * (13 - 1) + 0.5);
	SingY[80] = start - int(P[0] * (13 - 1) + P[2] * (2 - 1) + 0.5);
	for (i = 81; i <= 83; i++)
		SingY[i] = start
		- int(
		P[0] * (13 - 1) + P[1] * (2 - 1)
		+ P[2] * (i - 80) + 0.5);
	for (i = 84; i <= 86; i++)
		SingY[i] = start
		- int(
		P[0] * (13 - 1) + P[1] * (4 - 1)
		+ P[2] * (i - 83) + 0.5);
	for (i = 87; i <= 90; i++)
		SingY[i] = start
		- int(
		P[0] * (14 - 1) + P[1] * (2 - 1)
		+ P[2] * (i - 86) + 0.5);
	for (i = 91; i <= 92; i++)
		SingY[i] = start
		- int(
		P[0] * (14 - 1) + P[1] * (4 - 1)
		+ P[2] * (2 - 1) + 0.5);
	for (i = 93; i <= 94; i++)
		SingY[i] = start - int(P[0] * (15 - 1) + 0.5);
	for (i = 95; i <= 101; i++)
		SingY[i] = start
		- int(P[0] * (15 - 1) + P[2] * (i - 94) + 0.5);
	for (i = 102; i <= 103; i++)
		SingY[i] = start
		- int(
		P[0] * (16 - 1) + P[1] * (3 - 1)
		+ P[2] * (i - 101) + 0.5);
	//三列目
	for (i = 104; i <= 128; i++)
		SingY[i] = start
		- int(P[0] * (17 - 1) + P[2] * (i - 105) + 0.5);
	for (i = 129; i <= 132; i++)
		SingY[i] = start
		- int(P[0] * (20 - 1) + P[1] * (i - 129) + 0.5);
	//五列目
	for (i = 133; i <= 157; i++)
		SingY[i] = start
		- int(P[0] * (21 - 1) + P[2] * (i - 134) + 0.5);
	for (i = 158; i <= 161; i++)
		SingY[i] = start
		- int(P[0] * (24 - 1) + P[1] * (i - 158) + 0.5);
	//七列目
	for (i = 162; i <= 163; i++)
		SingY[i] = start
		- int(P[0] * (25 - 1) + P[3] * (i - 162) + 0.5);//連続フリック
	for (i = 164; i <= 168; i++)
		SingY[i] = start
		- int(P[0] * (25 - 1) + P[1] * (i - 163) + 0.5);
	for (i = 169; i <= 170; i++)
		SingY[i] = start
		- int(
		P[0] * (26 - 1) + P[1] * (i - 168)
		+ P[2] * (1 - 0) + 0.5);
	for (i = 171; i <= 179; i++)
		SingY[i] = start
		- int(P[0] * (27 - 1) + P[2] * (i - 172) + 0.5);
	SingY[180] = start
		- int(P[0] * (28 - 1) + P[1] * (2 - 0) + 0.5);
	for (i = 181; i <= 182; i++)
		SingY[i] = start
		- int(
		P[0] * (28 - 1) + P[1] * (2) + P[2] * (1)
		+ 0.5);
	for (i = 183; i <= 184; i++)
		SingY[i] = start - int(P[0] * (28 - 1) + P[1] * (3) + 0.5);
	for (i = 185; i <= 186; i++)
		SingY[i] = start
		- int(
		P[0] * (28 - 1) + P[1] * (3) + P[2] * (1)
		+ 0.5);
	//八列目
	for (i = 187; i <= 188; i++)
		SingY[i] = start
		- int(P[0] * (29 - 1) + P[3] * (i - 187) + 0.5);//連続フリック
	for (i = 189; i <= 190; i++)
		SingY[i] = start - int(P[0] * (29 - 1) + P[1] * (1) + 0.5);
	for (i = 191; i <= 192; i++)
		SingY[i] = start - int(P[0] * (29 - 1) + P[1] * (2) + 0.5);
	for (i = 193; i <= 194; i++)
		SingY[i] = start - int(P[0] * (29 - 1) + P[1] * (3) + 0.5);
	for (i = 195; i <= 196; i++)
		SingY[i] = start
		- int(
		P[0] * (29 - 1) + P[1] * (3) + P[2] * (1)
		+ 0.5);
	for (i = 197; i <= 198; i++)
		SingY[i] = start
		- int(
		P[0] * (30 - 1) + P[1] * (0) + P[2] * (1)
		+ 0.5);
	for (i = 199; i <= 200; i++)
		SingY[i] = start - int(P[0] * (30 - 1) + P[1] * (1) + 0.5);
	for (i = 201; i <= 202; i++)
		SingY[i] = start
		- int(
		P[0] * (30 - 1) + P[1] * (1) + P[2] * (1)
		+ 0.5);
	for (i = 203; i <= 204; i++)
		SingY[i] = start - int(P[0] * (30 - 1) + P[1] * (3) + 0.5);
	for (i = 205; i <= 210; i++)
		SingY[i] = start
		- int(P[0] * (31 - 1) + P[2] * (i - 205) + 0.5);
	for (i = 211; i <= 218; i++)
		SingY[i] = start
		- int(P[0] * (32 - 1) + P[2] * (i - 212) + 0.5);
	for (i = 219; i <= 220; i++)
		SingY[i] = start
		- int(
		P[0] * (32 - 1) + P[1] * (3) + P[2] * (1)
		+ 0.5);
	//九列目
	for (i = 221; i <= 222; i++)
		SingY[i] = start - int(P[0] * (33 - 1) + P[2] * (1) + 0.5);
	for (i = 223; i <= 224; i++)
		SingY[i] = start - int(P[0] * (33 - 1) + P[1] * (1) + 0.5);
	for (i = 225; i <= 226; i++)
		SingY[i] = start - int(P[0] * (33 - 1) + P[1] * (2) + 0.5);
	for (i = 227; i <= 228; i++)
		SingY[i] = start
		- int(
		P[0] * (33 - 1) + P[1] * (2) + P[2] * (1)
		+ 0.5);
	for (i = 229; i <= 230; i++)
		SingY[i] = start - int(P[0] * (33 - 1) + P[1] * (3) + 0.5);
	//十列目
	SingY[231] = start - int(P[0] * (34 - 1) + 0.5);
	for (i = 232; i <= 233; i++)
		SingY[i] = start
		- int(P[0] * (34 - 1) + P[3] * (i - 232) + 0.5);//連続フリック
	for (i = 234; i <= 237; i++)
		SingY[i] = start
		- int(P[0] * (34 - 1) + P[2] * (i - 233) + 0.5);
	for (i = 238; i <= 239; i++)
		SingY[i] = start
		- int(
		P[0] * (34 - 1) + P[1] * (2) + P[3] * (3)
		+ 0.5);
	for (i = 240; i <= 241; i++)
		SingY[i] = start
		- int(P[0] * (35 - 1) + P[2] * (i - 241) + 0.5);
	for (i = 242; i <= 244; i++)
		SingY[i] = start
		- int(P[0] * (35 - 1) + P[2] * (i - 242) + 0.5);
	for (i = 245; i <= 247; i++)
		SingY[i] = start
		- int(
		P[0] * (35 - 1) + P[1] * (2)
		+ P[3] * (i - 247) + 0.5);
	for (i = 248; i <= 250; i++)
		SingY[i] = start
		- int(P[0] * (36 - 1) + P[2] * (i - 250) + 0.5);
	for (i = 251; i <= 254; i++)
		SingY[i] = start
		- int(P[0] * (36 - 1) + P[2] * (i - 251) + 0.5);
	for (i = 255; i <= 256; i++)
		SingY[i] = start
		- int(
		P[0] * (36 - 1) + P[1] * (2)
		+ P[3] * (i - 256) + 0.5);
	for (i = 257; i <= 260; i++)
		SingY[i] = start
		- int(
		P[0] * (36 - 1) + P[1] * (2)
		+ P[2] * (i - 257) + 0.5);
	for (i = 261; i <= 262; i++)
		SingY[i] = start
		- int(
		P[0] * (37 - 1) + P[1] * (0)
		+ P[3] * (i - 261) + 0.5);	//連続フリック
	SingY[263] = start - int(P[0] * (37 - 1) + P[2] * (1) + 0.5);
	for (i = 264; i <= 265; i++)
		SingY[i] = start
		- int(
		P[0] * (37 - 1) + P[1] * (1)
		+ P[3] * (i - 264) + 0.5);	//連続フリック
	for (i = 266; i <= 267; i++)
		SingY[i] = start
		- int(
		P[0] * (37 - 1) + P[1] * (1)
		+ P[2] * (i - 266) + 0.5);
	for (i = 268; i <= 269; i++)
		SingY[i] = start
		- int(
		P[0] * (37 - 1) + P[1] * (2)
		+ P[3] * (i - 268) + 0.5);	//連続フリック
	SingY[270] = start - int(P[0] * (37 - 1) + P[1] * (2) + 0.5);
	for (i = 271; i <= 272; i++)
		SingY[i] = start
		- int(
		P[0] * (37 - 1) + P[1] * (3)
		+ P[3] * (i - 271) + 0.5);	//連続フリック
	//十一列目
	SingY[273] = start - int(P[0] * (38 - 1) + 0.5);
	for (i = 274; i <= 275; i++)
		SingY[i] = start
		- int(P[0] * (38 - 1) + P[3] * (i - 274) + 0.5);//連続フリック
	SingY[276] = start
		- int(P[0] * (38 - 1) + P[1] * (1) + P[3] * (-1) + 0.5);
	SingY[277] = start
		- int(P[0] * (38 - 1) + P[1] * (1) + P[2] * (1) + 0.5);
	for (i = 278; i <= 281; i++)
		SingY[i] = start
		- int(
		P[0] * (38 - 1) + P[1] * (2)
		+ P[2] * (i - 277) + 0.5);
	for (i = 282; i <= 284; i++)
		SingY[i] = start
		- int(P[0] * (39 - 1) + P[2] * (i - 282) + 0.5);
	for (i = 285; i <= 287; i++)
		SingY[i] = start
		- int(
		P[0] * (39 - 1) + P[1] * (2)
		+ P[3] * (i - 287) + 0.5);
	SingY[288] = start - int(P[0] * (39 - 1) + P[1] * (3) + 0.5);
	for (i = 289; i <= 291; i++)
		SingY[i] = start
		- int(P[0] * (40 - 1) + P[3] * (i - 291) + 0.5);
	for (i = 292; i <= 296; i++)
		SingY[i] = start
		- int(P[0] * (40 - 1) + P[2] * (i - 292) + 0.5);
	for (i = 297; i <= 300; i++)
		SingY[i] = start
		- int(
		P[0] * (40 - 1) + P[1] * (2)
		+ P[2] * (i - 297) + 0.5);
	SingY[301] = start - int(P[0] * (41 - 1) + P[2] * (-1) + 0.5);
	for (i = 302; i <= 303; i++)
		SingY[i] = start
		- int(P[0] * (41 - 1) + P[1] * (i - 301) + 0.5);
	SingY[304] = start - int(P[0] * (41 - 1) + P[1] * (2) + 0.5);
	for (i = 305; i <= 309; i++)
		SingY[i] = start
		- int(P[0] * (42 - 1) + P[2] * (i - 309) + 0.5);//連続フリック
	//十二列目
	for (i = 310; i <= 315; i++)
		SingY[i] = start
		- int(P[0] * (42 - 1) + P[1] * (i - 310) + 0.5);
	for (i = 316; i <= 318; i++)
		SingY[i] = start
		- int(P[0] * (43 - 1) + P[1] * (i - 315) + 0.5);
	for (i = 319; i <= 321; i++)
		SingY[i] = start
		- int(P[0] * (44 - 1) + P[1] * (i - 320) + 0.5);
	for (i = 322; i <= 323; i++)
		SingY[i] = start
		- int(P[0] * (44 - 1) + P[1] * (i - 321) + 0.5);
	for (i = 324; i <= 325; i++)
		SingY[i] = start
		- int(P[0] * (44 - 1) + P[1] * (i - 322) + 0.5);
	for (i = 326; i <= 327; i++)
		SingY[i] = start
		- int(P[0] * (44 - 1) + P[1] * (i - 323) + 0.5);
	for (i = 328; i <= 329; i++)
		SingY[i] = start
		- int(P[0] * (45 - 1) + P[1] * (i - 328) + 0.5);
	for (i = 330; i <= 331; i++)
		SingY[i] = start
		- int(P[0] * (45 - 1) + P[1] * (i - 329) + 0.5);
	//十三列目
	for (i = 332; i <= 333; i++)
		SingY[i] = start
		- int(
		P[0] * (46 - 1) + P[2] * (i - 332)
		+ P[3] * (i - 332) + 0.5);
	for (i = 334; i <= 335; i++)
		SingY[i] = start
		- int(
		P[0] * (46 - 1) + P[2] * (i - 332)
		+ P[2] * (i - 335) + 0.5);		//不安
	for (i = 336; i <= 339; i++)
		SingY[i] = start
		- int(P[0] * (47 - 1) + P[2] * (i - 339) + 0.5);
	for (i = 340; i <= 341; i++)
		SingY[i] = start
		- int(
		P[0] * (47 - 1) + P[1] * (1) + P[2] * (1)
		+ 0.5);
	for (i = 342; i <= 343; i++)
		SingY[i] = start - int(P[0] * (47 - 1) + P[1] * (2) + 0.5);
	for (i = 344; i <= 345; i++)
		SingY[i] = start
		- int(
		P[0] * (47 - 1) + P[1] * (3) + P[3] * (-1)
		+ 0.5);
	SingY[346] = start - int(P[0] * (48 - 1) + P[2] * (-1) + 0.5);
	for (i = 347; i <= 349; i++)
		SingY[i] = start
		- int(P[0] * (48 - 1) + P[1] * (i - 346) + 0.5);
	for (i = 350; i <= 354; i++)
		SingY[i] = start
		- int(P[0] * (49 - 1) + P[2] * (i - 350) + 0.5);
	SingY[355] = start
		- int(P[0] * (49 - 1) + P[1] * (3) + P[3] * (-1) + 0.5);
	for (i = 356; i <= 357; i++)
		SingY[i] = start - int(P[0] * (50 - 1) + P[2] * (-1) + 0.5);
	//十四列目
	for (i = 358; i <= 360; i++)
		SingY[i] = start
		- int(P[0] * (50 - 1) + P[1] * (i - 358) + 0.5);
	for (i = 361; i <= 363; i++)
		SingY[i] = start
		- int(P[0] * (50 - 1) + P[1] * (i - 359) + 0.5);
	for (i = 364; i <= 366; i++)
		SingY[i] = start
		- int(P[0] * (51 - 1) + P[1] * (i - 364) + 0.5);
	for (i = 367; i <= 369; i++)
		SingY[i] = start
		- int(P[0] * (51 - 1) + P[1] * (i - 365) + 0.5);
	for (i = 370; i <= 372; i++)
		SingY[i] = start
		- int(P[0] * (52 - 1) + P[1] * (i - 370) + 0.5);
	for (i = 373; i <= 374; i++)
		SingY[i] = start
		- int(P[0] * (52 - 1) + P[1] * (i - 371) + 0.5);
	SingY[375] = start - int(P[0] * (52 - 1) + P[1] * (3) + 0.5);
	for (i = 376; i <= 377; i++)
		SingY[i] = start - int(P[0] * (53 - 1) + P[1] * (0) + 0.5);
	for (i = 378; i <= 379; i++)
		SingY[i] = start - int(P[0] * (53 - 1) + P[2] * (1) + 0.5);
	SingY[380] = start - int(P[0] * (53 - 1) + P[2] * (2) + 0.5);
	for (i = 381; i <= 382; i++)
		SingY[i] = start - int(P[0] * (53 - 1) + P[2] * (3) + 0.5);
	for (i = 383; i <= 384; i++)
		SingY[i] = start - int(P[0] * (53 - 1) + P[2] * (4) + 0.5);
	for (i = 385; i <= 386; i++)
		SingY[i] = start - int(P[0] * (53 - 1) + P[2] * (5) + 0.5);
	SingY[387] = start - int(P[0] * (53 - 1) + P[2] * (6) + 0.5);
	//十五列目
	for (i = 388; i <= 400; i = i + 2)
		SingY[i] = start
		- int(P[0] * (54 - 1) + P[2] * (i - 388) + 0.5);//
	for (i = 389; i <= 401; i = i + 2)
		SingY[i] = start
		- int(P[0] * (54 - 1) + P[2] * (i - 389) + 0.5);//
	for (i = 402; i <= 403; i++)
		SingY[i] = start
		- int(
		P[0] * (55 - 1) + P[1] * (3) + P[3] * (-1)
		+ 0.5);
	for (i = 404; i <= 405; i++)
		SingY[i] = start - int(P[0] * (56 - 1) + P[2] * (-1) + 0.5);
	for (i = 406; i <= 407; i++)
		SingY[i] = start - int(P[0] * (56 - 1) + P[1] * (1) + 0.5);
	for (i = 408; i <= 414; i++)
		SingY[i] = start
		- int(
		P[0] * (56 - 1) + P[1] * (2)
		+ P[3] * (i - 408) + 0.5);
	SingY[415] = start - int(P[0] * (57 - 1) + P[2] * (1) + 0.5);
	for (i = 416; i <= 417; i++)
		SingY[i] = start - int(P[0] * (57 - 1) + P[1] * (1) + 0.5);
	for (i = 418; i <= 419; i++)
		SingY[i] = start
		- int(
		P[0] * (57 - 1) + P[1] * (2)
		+ P[2] * (i - 418)
		+ P[3] * (i - 418) + 0.5);
	//十六列目
	for (i = 420; i <= 425; i++)
		SingY[i] = start
		- int(P[0] * (58 - 1) + P[2] * (i - 421) + 0.5);
	for (i = 426; i <= 427; i++)
		SingY[i] = start
		- int(
		P[0] * (58 - 1) + P[1] * (3)
		+ P[2] * (i - 426) + 0.5);
	SingY[428] = start - int(P[0] * (59 - 1) + P[2] * (1) + 0.5);
	for (i = 429; i <= 430; i++)
		SingY[i] = start
		- int(
		P[0] * (59 - 1) + P[1] * (1) + P[2] * (1)
		+ 0.5);
	for (i = 431; i <= 433; i++)
		SingY[i] = start
		- int(P[0] * (59 - 1) + P[1] * (i - 429) + 0.5);
	for (i = 434; i <= 444; i = i + 2)
		SingY[i] = start
		- int(P[0] * (60 - 1) + P[2] * (i - 432) + 0.5);//連続フリック
	for (i = 435; i <= 445; i = i + 2)
		SingY[i] = start
		- int(
		P[0] * (60 - 1) + P[2] * (i - 433)
		+ P[3] * (1) + 0.5);		//連続フリック
	for (i = 446; i <= 447; i++)
		SingY[i] = start
		- int(
		P[0] * (61 - 1) + P[1] * (3)
		+ P[3] * (i - 446) + 0.5);	//連続フリック
	//十七列目
	SingY[448] = start - int(P[0] * (62 - 1) + 0.5);
	for (i = 449; i <= 453; i++)
		SingY[i] = start
		- int(P[0] * (62 - 1) + P[2] * (i - 449) + 0.5);
	for (i = 454; i <= 455; i++)
		SingY[i] = start
		- int(
		P[0] * (62 - 1) + P[1] * (3)
		+ P[2] * (i - 454) + 0.5);
	for (i = 456; i <= 457; i++)
		SingY[i] = start
		- int(
		P[0] * (63 - 1) + P[1] * (i - 456)
		+ P[2] * (1) + 0.5);
	SingY[458] = start - int(P[0] * (64 - 1) + P[1] * (3) + 0.5);
	for (i = 459; i <= 460; i++)
		SingY[i] = start - int(P[0] * (64 - 1) + P[2] * (-1) + 0.5);
	for (i = 461; i <= 462; i++)
		SingY[i] = start - int(P[0] * (64 - 1) + P[1] * (3) + 0.5);
	for (i = 463; i <= 464; i++)
		SingY[i] = start - int(P[0] * (64 - 1) + P[1] * (3) + P[2] * (1)+ 0.5);
	for (i = 465; i <= 466; i++)
		SingY[i] = start - int(P[0] * (66 - 1) + P[1] * (-1) + 0.5);

	//for (i = 3; i <= 4; i++)SingY[i] = -500 - i * 200;
	//for (i = 6; i <= 9; i++)SingY[i] = -1000 - i * 50;
	//return SingY;
	return 1;
}