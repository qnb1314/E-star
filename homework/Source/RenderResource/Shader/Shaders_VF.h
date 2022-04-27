#pragma once
#include "Shader.h"

class PointShadowShader :public ShaderVF
{
public:
	PointShadowShader() :ShaderVF("vs_pointshadow", "fs_pointshadow") {}
};

class DepthBlendShader :public ShaderVF
{
public:
	DepthBlendShader() :ShaderVF("vs_depthblend", "fs_depthblend") {}
};

class DepthShader :public ShaderVF
{
public:
	DepthShader() :ShaderVF("vs_depth", "fs_depth") {}
};

class HiZShader :public ShaderVF
{
public:
	HiZShader() :ShaderVF("vs_hiz", "fs_hiz") {}
};

class SkyBoxShader :public ShaderVF
{
public:
	SkyBoxShader() :ShaderVF("vs_skybox", "fs_skybox") {}
};

class GbufferShader :public ShaderVF
{
public:
	GbufferShader() :ShaderVF("vs_gbuffer", "fs_gbuffer") {}
};

class IBLShader :public ShaderVF
{
public:
	IBLShader() :ShaderVF("vs_ibl", "fs_ibl") {}
};

class PointLightShader :public ShaderVF
{
public:
	PointLightShader() :ShaderVF("vs_pointlight", "fs_pointlight") {}
};

class SSRShader :public ShaderVF
{
public:
	SSRShader() :ShaderVF("vs_ssr", "fs_ssr") {}
};

class PostShader :public ShaderVF
{
public:
	PostShader() :ShaderVF("vs_post", "fs_post") {}
};

class DebugShader :public ShaderVF
{
public:
	DebugShader() :ShaderVF("vs_debug", "fs_debug") {}
};

class DebuglineShader :public ShaderVF
{
public:
	DebuglineShader() :ShaderVF("vs_debugline", "fs_debugline") {}
};