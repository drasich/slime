attribute vec3 vertex;
uniform mat4 matrix;

void main(void)
{
    float reciprScaleOnscreen = 0.1; 

    float w = (matrix * vec4(0,0,0,1)).w;
    w *= reciprScaleOnscreen;

    gl_Position = matrix * vec4(vertex.xyz * w , 1);  

  //gl_Position = matrix * vec4(vertex, 1.0);
}

