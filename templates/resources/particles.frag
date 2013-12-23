//#extension GL_ARB_draw_buffers : enable
//#extension GL_ARB_texture_rectangle : enable
//#extension GL_ARB_texture_non_power_of_two : enable

#define EPS 0.001

uniform sampler2D positions;
uniform sampler2D velocities;
uniform vec2 ab;
uniform vec2 cd;
uniform float e;
uniform float alpha;
uniform float randomness;
uniform vec3 suppliedColor;
uniform vec3 rainbowColorFactor;
uniform vec3 colorAddFactor;
uniform vec3 suppliedColorFactor;
varying vec4 texCoord;

void main(void)
{
    vec3 p0 = texture2D( positions, texCoord.st).rgb;
    float jitter = texture2D( positions, texCoord.st).a;

    // scale down for next point calculation
    p0 *= 0.005;
    
    vec3 p1;
    p1.x = sin(p0.y * ab[0]) - p0.z * cos(p0.x * ab[1]);
    p1.y = p0.z * sin(p0.x * cd[0]) - cos(p0.y * cd[1]);
    p1.z = e*sin(p0.x);
    
    // add randomness to keep things from collapsing
    p1 += jitter * randomness;
    
    // speed for color (rainbow mode) otherwise use supplied color
    vec3 rainbowColor = 0.25*abs(p1 - p0);
    vec3 v1 = rainbowColor * rainbowColorFactor + colorAddFactor + suppliedColor * suppliedColorFactor;
    
    // scale back up
    p1 *= 200.0;
    
    //Render to positions texture
    gl_FragData[0] = vec4(p1, jitter);
    //Render to velocities texture
    gl_FragData[1] = vec4(v1, alpha);
}

