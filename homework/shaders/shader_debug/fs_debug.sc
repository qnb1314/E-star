$input v_texcoord0

#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texColor,  0);

void main()
{
	vec3 radiance = texture2D(s_texColor, v_texcoord0).xyz;
	gl_FragColor = vec4(radiance, 1.0f);
}
