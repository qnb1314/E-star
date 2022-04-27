/*************************************************

Author:乔南

Date:2022-4.01

Description:ZPrePass渲染器,提前渲染深度,与earlyz结合加速GbufferPass

**************************************************/
#pragma once
#include "RenderBase.h"

class DepthRender :public RenderBase
{
public:
	ShaderRef DepthShaderref;
	bgfx::FrameBufferHandle FBO;

	DepthRender(bgfx::ViewId id) :RenderBase(id), DepthShaderref(new DepthShader){}
	~DepthRender() {}

	virtual void InitRHI() override
	{
		DepthShaderref->InitRHI();
	}

	virtual void ReleaseRHI() override
	{
		DepthShaderref->ReleaseRHI();
	}

public:
	void BeforeRender(const View& view, DynamicTexture2DRef target)
	{
		bgfx::Attachment gbufferAt[1];
		gbufferAt[0].init(target->m_texh);
		FBO = bgfx::createFrameBuffer(1, gbufferAt, false);

		bgfx::setViewFrameBuffer(m_viewid, FBO);
		bgfx::setViewClear(m_viewid
			, BGFX_CLEAR_DEPTH
			, 0x000000ff
			, 1.0f
			, 0
		);
		bgfx::setViewRect(m_viewid, 0, 0, uint16_t(target->width), uint16_t(target->height));
		bgfx::touch(m_viewid);

		//视点变换矩阵
		float viewmat[16];
		float projmat[16];
		GetMat4WithMatrix4f(view.GetViewMatrix(), viewmat);
		GetMat4WithMatrix4f(view.GetProjectionMatrix(), projmat);
		bgfx::setViewTransform(m_viewid, viewmat, projmat);
	}

	void Render(const std::vector<ObjectRef>& render_objects)
	{
		for (int i = 0; i < render_objects.size(); i++)
		{
			ObjectRef curr = render_objects[i];
			float mtx[16];
			GetMat4WithMatrix4f(curr->Model, mtx);
			bgfx::setTransform(mtx);

			bgfx::setVertexBuffer(0, curr->staticmeshref->renderdata.m_vbh[0]);
			bgfx::setIndexBuffer(curr->staticmeshref->renderdata.m_ibh[1]);

			bgfx::setState(0
				| BGFX_STATE_WRITE_Z
				| BGFX_STATE_DEPTH_TEST_LEQUAL
				| BGFX_STATE_CULL_CW
			);
			bgfx::submit(m_viewid, DepthShaderref->m_program);
		}
	}

	void AfterRender()
	{
		bgfx::setViewFrameBuffer(m_viewid, BGFX_INVALID_HANDLE);
		bgfx::destroy(FBO);
	}
};
typedef CountRef<DepthRender> DepthRenderRef;