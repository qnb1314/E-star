$input v_texcoord0

#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texColor,  0);
uniform vec4 u_exposure;

float clamp(float x){return max(0.0f, min(1.0f, x));}
vec3 RadianceToRGB(vec3 r)
{
	float gamma_exp = 1.0f / 2.2f;
	return vec3(pow(clamp(r.x), gamma_exp), pow(clamp(r.y), gamma_exp), pow(clamp(r.z), gamma_exp));
}

void main()
{
	vec3 radiance_hdr = texture2D(s_texColor, v_texcoord0).xyz;
	float exposure = u_exposure.x;
	vec3 radiance = vec3_splat(1.0) - exp(-radiance_hdr * exposure);
	gl_FragColor = vec4(RadianceToRGB(radiance), 1.0f);
}
