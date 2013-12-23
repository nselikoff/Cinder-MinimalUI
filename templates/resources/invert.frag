// contents of the FBO
uniform sampler2D tex;

void main(void)
{
	vec2 texCoord	= gl_TexCoord[0].st;
    vec4 color = texture2D(tex, texCoord);
    
    // invert
    color.r = 1.0 - color.r;
    color.g = 1.0 - color.g;
    color.b = 1.0 - color.b;
    
    gl_FragColor = color;
}