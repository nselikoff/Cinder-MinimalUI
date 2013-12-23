// contents of the FBO
uniform sampler2D tex;

uniform float exposure;
uniform float gamma;

void main(void)
{
	vec2 texCoord	= gl_TexCoord[0].st;
    vec4 color = texture2D(tex, texCoord);
    
    // perform tone mapping
    float Y = dot(vec4(0.30, 0.59, 0.11, 0.0), color);
    
    color *= pow(exposure, 3.1)*log(Y+1.0);

    // normal gamma
    color.r = pow(color.r, gamma);
    color.g = pow(color.g, gamma);
    color.b = pow(color.b, gamma);
    
    gl_FragColor = color;
}