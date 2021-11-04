#pragma once
#include "Drawable.h"
#include "GodRay.h"
#include "PointLight.h"
#include "Terrain.h"
#include "Timer.h"
#include "Window.h"
#include <random>
#include <vector>


class App
{
public:
	App(int width);
	
	int Run();

private:
	void DoFrame();

private:
	Window						m_window;
	Timer						m_timer;
	PointLight					m_light;

	std::unique_ptr<Terrain>	m_pTerrain;
	std::unique_ptr<GodRay>		m_pGodRay;

	Camera*						m_pCamera;

	bool m_quit = false;
};

