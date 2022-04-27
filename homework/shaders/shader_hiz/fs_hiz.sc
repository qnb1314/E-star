$input v_texcoord0

#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texDepth,  0);
uniform vec4 mapinfo;//curr_level,pre_size_inv

vec4 ReadPreLevel(vec2 vt, int level)
{
	vec2 size_inv = vec2(mapinfo.y, mapinfo.y);
	vec2 x00 = vec2(-1, -1) * size_inv * 0.5 + vt;
	vec2 x10 = vec2(1, -1) * size_inv * 0.5 + vt;
	vec2 x01 = vec2(-1, 1) * size_inv * 0.5 + vt;
	vec2 x11 = vec2(1, 1) * size_inv * 0.5 + vt;
	float d00 = texture2DLod(s_texDepth, x00, level).x;
	float d10 = texture2DLod(s_texDepth, x10, level).x;
	float d01 = texture2DLod(s_texDepth, x01, level).x;
	float d11 = texture2DLod(s_texDepth, x11, level).x;
	
	return vec4(d00, d10, d01, d11);
}

float GetMinDepth(vec2 vt)
{
	int curr_level =int(mapinfo.x);
	if(curr_level == 0)
	{
		return texture2DLod(s_texDepth, vt, curr_level).x;
	}
	else
	{
		vec4 depth = ReadPreLevel(vt, curr_level - 1);
		return min(min(depth.x, depth.y), min(depth.z, depth.w));
	}
}

void main()
{
	gl_FragDepth = GetMinDepth(v_texcoord0);
}
