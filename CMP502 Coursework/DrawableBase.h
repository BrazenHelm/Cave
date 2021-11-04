#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "IndexBuffer.h"
#include "OBJ_Loader.h"
#include "Surface.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include <memory>
#include <vector>


template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() { return !m_staticBinds.empty(); }

	static void AddStaticBind(std::unique_ptr<Bindable> bind)
	{
		assert(typeid(*bind) != typeid(IndexBuffer));
		m_staticBinds.push_back(std::move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf)
	{
		//assert(m_pIndexBuffer == nullptr);
		m_pIndexBuffer = ibuf.get();
		m_staticBinds.push_back(std::move(ibuf));
	}

	void SetIndexFromStatic()
	{
		assert(m_pIndexBuffer == nullptr);
		for (const auto& sb : m_staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(sb.get()))
			{
				m_pIndexBuffer = p;
				return;
			}
		}
	}

	void LoadStaticMesh(Graphics& gfx, const std::string& path)
	{
		objl::Loader loader;
		loader.LoadFile(path);

		const std::vector<objl::Vertex> vertices = loader.LoadedVertices;
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		std::vector<unsigned short> indices;
		const size_t N_INDICES = loader.LoadedIndices.size();
		indices.reserve(N_INDICES);
		for (size_t i = 0; i < N_INDICES; i++)
		{
			indices.push_back(loader.LoadedIndices[i]);
		}
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
	}

	void AddStaticMesh(Graphics&gfx, const std::vector<objl::Vertex> vertices,
		const std::vector<unsigned short> indices)
	{
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));
	}

	void LoadStaticTexture(Graphics& gfx, const std::string& path)
	{
		const Surface s = Surface::FromFile(path);
		AddStaticBind(std::make_unique<Texture>(gfx, s));
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const
	{
		return m_staticBinds;
	}

	// Bindables that are shared by all Drawables of a given type
	// e.g. Vertex Buffer, Index Buffer, Texture
	static std::vector<std::unique_ptr<Bindable>> m_staticBinds;
};


template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::m_staticBinds;

