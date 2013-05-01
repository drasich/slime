attribute vec3 vertex;
attribute vec4 color;
uniform mat4 matrix;
uniform int size_fixed;

varying vec4 vcolor;

void main(void)
{
  vcolor = color;

  if (size_fixed == 1) {
    // change value to match resolution.    = (2 * ObjectSizeOnscreenInPixels / ScreenWidthInPixels)
    // transform the vector (0,0,0) to clipspace.
    // This will get the W the object's pivot will be divided by, thus you get the "inverseScale" that will be applied to vertices.

    float reciprScaleOnscreen = 0.1; 

    float w = (matrix * vec4(0,0,0,1)).w;
    w *= reciprScaleOnscreen;

    gl_Position = matrix * vec4(vertex.xyz * w , 1);  
  }
  else {
    gl_Position = matrix * vec4(vertex, 1.0);
  }
}

