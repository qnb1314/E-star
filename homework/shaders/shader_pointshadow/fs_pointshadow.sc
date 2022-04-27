$input v_worldpos
#include "../../bgfx/examples/common/common.sh"

uniform vec4 lightinfo;
void main()
{
	vec3 lightpoint = lightinfo.xyz;
	float far = lightinfo.w;
	float lightDistance = length(v_worldpos - lightpoint);
	gl_FragDepth = lightDistance / far;
}
