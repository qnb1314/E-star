/*************************************************

Author:����

Date:2022-4.01

Description:Shader��,����bgfxû�м�����ɫ��������ֻ������VF������Ƭ����ɫ��

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