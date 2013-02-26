uniform float test;
uniform int wireframe;

varying vec4 diffuse,ambient;
varying vec3 eye_normal,lightDir,halfway;
varying vec2 f_texcoord;
uniform sampler2D texture;
varying vec3 bc;

//#extension GL_OES_standard_derivatives : enable
float edgeFactor(){
    vec3 d = fwidth(bc);
    vec3 a3 = smoothstep(vec3(0.0), d*1.0, bc);
    return min(min(a3.x, a3.y), a3.z);
}

void main (void)
{
  vec3 n,halfv;
	float NdotL,NdotHV;
  vec4 specular = vec4(1.0,1.0,1.0,1);

	vec4 color = ambient;
  vec4 diffuse_tex = texture2D(texture, f_texcoord);
  //vec4 diffuse_tex = vec4(0.2, 0.2, 0.8,1);
	n = normalize(eye_normal);

	NdotL = max(dot(n,-lightDir),0.0);

  float shininess = 100.0;

  if (NdotL > 0.0) {
    color += diffuse_tex * NdotL;
    halfv = normalize(halfway);
    NdotHV = max(dot(n,halfv),0.0);
    color += specular * pow(NdotHV, shininess);
  }

  if (wireframe == 1) {
    float near_edge = (1.0-edgeFactor())*0.95;
    if (near_edge > 0.5) {
      gl_FragColor = vec4(1.0, 0.6, 0.0, near_edge);
    }
    else {
      gl_FragColor = color;
    }
  } else {
    gl_FragColor = color;
  }

}

