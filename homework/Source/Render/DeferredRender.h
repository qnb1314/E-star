/*************************************************

Author:乔南

Date:2022-4.01

Description:延迟渲染器类，包含一系列子渲染器，主要负责控制各个子渲染器的调用

**************************************************/
#pragma once
#include "ShadowRender.h"
#include "DepthRender.h"
#include "HiZRender.h"
#include "GBufferRender.h"
#include "LightRender.h"
#include "SSRRender.h"
#include "PostRender.h"
#include "DebugRender.h"
#include "DebuglineRender.h"

class DeferredRender
{
public:
	ShadowRenderRef ShadowRenderref;
	DepthRenderRef DepthRenderref;
	HiZRenderRef HiZRenderref;
	GBufferRenderRef GBufferRenderref;
	LightRenderRef LightRenderref;
	SSRRenderRef SSRRenderref;
	PostRenderRef PostRenderref;
	DebuglineRenderRef DebuglineRenderref;
	std::vector<DebugRenderRef> DebugRenderrefs;

	int HiZLevel = 7;

	DeferredRender(unsigned int size_level)
		:ShadowRenderref(new ShadowRender(1))
		, DepthRenderref(new DepthRender(189))
		, HiZRenderref(new HiZRender(190, size_level))
		, GBufferRenderref(new GBufferRender(201, pow(2, size_level)))
		, LightRenderref(new LightRender(202, pow(2, size_level)))
		, SSRRenderref(new SSRRender(203, size_level))
		, PostRenderref(new PostRender(204))
		, DebuglineRenderref(new DebuglineRender(205))
	{
		for (int i = 206; i <= 210; i++)DebugRenderrefs.push_back(new DebugRender(i));
	}
	~DeferredRender() {}

	void InitRHI()
	{
		ShadowRenderref->InitRHI();
		DepthRenderref->InitRHI();
		HiZRenderref->InitRHI();
		GBufferRenderref->InitRHI();
		LightRenderref->InitRHI();
		SSRRenderref->InitRHI();
		PostRenderref->InitRHI();
		DebuglineRenderref->InitRHI();
		for (int i = 0; i < DebugRenderrefs.size(); i++)DebugRenderrefs[i]->InitRHI();
	}

	void ReleaseRHI()
	{
		ShadowRenderref->ReleaseRHI();
		DepthRenderref->ReleaseRHI();
		HiZRenderref->ReleaseRHI();
		GBufferRenderref->ReleaseRHI();
		LightRenderref->ReleaseRHI();
		SSRRenderref->ReleaseRHI();
		PostRenderref->ReleaseRHI();
		DebuglineRenderref->ReleaseRHI();
		for (int i = 0; i < DebugRenderrefs.size(); i++)DebugRenderrefs[i]->ReleaseRHI();
	}
public:
	void Render(const Scene& scene, const View& view, float scene_size, Object* object_dubug = NULL)
	{
		//获取渲染对象
		std::vector<ObjectRef> RenderObjects;
		std::vector<LightRef> RenderLights;
		SkyBoxRef RenderSky = scene.Environment[scene.sky_state];

		//裁剪
		CullObject(scene.All_Object, RenderObjects, view);
		CullLight(scene.All_Light, RenderLights, view);

		//初始化渲染器
		BeforeRender(view);

		//逐个pass提交
		ShadowPass(RenderLights, scene.All_Object);
		DepthPass(RenderObjects, view);
		HiZPass();
		GBufferPass(RenderObjects, RenderSky, view);
		LightPass(RenderLights, RenderSky, view);
		SSRPass(view);
		PostPass(view, scene_size);
		if (object_dubug)DebuglinePass(view, scene_size, object_dubug);
		DebugPass(view, scene_size);

		//结算管线
		bgfx::frame();

		//恢复渲染器
		AfterRender();
	}


private:

	void BeforeRender(const View& view)
	{
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR
			, 0x000000ff
		);
		bgfx::setViewRect(0, 0, 0, view.width, view.height);
		bgfx::touch(0);
	}

	void AfterRender()
	{
		ShadowRenderref->AfterRender();
		DepthRenderref->AfterRender();
	}


private:

	void ShadowPass(const std::vector<LightRef>& render_lights, const std::vector<ObjectRef>& all_objects)
	{
		ShadowRenderref->Render(render_lights, all_objects);
	}

	void DepthPass(const std::vector<ObjectRef>& render_objects, const View& view)
	{
		DepthRenderref->BeforeRender(view, GBufferRenderref->DepthBufferref);
		DepthRenderref->Render(render_objects);
	}

	void HiZPass()
	{
		HiZRenderref->BeforeRender();
		HiZRenderref->Render(GBufferRenderref->DepthBufferref, HiZLevel);
	}

	void GBufferPass(const std::vector<ObjectRef>& render_objects, SkyBoxRef render_sky, const View& view)
	{
		GBufferRenderref->BeforeRender(view);
		GBufferRenderref->Render(render_objects, render_sky, view);
	}

	void LightPass(const std::vector<LightRef>& render_lights, SkyBoxRef render_sky, const View& view)
	{
		LightRenderref->BeforeRender(view);
		LightRenderref->Render(render_lights, render_sky, view, GBufferRenderref->GBufferref);
	}

	void SSRPass(const View& view)
	{
		SSRRenderref->BeforeRender(view, GBufferRenderref->GBufferref, LightRenderref->ColorTextureref, HiZRenderref->HiZBuffer, HiZLevel);
		SSRRenderref->Render();
	}

	void PostPass(const View& view, float scene_size)
	{
		PostRenderref->BeforeRender(0, view.height * 0.5f * (1.0 - scene_size), view.width * scene_size, view.height * scene_size, SSRRenderref->ColorTextureref);
		PostRenderref->Render();
	}

	void DebuglinePass(const View& view, float scene_size, Object* object_debug)
	{
		DebuglineRenderref->BeforeRender(0, view.height * 0.5f * (1.0 - scene_size), view.width * scene_size, view.height * scene_size, view);
		DebuglineRenderref->Render(object_debug);
	}

	void DebugPass(const View& view, float scene_size)
	{
		for (int i = 0; i < 5; i++)
		{
			DebugRenderrefs[i]->BeforeRender(view.width * scene_size, view.height * 0.2 * i, view.width * (1.0 - scene_size), view.height * 0.2, GBufferRenderref->GBufferref->textures[i]);
			DebugRenderrefs[i]->Render();
		}
	}

private:
	void CullObject(const std::vector<ObjectRef>& all_objects, std::vector<ObjectRef>& render_objects, const View& view)
	{
		//得到视锥平面-左,右,上,下,前,后
		std::vector <Vector3f> normals;
		std::vector <Vector3f> points;
		view.GetViewPlane(normals, points);
		for (int i = 0; i < all_objects.size(); i++)
		{
			ObjectRef curr_object = all_objects[i];
			Vector3f pmax = curr_object->bound->PMax;
			Vector3f pmin = curr_object->bound->PMin;
			if (AABBWithView(normals, points, pmax, pmin) != 1)render_objects.push_back(curr_object);
		}
	}

	void CullLight(const std::vector<LightRef>& all_lights, std::vector<LightRef>& render_lights, const View& view)
	{
		for (int i = 0; i < all_lights.size(); i++)
		{
			render_lights.push_back(all_lights[i]);
		}
	}
};
typedef CountRef<DeferredRender> DeferredRenderRef;