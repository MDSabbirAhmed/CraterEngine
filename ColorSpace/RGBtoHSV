#include <cmath> // For math functions

// Convert RGB color to HSV color
void rgbToHsv(float r, float g, float b, float& h, float& s, float& v) {
    float cmax = fmax(fmax(r, g), b); // Maximum color component
    float cmin = fmin(fmin(r, g), b); // Minimum color component
    float delta = cmax - cmin; // Delta between max and min

    // Calculate hue
    if (delta == 0) {
        h = 0; // No color difference, hue is 0
    } else if (cmax == r) {
        h = fmod((g - b) / delta, 6.0f);
    } else if (cmax == g) {
        h = (b - r) / delta + 2.0f;
    } else {
        h = (r - g) / delta + 4.0f;
    }
    h *= 60.0f;

    // Calculate value
    v = cmax;

    // Calculate saturation
    if (v == 0) {
        s = 0; // If value is 0, saturation is 0
    } else {
        s = delta / v;
    }
}



//example
    // Set background color to red
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Convert red RGB color to HSV
    float r = 1.0f, g = 0.0f, b = 0.0f;
    float h, s, v;
    rgbToHsv(r, g, b, h, s, v);

    // Output HSV values to console
    printf("HSV: %f %f %f\n", h, s, v);
