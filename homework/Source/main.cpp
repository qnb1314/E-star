#include "./scene/Scene.h"
#include "./scene/View.h"
#include "./Render/DeferredRender.h"
#include "./RenderResource/RHIController.h"
#include "common.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"

#include <entry/input.h>
#include <entry/cmd.h>

namespace
{
	static const InputBinding s_bindingView[] =
	{
		{entry::Key::KeyW,entry::Modifier::None,1,NULL,"view front +3.0"},
		{entry::Key::KeyS,entry::Modifier::None,1,NULL,"view front -3.0"},
		{entry::Key::KeyA,entry::Modifier::None,1,NULL,"view right -3.0"},
		{entry::Key::KeyD,entry::Modifier::None,1,NULL,"view right +3.0"},
		INPUT_BINDING_END
	};

	static const InputBinding s_bindingSky[] =
	{
		{entry::Key::Space,entry::Modifier::None,1,NULL,"sky"},
		INPUT_BINDING_END
	};

	static const char* s_bindingName[] =
	{
		"View",
		"Sky"
	};

	static const InputBinding* s_binding[] =
	{
		s_bindingView,
		s_bindingSky
	};


	int cmdView(CmdContext*, void* _userData, int _argc, char const* const* _argv)
	{
		View* m_camera = static_cast<View*>(_userData);
		if (_argc >= 3)
		{
			if (0 == bx::strCmp(_argv[1], "front"))
			{
				float step;
				bx::fromString(&step, _argv[2]);
				m_camera->GoForward(step);
			}
			else if (0 == bx::strCmp(_argv[1], "right"))
			{
				float step;
				bx::fromString(&step, _argv[2]);
				m_camera->GoRight(step);
			}
		}
		return 0;
	}
	int cmdSky(CmdContext*, void* _userData, int _argc, char const* const* _argv)
	{
		Scene* m_camera = static_cast<Scene*>(_userData);
		m_camera->ChangeSkyState();
		return 0;
	}

	void CopyMouseState(const entry::MouseState &source, entry::MouseState &target)
	{
		target.m_mx = source.m_mx;
		target.m_my = source.m_my;
		target.m_mz = source.m_mz;
		target.m_buttons[entry::MouseButton::Left] = source.m_buttons[entry::MouseButton::Left];
		target.m_buttons[entry::MouseButton::Right] = source.m_buttons[entry::MouseButton::Right];
		target.m_buttons[entry::MouseButton::Middle] = source.m_buttons[entry::MouseButton::Middle];
	}

	class Example : public entry::AppI
	{
	public:
		Example(const char* _name, const char* _description, const char* _url)
			: entry::AppI(_name, _description, _url)
			, object_debug(NULL)
			, scene_ratio(0.8f)
		{
		}

