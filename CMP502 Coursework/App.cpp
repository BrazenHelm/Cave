#include "App.h"
#include "GDIPlusManager.h"
#include <memory>
#include <random>


GDIPlusManager gdpim;


App::App(int width) :
	m_window(width, width / 16 * 9, "CMP505 Coursework"),
	m_light(m_window.Gfx())
{
	Material mat = { {0.44f, 0.5f, 0.5f}, 0.2f, 100.f };
	m_pTerrain = std::make_unique<Terrain>(m_window.Gfx(), mat);
	m_pGodRay = std::make_unique<GodRay>(m_window.Gfx());

	Drawable::InitializeSharedBindables(m_window.Gfx());

	m_pCamera = m_window.Gfx().GetCamera();
	m_window.PlayAudio(true);
}


int App::Run()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())	// if (msg == WM_QUIT)
		{
			return *ecode;
		}

		DoFrame();

		if (m_quit) break;
	}
}


void App::DoFrame()
{
	float dt = m_timer.Split();

	m_window.Gfx().ControlCamera(m_window.input.GetInputs(), dt);
	auto camPos = m_pCamera->GetPos();

	objl::Vector3 vectorOut;
	if (m_pTerrain->Collision(camPos.x, camPos.y, camPos.z, 0.5f, vectorOut))
	{
		m_pCamera->SetPosition(camPos.x + vectorOut.X, camPos.y + vectorOut.Y, camPos.z + vectorOut.Z);
	}

	camPos = m_pCamera->GetPos();
	if (camPos.z < 1.5f)
	{
		m_pCamera->SetPosition(camPos.x, camPos.y, 1.5f);
	}
	
	camPos = m_pCamera->GetPos();
	auto camDir = m_pCamera->GetDir();

	//objl::Vector3 vCamPos(camPos.x, camPos.y, camPos.z);
	//objl::Vector3 vCamDir(camDir.x, camDir.y, camDir.z);
	//if (m_window.input.GetLeftClick())
	//{
	//	m_pTerrain->Mine(m_window.Gfx(), vCamPos, vCamDir);
	//}

	m_window.ControlAudio(m_window.input.GetVolumeControls());

	m_light.Bind(m_window.Gfx(), m_window.Gfx().GetCameraMatrix());

	// Render pass for depth map
	m_window.Gfx().SetDrawModeDepth();
	Drawable::SetDrawModeDepth(m_window.Gfx());
	m_pTerrain->Draw(m_window.Gfx());

	// Main render pass
	m_window.Gfx().SetDrawModeScene();
	Drawable::SetDrawModeLight(m_window.Gfx());
	m_pTerrain->Draw(m_window.Gfx());

	// Fog render pass
	m_window.Gfx().SetDrawModeFog();
	Drawable::SetDrawModeFog(m_window.Gfx());
	m_pGodRay->Draw(m_window.Gfx());

	m_window.Gfx().DoPostProcessing();

	m_window.Gfx().EndFrame();
	m_window.kb.EndFrame();
	m_window.mouse.EndFrame();

	if (m_window.input.GetQuit()) m_quit = true;
}

