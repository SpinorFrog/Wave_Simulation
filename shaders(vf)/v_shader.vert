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
uniform int render_option;

vec3 light = vec3(0, 60, 0);

vec2 res = vec2(1024, 1024);

vec3 calculateNormal(vec4 mCol);
float colVal(vec4 texVec);
vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);

void main()
{
   vec4 texColor = texture(tex, aTexCoord);
   float t = colVal(texColor);
   modPos = vec3(aPos.x, t+ 10*int(bool(render_option)) + 20*render_option, aPos.z);
   TexCoord = aTexCoord;
   vec4 color = vec4(0.0039, 0.9216, 0.9882, 1.0);

   float phase = atan(texColor.y, texColor.x);
   float norm = length(texColor.xy);
   if(norm < 0.001)
      phase = 0;
   vec3 hsv = rgb2hsv(color.xyz);
   hsv.x = mod(hsv.x + phase/(2*PI), 1);
   hsv.y = 1.4*norm;

   hsv = hsv2rgb(hsv);

   color.xyz = mix(hsv, color.xyz, 0.2);

   TexCol = color;
   normal = calculateNormal(texColor);
   lightS = normalize(light - modPos);
   viewP = viewPos;

   //gl_Position = mvp*vec4(modPos, 1);
   if(render_option > 0){
      if(norm > 0.004)
         gl_Position = mvp*vec4(modPos, 1);
      else
         gl_Position = vec4(0, 0, 9999999, 1);
   }
   else
      gl_Position = mvp*vec4(modPos, 1);
}//