		void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
		{
			Args args(_argc, _argv);

			m_debug = BGFX_DEBUG_NONE;
			m_reset = BGFX_RESET_VSYNC;

			bgfx::Init init;
			init.type = bgfx::RendererType::Enum::OpenGL;
			init.vendorId = args.m_pciId;
			init.resolution.width = _width;
			init.resolution.height = _height;
			init.resolution.reset = m_reset;
			bgfx::init(init);

			bgfx::setDebug(m_debug);

			imguiCreate();

			inputAddBindings(s_bindingName[0], s_binding[0]);
			inputAddBindings(s_bindingName[1], s_binding[1]);
			cmdAdd("view", cmdView, &my_camera);
			cmdAdd("sky", cmdSky, &my_scene);

		//A.创建相机
			my_camera = View(Vector3f(0, 20, 30), 90, 0, _width, _height, 0.1f, 50.0f, 90);

		//B.创建渲染原始资源
			//1.静态网格体
			StaticMeshRef stone = new StaticMesh(new GraphObj("./meshes/stone.txt"));
			StaticMeshRef plane = new StaticMesh(new GraphCuboid(1, 1, 1));
			StaticMeshRef ball = new StaticMesh(new GraphBall(30, 30));
			resources.AddResource(stone);
			resources.AddResource(plane);
			resources.AddResource(ball);
			//2.纹理
			StaticTexture2DRef color = new StaticTexture2D("./textures/pbr_stone_base_color.dds");
			StaticTexture2DRef normal = new StaticTexture2D("./textures/pbr_stone_normal.dds");
			StaticTexture2DRef aorm = new StaticTexture2D("./textures/pbr_stone_aorm.dds");
			StaticTexture2DRef color_default = new StaticTexture2D("./textures/default_color.dds");
			StaticTexture2DRef normal_default = new StaticTexture2D("./textures/default_normal.dds");
			StaticTexture2DRef aorm_default_smooth = new StaticTexture2D("./textures/default_aorm_smooth.dds");
			StaticTexture2DRef aorm_default_rough = new StaticTexture2D("./textures/default_aorm_rough.dds");
			StaticTexture2DRef aorm_default_middle = new StaticTexture2D("./textures/default_aorm_middle.dds");
			StaticTexture2DRef aorm_default_specular = new StaticTexture2D("./textures/default_aorm_specular.dds");
			resources.AddResource(color);
			resources.AddResource(normal);
			resources.AddResource(aorm);
			resources.AddResource(color_default);
			resources.AddResource(normal_default);
			resources.AddResource(aorm_default_smooth);
			resources.AddResource(aorm_default_rough);
			resources.AddResource(aorm_default_middle);
			resources.AddResource(aorm_default_specular);
			StaticTextureCubeMapRef tex_lod_bolonga = new StaticTextureCubeMap("./textures/bolonga_lod.dds");
			StaticTextureCubeMapRef tex_irr_bolonga = new StaticTextureCubeMap("./textures/bolonga_irr.dds");

			StaticTextureCubeMapRef tex_lod_kyoto = new StaticTextureCubeMap("./textures/kyoto_lod.dds");
			StaticTextureCubeMapRef tex_irr_kyoto = new StaticTextureCubeMap("./textures/kyoto_irr.dds");
			resources.AddResource(tex_lod_bolonga);
			resources.AddResource(tex_irr_bolonga);
			resources.AddResource(tex_lod_kyoto);
			resources.AddResource(tex_irr_kyoto);
			//3.天空盒
			SkyBoxRef my_sky_bolonga = new SkyBox(tex_lod_bolonga, tex_irr_bolonga);
			SkyBoxRef my_sky_kyoto = new SkyBox(tex_lod_kyoto, tex_irr_kyoto);
			resources.AddResource(my_sky_bolonga);
			resources.AddResource(my_sky_kyoto);
			//4.光源
			LightRef light_point1 = new PointLight(Vector3f(-6, 20, 0), Vector3f(1, 1, 1) * 500, 100, 50);
			LightRef light_point2 = new PointLight(Vector3f(6, 20, 0), Vector3f(1, 1, 1) * 500, 100, 50);
			resources.AddResource(light_point1);
			resources.AddResource(light_point2);

		//C.创建材质
			MaterialRef material_1 = new Material(color, normal, aorm);
			MaterialRef material_2 = new Material(color_default, normal_default, aorm_default_specular);
			MaterialRef material_3 = new Material(color_default, normal_default, aorm_default_rough);
			MaterialRef material_4 = new Material(color_default, normal_default, aorm_default_middle);
			MaterialRef material_5 = new Material(color_default, normal_default, aorm_default_smooth);
			
		//D.加入场景
			//1.object
			my_scene.AddObject(new Object(stone, material_1));
			my_scene.All_Object[0]->Zoom(Vector3f(0.01,0.01,0.01));
			my_scene.All_Object[0]->MoveTo(Vector3f(10, 0, 0));
			my_scene.AddObject(new Object(stone, material_1));
			my_scene.All_Object[1]->Zoom(Vector3f(0.01, 0.02, 0.01));
			my_scene.All_Object[1]->MoveTo(Vector3f(-10, 0, 0));
			my_scene.AddObject(new Object(plane, material_2));
			my_scene.All_Object[2]->Zoom(Vector3f(20, 1, 40));
			my_scene.All_Object[2]->MoveTo(Vector3f(-10, 0, 0));
			my_scene.AddObject(new Object(plane, material_3));
			my_scene.All_Object[3]->Zoom(Vector3f(20, 1, 40));
			my_scene.All_Object[3]->MoveTo(Vector3f(10, 0, 0));
			my_scene.AddObject(new Object(plane, material_3));
			my_scene.All_Object[4]->Zoom(6);
			my_scene.All_Object[4]->RotateY(30);
			my_scene.All_Object[4]->MoveTo(Vector3f(0, 3.5, 0));
			my_scene.AddObject(new Object(plane, material_3));
			my_scene.All_Object[5]->Zoom(6);
			my_scene.All_Object[5]->RotateY(-30);
			my_scene.All_Object[5]->MoveTo(Vector3f(-10, 3.5, -10));
			my_scene.AddObject(new Object(ball, material_5));
			my_scene.All_Object[6]->Zoom(3);
			my_scene.All_Object[6]->RotateY(30);
			my_scene.All_Object[6]->MoveTo(Vector3f(10, 3.5, 10));
			//2.light
			my_scene.AddLight(light_point1);
			my_scene.AddLight(light_point2);
			//3.sky
			my_scene.AddSkyBox(my_sky_bolonga);
			my_scene.AddSkyBox(my_sky_kyoto);

		//E.初始化RHI-应当交给渲染线程
			resources.InitRHI();

		//F.延迟渲染器
			my_renders.push_back(new DeferredRender(10));
			my_renders[0]->InitRHI();
		}


