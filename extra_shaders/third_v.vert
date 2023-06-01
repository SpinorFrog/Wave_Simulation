#version 460 core
#define PI 3.1415926
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec4 TexCol;
out vec3 normal;
out vec3 lightS;
out vec3 modPos;
out vec3 viewP;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform mat4 mvp;
uniform float time;
vec3 light = vec3(0, 60, 0);

vec2 res = vec2(1024, 1024);

vec3 calculateNormal(vec4 mCol);
float colVal(vec4 texVec);

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
}

void main()
{
   vec4 texColor = texture(tex, aTexCoord);
   float t = colVal(texColor);
   modPos = vec3(aPos.x, t+50, aPos.z);
   //vec3 modPos = vec3(aPos.x, 5*texColor.x, aPos.z);
   TexCoord = aTexCoord;
   vec4 color = vec4(0.0039, 0.9216, 0.9882, 1.0);



   /*if(texColor.x < 0)
      color = vec4(0, color.y, 1.2+texColor.x, 1);
   else if(texColor.x < 4)
      color = vec4(texColor.x,color.y, 1, 1);*/
   float phase = atan(texColor.y, texColor.x);
   float norm = length(texColor.xy);
   if(norm < 0.001)
      phase = 0;
   vec3 hsv = rgb2hsv(color.xyz);
   hsv.x = mod(hsv.x + phase/(2*PI), 1);
   hsv.y = 1.4*norm;

   hsv = hsv2rgb(hsv);

   color.xyz = mix(hsv, color.xyz, 0.2);
   //color = vec4(modPos.y/10, color.y, 0.5, 1);
   //color = vec4(abs(phase.x*color.x - phase.y*color.z), color.y, abs(phase.y*color.x + phase.x*color.z), 1.0);

    TexCol = color;
    normal = calculateNormal(texColor);
    lightS = normalize(light - modPos);
    viewP = viewPos;

    if(norm > 0.004)
        gl_Position = mvp*vec4(modPos, 1);
    else
        gl_Position = vec4(0*9999999, 0, 9999999, 1);
}

float colVal(vec4 texVec){
   float newCol;
   //newCol = 20*dot(texVec.xy, texVec.xy);
   newCol = 10*texVec.y;
   return newCol;
}

vec3 calculateNormal(vec4 mCol){
   vec3 normalV = vec3(0);
   vec4 center = mCol;
   vec4 adjL = texture(tex, aTexCoord + vec2(-1.0f, 0.0f)/res);
   vec4 adjD = texture(tex, aTexCoord + vec2(0.0f, -1.0f)/res);

   float cH = colVal(center);
   float lH = colVal(adjL);
   float dH = colVal(adjD);

   vec3 vecD = vec3(0.0f, cH - dH, 1.0f);
   vec3 vecL = vec3(1.0f, cH - lH, 0.0f);
   normalV = normalize(cross(vecD, vecL));
   return normalV;
}////