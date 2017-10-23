#include "Graphics.cpp"

/*ノーツを消した時のエフェクト*/
class HitEffect {
private:
	Graph effect;
	const int type = 5;
	const double moveTime = 0.2;
	const double finishTime = 0.5;
	double time[5];
	double posX, posY;
public:
	/*effect.setGraph(LoadGraph("materials\\Image\\hit_circle.png"));
		for (int i = 0; i < type; i++) time[i] = 0.0;*/
	HitEffect() {
		effect.setGraph(LoadGraph("materials\\Image\\hit_circle.png"));
		for (int i = 0; i < type; i++) time[i] = 0.0;
	}
	/*位置設定*/
	void SetPos(double _posX, double _posY) {
		posX = _posX;
		posY = _posY;
	}
	/*_posX = 1,2,3,4,5*/
	void Hit(int _posX) {
		time[_posX - 1] = (double)GetNowCount() / 1000.0;
	}
	/*
	for (int i = 0; i < type; i++) {
		if (0.0 < time[i]) {
			effect.DrawHitEffect(posX * (i + 1), posY, moveTime, (double)GetNowCount() / 1000.0 - time[i]);
			if (finishTime <= GetNowCount() / 1000.0 - time[i]) {
				time[i] = 0.0;
			}
		}
	}
	*/
	void PrintEffect() {
		for (int i = 0; i < type; i++) {
			if (0.0 < time[i]) {
				effect.DrawHitEffect(posX * (i + 1), posY, moveTime, (double)GetNowCount() / 1000.0 - time[i]);
				if (finishTime <= GetNowCount() / 1000.0 - time[i]) {
					time[i] = 0.0;
				}
			}
		}
	}
};