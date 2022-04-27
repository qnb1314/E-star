/*************************************************

Author:乔南

Date:2022-3.29

Description:渲染资源基类,主要负责创建GPU资源

**************************************************/
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "../Math/CountRef.h"
#include "bgfx_utils.h"


class RenderResource
{
public:
	virtual void InitRHI() = 0;
	virtual void ReleaseRHI() = 0;
};
