#pragma once
#include "Graphics.h"
#include "Transform.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	static void InitializeSharedBindables(Graphics& gfx);

	void Draw(Graphics& gfx) const;
	
	static void SetDrawModeDepth(Graphics& gfx);
	static void SetDrawModeLight(Graphics& gfx);
	static void SetDrawModeFog(Graphics& gfx);

	Transform transform;
	DirectX::XMMATRIX GetTransformXM() const { return transform.GetWorldMatrix(); }

protected:
	void AddBind(std::unique_ptr<Bindable> bindable);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf);
	//void AddVertexBuffer(std::unique_ptr<class VertexBuffer> vbuf);

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const = 0;

	// Bindables that are shared by all Drawables 

	static std::unique_ptr<class VertexShader>		m_depth_vs;
	static std::unique_ptr<class PixelShader>		m_depth_ps;
	static std::unique_ptr<class InputLayout>		m_depth_inputLayout;

	static std::unique_ptr<class VertexShader>		m_light_vs;
	static std::unique_ptr<class PixelShader>		m_light_ps;
	static std::unique_ptr<class InputLayout>		m_light_inputLayout;

	static std::unique_ptr<class PixelShader>		m_fog_ps;

	static std::unique_ptr<class PrimitiveTopology>	m_pTopology;
	static std::unique_ptr<class Sampler>			m_pSampler;

private:
	const IndexBuffer* m_pIndexBuffer;

	// Bindables that are unique to an instance of the Drawable
	// e.g. Matrix Buffer
	std::vector<std::unique_ptr<Bindable>> m_binds;
};

