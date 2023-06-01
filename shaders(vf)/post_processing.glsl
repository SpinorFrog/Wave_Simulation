#version 460
#define PI 3.1415926

layout (location = 1) in vec2 aTexCoord;
layout(rgba32f, binding = 0) uniform image2D data;

uniform int render_option;
uniform int classical;
vec2 res = vec2(1024, 1024);

float colVal(vec4 texVec){
   float newCol;
   newCol = 16*dot(texVec.xy, texVec.xy);
   if(bool(classical))
      newCol = 16*texVec.x;
   if(render_option == 1)
      newCol = 16*texVec.x;
   if(render_option == 2)
      newCol = 16*texVec.y;
   return newCol;
}

vec3 calculateNormal(vec4 mCol){
   vec3 normalV = vec3(0);
   vec4 center = mCol;
   vec4 adjL = imageLoad(data, ivec2(aTexCoord*res) + ivec2(-1, 0));
   vec4 adjD = imageLoad(data, ivec2(aTexCoord*res) + ivec2(0, -1));

   float cH = colVal(center);
   float lH = colVal(adjL);
   float dH = colVal(adjD);

   vec3 vecD = vec3(0.0f, cH - dH, 1.0f);
   vec3 vecL = vec3(1.0f, cH - lH, 0.0f);
   normalV = normalize(cross(vecD, vecL));
   return normalV;
}

/*helpful rgb to hsv functions I found online, will credit when I find article again*/

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}//