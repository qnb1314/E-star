/*************************************************

Author:乔南

Date:2022-4.02

Description:debugline渲染器类，负责渲染选定物体的线条

**************************************************/
#pragma once
#include "RenderBase.h"

#undef DEBUG_LINE_LENGTH
#define DEBUG_LINE_LENGTH 10.0f

class DebuglineRender :public RenderBase
{
public:
	ShaderRef DebuglineShaderref;

	bgfx::UniformHandle u_color;
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;


	DebuglineRender(bgfx::ViewId id) :RenderBase(id), DebuglineShaderref(new DebuglineShader) {}
	~DebuglineRender() {}

	virtual void InitRHI() override
	{
		//初始化着色器
		DebuglineShaderref->InitRHI();
		u_color = bgfx::createUniform("color", bgfx::UniformType::Vec4);


		struct PosVertex
		{
			float x, y, z;
		};
		static const PosVertex buffer[] =
		{
			{0.0f,0.0f,0.0f},
			{DEBUG_LINE_LENGTH,0.0f,0.0f}
		};
		bgfx::VertexLayout layout;
		layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.end();
		m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(buffer, 2 * sizeof(PosVertex)), layout);

		static const uint16_t Indices[] = { 0,1 };
		m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(Indices, 2 * sizeof(uint16_t)));
	}

	virtual void ReleaseRHI() override
	{
		DebuglineShaderref->ReleaseRHI();
		bgfx::destroy(u_color);
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);
	}

public:
	void BeforeRender(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const View& view)
	{
		bgfx::setViewRect(m_viewid, _x, _y, _width, _height);
		float viewmat[16];
		float projmat[16];
		GetMat4WithMatrix4f(view.GetViewMatrix(), viewmat);
		GetMat4WithMatrix4f(view.GetProjectionMatrix(), projmat);
		bgfx::setViewTransform(m_viewid, viewmat, projmat);
	}

	void Render(Object* object_debug)
	{
		ObjectRender(object_debug);
		AxisRender(object_debug);
	}

private:
	void ObjectRender(Object* object_debug)
	{
		float mtx[16];
		GetMat4WithMatrix4f(object_debug->Model, mtx);
		bgfx::setTransform(mtx);

		float color[4] = { 1.0,0.0,0.0,1.0 };
		bgfx::setUniform(u_color, color);

		bgfx::setVertexBuffer(0, object_debug->staticmeshref->renderdata.m_vbh[0]);
		bgfx::setIndexBuffer(object_debug->staticmeshref->renderdata.m_ibh[0]);
		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_PT_LINES
		);
		bgfx::submit(m_viewid, DebuglineShaderref->m_program);
	}

	void AxisRender(Object* object_debug)
	{
		Eigen::Matrix4f Move;
		Move <<
			1.0f, 0.0f, 0.0f, object_debug->Model(0, 3),
			0.0f, 1.0f, 0.0f, object_debug->Model(1, 3),
			0.0f, 0.0f, 1.0f, object_debug->Model(2, 3),
			0.0f, 0.0f, 0.0f, 1.0f;

		{
			//x轴
			float mtx[16];
			GetMat4WithMatrix4f(Move, mtx);
			bgfx::setTransform(mtx);
			float color[4] = { 1.0,0.0,0.0,1.0 };
			bgfx::setUniform(u_color, color);
			bgfx::setVertexBuffer(0, m_vbh);
			bgfx::setIndexBuffer(m_ibh);
			bgfx::setState(0
				| BGFX_STATE_WRITE_RGB
				| BGFX_STATE_WRITE_A
				| BGFX_STATE_PT_LINES
			);
			bgfx::submit(m_viewid, DebuglineShaderref->m_program);
		}
		
		{
			//y轴
			float mtx[16];
			GetMat4WithMatrix4f(Move * RotateOn_Z_WithRadian(M_PI * 0.5f), mtx);
			bgfx::setTransform(mtx);
			float color[4] = { 0.0,1.0,0.0,1.0 };
			bgfx::setUniform(u_color, color);
			bgfx::setVertexBuffer(0, m_vbh);
			bgfx::setIndexBuffer(m_ibh);
			bgfx::setState(0
				| BGFX_STATE_WRITE_RGB
				| BGFX_STATE_WRITE_A
				| BGFX_STATE_PT_LINES
			);
			bgfx::submit(m_viewid, DebuglineShaderref->m_program);
		}

		{
			//z轴
			float mtx[16];
			GetMat4WithMatrix4f(Move * RotateOn_Y_WithRadian(-M_PI * 0.5f), mtx);
			bgfx::setTransform(mtx);
			float color[4] = { 0.0,0.0,1.0,1.0 };
			bgfx::setUniform(u_color, color);
			bgfx::setVertexBuffer(0, m_vbh);
			bgfx::setIndexBuffer(m_ibh);
			bgfx::setState(0
				| BGFX_STATE_WRITE_RGB
				| BGFX_STATE_WRITE_A
				| BGFX_STATE_PT_LINES
			);
			bgfx::submit(m_viewid, DebuglineShaderref->m_program);
		}
	}
};
typedef CountRef<DebuglineRender> DebuglineRenderRef;
