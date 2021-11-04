#include "Drawable.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "PrimitiveTopology.h"
#include "Sampler.h"
#include "VertexShader.h"
#include "VertexBuffer.h"
#include <assert.h>


std::unique_ptr<VertexShader>		Drawable::m_depth_vs;
std::unique_ptr<PixelShader>		Drawable::m_depth_ps;
std::unique_ptr<InputLayout>		Drawable::m_depth_inputLayout;

std::unique_ptr<VertexShader>		Drawable::m_light_vs;
std::unique_ptr<PixelShader>		Drawable::m_light_ps;
std::unique_ptr<InputLayout>		Drawable::m_light_inputLayout;

std::unique_ptr<PixelShader>		Drawable::m_fog_ps;

std::unique_ptr<PrimitiveTopology>	Drawable::m_pTopology;
std::unique_ptr<Sampler>			Drawable::m_pSampler;


void Drawable::InitializeSharedBindables(Graphics& gfx)
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_depth_vs = std::make_unique<VertexShader>(gfx, "Shaders/depth_vs.cso");
	m_depth_ps = std::make_unique<PixelShader>(gfx, "Shaders/depth_ps.cso");
	m_depth_inputLayout = std::make_unique<InputLayout>(
		gfx, ied, m_depth_vs->GetBytecode());

	m_light_vs = std::make_unique<VertexShader>(gfx, "Shaders/shadow_vs.cso");
	m_light_ps = std::make_unique<PixelShader>(gfx, "Shaders/shadow_ps.cso");
	m_light_inputLayout = std::make_unique<InputLayout>(
		gfx, ied, m_light_vs->GetBytecode());

	m_fog_ps = std::make_unique<PixelShader>(gfx, "Shaders/fog_ps.cso");

	m_pTopology = std::make_unique<PrimitiveTopology>(
		gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTopology->Bind(gfx);

	m_pSampler = std::make_unique<Sampler>(gfx);
	m_pSampler->Bind(gfx);
}


void Drawable::Draw(Graphics& gfx) const
{
	m_pTopology->Bind(gfx);
	m_pSampler->Bind(gfx);
	for (auto& b : m_binds)
	{
		b->Bind(gfx);
	}
	for (auto& sb : GetStaticBinds())
	{
		sb->Bind(gfx);
	}

	gfx.DrawIndexed(m_pIndexBuffer->GetCount());
}


void Drawable::SetDrawModeDepth(Graphics& gfx)
{
	m_depth_vs->Bind(gfx);
	m_depth_ps->Bind(gfx);
	m_depth_inputLayout->Bind(gfx);
}


void Drawable::SetDrawModeLight(Graphics& gfx)
{
	m_light_vs->Bind(gfx);
	m_light_ps->Bind(gfx);
	m_light_inputLayout->Bind(gfx);
}


void Drawable::SetDrawModeFog(Graphics& gfx)
{
	m_fog_ps->Bind(gfx);
}


void Drawable::AddBind(std::unique_ptr<Bindable> bindable)
{
	assert("Use AddIndexBuffer to bind Index Buffer" && typeid(*bindable) != typeid(IndexBuffer));
	m_binds.push_back(std::move(bindable));
}


void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf)
{
	// ORIGINAL
	assert("Index Buffer already added" && m_pIndexBuffer == nullptr);
	m_pIndexBuffer = ibuf.get();
	m_binds.push_back(std::move(ibuf));


	// NEW
	//delete m_pIndexBuffer;
	//m_pIndexBuffer = ibuf.get();
}


//void Drawable::AddVertexBuffer(std::unique_ptr<VertexBuffer> vbuf)
//{
//	delete m_pVertexBuffer;
//	m_pVertexBuffer = vbuf.get();
//}

