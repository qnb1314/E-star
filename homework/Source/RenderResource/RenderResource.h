/*************************************************

Author:����

Date:2022-3.29

Description:��Ⱦ��Դ����,��Ҫ���𴴽�GPU��Դ

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
