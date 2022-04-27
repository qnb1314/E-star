/*************************************************

Author:乔南

Date:2022-4.01

Description:Shader类,由于bgfx没有几何着色器，所以只派生出VF即顶点片段着色器

**************************************************/
#pragma once
#include "../RenderResource.h"


class Shader : public RenderResource
{
public:
	bgfx::ProgramHandle m_program;
	virtual void ReleaseRHI() override { bgfx::destroy(m_program); }
};
typedef CountRef<Shader> ShaderRef;


class ShaderVF :public Shader
{
public:
	std::string VertexShaderPath;
	std::string FragmentShaderPath;
	
	ShaderVF(std::string vs, std::string fs) :VertexShaderPath(vs), FragmentShaderPath(fs) {}

	virtual void InitRHI() override { m_program = loadProgram(VertexShaderPath.c_str(), FragmentShaderPath.c_str()); }
};