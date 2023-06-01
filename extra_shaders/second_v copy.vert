#version 460 core
#define PI 3.1415926
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform mat4 mvp;
uniform float time;

void main()
{
    float scale = 1.0f;
    vec2 fakePos = scale*32.0f*aPos.xz/512.0f;
    float w = 5;
    float pe = 0.5*w*dot(fakePos, fakePos)/scale;
    //if(pe > 100)
        //pe = 100;
    vec3 modPos = scale*vec3(aPos.x, pe, aPos.z)-vec3(0, 40, 0);
    gl_Position = mvp*vec4(modPos, 1);
}//