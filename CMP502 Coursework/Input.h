#pragma once
#include "Inputs.h"

class Keyboard;
class Mouse;


class Input
{
public:
	Input(Keyboard* pKeyboard, Mouse* pMouse);
	~Input();

	Inputs GetInputs();
	VolumeControls GetVolumeControls();
	bool GetLeftClick();
	bool GetRightClick();
	bool GetQuit();

private:
	float GetMoveForward();
	float GetMoveRight();
	float GetMoveUp();
	float GetLookRight();
	float GetLookUp();

	//float GetVerticalPan();
	//float GetHorizontalPan();
	//int GetRotation();

private:
	Keyboard* m_pKeyboard;
	Mouse* m_pMouse;
};

