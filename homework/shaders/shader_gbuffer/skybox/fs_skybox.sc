$input v_modelpos
#include "../../bgfx/examples/common/common.sh"

SAMPLERCUBE(s_texCube,0);

void main()
{
	vec3 color = toLinear(textureCubeLod(s_texCube, v_modelpos, 0)).xyz;
	gl_FragData[3] = vec4(color, 1.0f);
	gl_FragData[4] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}