		int shutdown() override
		{
			imguiDestroy();

			resources.ReleaseRHI();
			my_renders[0]->ReleaseRHI();

			bgfx::shutdown();

			return 0;
		}

		bool update() override
		{
			if (!entry::processEvents(my_camera.width, my_camera.height, m_debug, m_reset, &m_mouseState))
			{
				imguiBeginFrame(m_mouseState.m_mx
					, m_mouseState.m_my
					, (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
					| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
					, m_mouseState.m_mz
					, uint16_t(my_camera.width)
					, uint16_t(my_camera.height)
				);
				showExampleDialog(this);
				//按键操作帮助
				ImGui::SetNextWindowPos(
					ImVec2(0, my_camera.height * 0.5f * (1.0 + scene_ratio))
					, ImGuiCond_FirstUseEver
				);
				ImGui::SetNextWindowSize(
					ImVec2(my_camera.width * scene_ratio, my_camera.height * 0.5f * (1.0 - scene_ratio))
					, ImGuiCond_FirstUseEver
				);
				ImGui::Begin("Help", NULL, 0);
				ImGui::Text("1.Keyboard:\tWASD-move_camera\t\tSpace-change_skybox\n2.Mouse:\tLeftMouse-pickup_object\t\tRightMouse-rotate_camera\t\tMiddleMouse-move_camera");
				ImGui::End();

				imguiEndFrame();

				//相机操作相关
				if (m_mouseState.m_buttons[entry::MouseButton::Right] && m_mouseState_pre.m_buttons[entry::MouseButton::Right])
				{
					my_camera.TurnLeft((m_mouseState_pre.m_mx - m_mouseState.m_mx) * mouse_rotate_speed);
					my_camera.TurnUp((m_mouseState_pre.m_my - m_mouseState.m_my) * mouse_rotate_speed);
				}
				my_camera.GoForward((m_mouseState.m_mz - m_mouseState_pre.m_mz) * mouse_move_speed);
				
				//左键拾取
				if (m_mouseState.m_buttons[entry::MouseButton::Left] && !m_mouseState_pre.m_buttons[entry::MouseButton::Left])
				{
					if (object_debug != NULL)
					{
						object_debug = NULL;
					}
					else
					{
						//转换屏幕坐标
						float dx = m_mouseState.m_mx / (scene_ratio * my_camera.width);
						float dy = (0.5f * (1.0f + scene_ratio) * my_camera.height - m_mouseState.m_my) / (scene_ratio * my_camera.height);
						dx = dx * 2 - 1;
						dy = dy * 2 - 1;

						Vector3f inter_point(dx, dy, -1);
						Vector3f world_point = my_camera.GetVPMatrix().inverse().eval() * inter_point;

						Intersection inter = my_scene.GetIntersectionWithOutBVH(Ray(my_camera.ViewPoint, (world_point - my_camera.ViewPoint).normalized()));
						if (inter.happened)object_debug = inter.object;
					}
				}

				//延迟渲染
				my_renders[0]->Render(my_scene, my_camera, scene_ratio, object_debug);

				CopyMouseState(m_mouseState, m_mouseState_pre);
				return true;
			}
			return false;
		}

		entry::MouseState m_mouseState;
		entry::MouseState m_mouseState_pre;
		float mouse_rotate_speed = 0.5f;
		float mouse_move_speed = 5;

		uint32_t m_debug;
		uint32_t m_reset;

		Scene my_scene;
		float scene_ratio;
		View my_camera;

		RHIController resources;
		std::vector <DeferredRenderRef> my_renders;

		Object* object_debug;
	};

}
int _main_(int _argc, char** _argv)
{
	Example app("DefferRender", "", "");
	return entry::runApp(&app, _argc, _argv);
}