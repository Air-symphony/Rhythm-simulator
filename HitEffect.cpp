#include "Graphics.cpp"

/*ノーツを消した時のエフェクト*/
class HitEffect {
private:
	Graph effect;
	const int type = 5;
	const double moveTime = 0.2;
	const double finishTime = 0.5;
	double blendTime = finishTime / 5.0;
	double time[5];
	int judgeType[5];
	double posX, posY, ScreenSideSize;
public:
	/*for (int i = 0; i < type; i++) {
			time[i] = 0.0;
			judgeType[i] = -1;
		}*/
	HitEffect() {
		for (int i = 0; i < type; i++) {
			time[i] = 0.0;
			judgeType[i] = -1;
		}
	}
	/*effect.setGraph(graph);*/
	void SetGraph(int graph) {
		effect.setGraph(graph);
	}
	/*位置設定*/
	void SetPos(double sideSize, double _posX, double _posY) {
		posX = _posX;
		posY = _posY;
		ScreenSideSize = sideSize;
	}
	/*_posX = 1,2,3,4,5
	type = 0,1,2 Per,Gre,Ni*/
	void Hit(int _posX, int type) {
		time[_posX - 1] = (double)GetNowCount() / 1000.0;
		judgeType[_posX - 1] = type;
	}
	/*
	for (int i = 0; i < type; i++) {
			if (0.0 < time[i] && 0 <= judgeType[i]) {
				double limit = GetNowCount() / 1000.0 - time[i];
				if (finishTime <= limit + blendTime) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.0 * (finishTime - limit) / blendTime));
					effect.DrawHitEffect(judgeType[i], (int)posX * (i + 1), (int)posY, moveTime, limit);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				effect.DrawHitEffect(judgeType[i], (int)posX * (i + 1), (int)posY, moveTime, limit);
				if (finishTime <= limit) {
					time[i] = 0.0;
					judgeType[i] = -1;
				}
			}
		}
	*/
	void PrintEffect() {
		for (int i = 0; i < type; i++) {
			if (0.0 < time[i] && 0 <= judgeType[i]) {
				double limit = GetNowCount() / 1000.0 - time[i];
				if (finishTime <= limit + blendTime) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.0 * (finishTime - limit) / blendTime));
					effect.DrawHitEffect(judgeType[i], (int)(ScreenSideSize + posX * i), (int)posY, moveTime, limit);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				effect.DrawHitEffect(judgeType[i], (int)(ScreenSideSize + posX * i), (int)posY, moveTime, limit);
				if (finishTime <= limit) {
					time[i] = 0.0;
					judgeType[i] = -1;
				}
			}
		}
	}
};