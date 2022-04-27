$input v_texcoord0

#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texDepthCW,  0);
SAMPLER2D(s_texDepthCCW,  1);

void main()
{
	float depth_cw = texture2D(s_texDepthCW, v_texcoord0).x;
	float depth_ccw = texture2D(s_texDepthCCW, v_texcoord0).x;
	gl_FragDepth = 0.5 * (depth_cw + depth_ccw);
}
