// Used for shadow lookup
varying vec2 vTexCoords;

void main()
{
  vTexCoords = gl_MultiTexCoord0.st;

  gl_Position = ftransform();
}
