#include <GL/gl.h>

void hsvToRgb(float h, float s, float v, GLfloat &r, GLfloat &g, GLfloat &b) {
  int i;
  float f, p, q, t;

  if (s == 0) {
    // achromatic (gray)
    r = g = b = v;
    return;
  }

  h /= 60;            // sector 0 to 5
  i = floor(h);
  f = h - i;          // fractional part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));

  switch (i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:        // case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
}





//example
GLfloat h = 0.5f;  // example hue value
GLfloat s = 0.8f;  // example saturation value
GLfloat v = 1.0f;  // example value/brightness value
GLfloat r, g, b;

hsvToRgb(h, s, v, r, g, b);

glColor3f(r, g, b);  // set the current color to the calculated RGB values
