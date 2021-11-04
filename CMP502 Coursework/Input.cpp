#include "Input.h"
#include "Keyboard.h"
#include "Mouse.h"


Input::Input(Keyboard* pKeyboard, Mouse* pMouse) :
	m_pKeyboard(pKeyboard), m_pMouse(pMouse)
{}


Input::~Input()
{
	m_pKeyboard = nullptr;
}


Inputs Input::GetInputs()
{
	Inputs inputs;

	inputs.moveForward = GetMoveForward();
	inputs.moveRight = GetMoveRight();
	inputs.moveUp = GetMoveUp();

	float moveMagnitude = sqrtf(
		inputs.moveForward * inputs.moveForward
		+ inputs.moveRight * inputs.moveRight
		+ inputs.moveUp * inputs.moveUp);

	if (moveMagnitude > 0.00001f)
	{
		inputs.moveForward /= moveMagnitude;
		inputs.moveRight /= moveMagnitude;
		inputs.moveUp /= moveMagnitude;
	}

	inputs.lookRight = 0;
	inputs.lookUp = 0;

	auto od = m_pMouse->ReadRawDelta();
	while (od != std::nullopt)
	{
		auto d = od.value();
		inputs.lookRight += d.dx;
		inputs.lookUp -= d.dy;		// inverted
		od = m_pMouse->ReadRawDelta();
	}

	return inputs;
}


VolumeControls Input::GetVolumeControls()
{
	VolumeControls ctrls;
	ctrls.increase = m_pKeyboard->KeyPressedThisFrame('P');
	ctrls.decrease = m_pKeyboard->KeyPressedThisFrame('O');
	return ctrls;
}


bool Input::GetLeftClick()
{
	return m_pMouse->LeftPressedThisFrame();
}


bool Input::GetRightClick()
{
	return m_pMouse->RightPressedThisFrame();
}


bool Input::GetQuit()
{
	return m_pKeyboard->KeyIsPressed(27);	// escape?
}


float Input::GetMoveForward()
{
	bool goingForward = m_pKeyboard->KeyIsPressed('W');
	bool goingBackward = m_pKeyboard->KeyIsPressed('S');

	if (goingForward && !goingBackward) return 1.f;
	if (goingBackward && !goingForward) return -1.f;
	else return 0;
}


float Input::GetMoveRight()
{
	bool goingRight = m_pKeyboard->KeyIsPressed('D');
	bool goingLeft = m_pKeyboard->KeyIsPressed('A');

	if (goingRight && !goingLeft) return 1.f;
	if (goingLeft && !goingRight) return -1.f;
	else return 0;
}


float Input::GetMoveUp()
{
	bool goingUp = m_pKeyboard->KeyIsPressed('Q');
	bool goingDown = m_pKeyboard->KeyIsPressed('E');

	if (goingUp && !goingDown) return 1.f;
	if (goingDown && !goingUp) return -1.f;
	else return 0;
	//return 0;
}


float Input::GetLookRight()
{
	static int xLastFrame = m_pMouse->GetPosX();
	int x = m_pMouse->GetPosX();
	int dx = x - xLastFrame;
	xLastFrame = x;
	return (float)dx;
	//return (float)m_pMouse->GetPosX();
}


float Input::GetLookUp()
{
	static int yLastFrame = m_pMouse->GetPosY();
	int y = m_pMouse->GetPosY();
	int dy = y - yLastFrame;
	yLastFrame = y;
	return -(float)dy;
}

