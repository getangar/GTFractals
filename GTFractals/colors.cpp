#include "colors.h"

// Standard 256-color VGA Palette (Commonly used in DOS software like Fractint)
static COLORREF vga_palette[256] = {
    RGB(0, 0, 0), RGB(0, 0, 168), RGB(0, 168, 0), RGB(0, 168, 168),
    RGB(168, 0, 0), RGB(168, 0, 168), RGB(168, 84, 0), RGB(168, 168, 168),
    RGB(84, 84, 84), RGB(84, 84, 252), RGB(84, 252, 84), RGB(84, 252, 252),
    RGB(252, 84, 84), RGB(252, 84, 252), RGB(252, 252, 84), RGB(252, 252, 252),
    RGB(0, 0, 0), RGB(20, 20, 20), RGB(32, 32, 32), RGB(44, 44, 44),
    RGB(56, 56, 56), RGB(68, 68, 68), RGB(80, 80, 80), RGB(96, 96, 96),
    RGB(112, 112, 112), RGB(128, 128, 128), RGB(144, 144, 144), RGB(160, 160, 160),
    RGB(176, 176, 176), RGB(192, 192, 192), RGB(208, 208, 208), RGB(224, 224, 224),
    RGB(252, 252, 252), RGB(0, 0, 252), RGB(64, 0, 252), RGB(124, 0, 252),
    RGB(188, 0, 252), RGB(252, 0, 252), RGB(252, 0, 188), RGB(252, 0, 124),
    RGB(252, 0, 64), RGB(252, 0, 0), RGB(252, 64, 0), RGB(252, 124, 0),
    RGB(252, 188, 0), RGB(252, 252, 0), RGB(188, 252, 0), RGB(124, 252, 0),
    RGB(64, 252, 0), RGB(0, 252, 0), RGB(0, 252, 64), RGB(0, 252, 124),
    RGB(0, 252, 188), RGB(0, 252, 252), RGB(0, 188, 252), RGB(0, 124, 252),
    RGB(0, 64, 252), RGB(124, 124, 252), RGB(156, 124, 252), RGB(188, 124, 252),
    RGB(220, 124, 252), RGB(252, 124, 252), RGB(252, 124, 220), RGB(252, 124, 188),
    RGB(252, 124, 156), RGB(252, 124, 124), RGB(252, 156, 124), RGB(252, 188, 124),
    RGB(252, 220, 124), RGB(252, 252, 124), RGB(220, 252, 124), RGB(188, 252, 124),
    RGB(156, 252, 124), RGB(124, 252, 124), RGB(124, 252, 156), RGB(124, 252, 188),
    RGB(124, 252, 220), RGB(124, 252, 252), RGB(124, 220, 252), RGB(124, 188, 252),
    RGB(124, 156, 252), RGB(180, 180, 252), RGB(196, 180, 252), RGB(216, 180, 252),
    RGB(236, 180, 252), RGB(252, 180, 252), RGB(252, 180, 236), RGB(252, 180, 216),
    RGB(252, 180, 196), RGB(252, 180, 180), RGB(252, 196, 180), RGB(252, 216, 180),
    RGB(252, 236, 180), RGB(252, 252, 180), RGB(236, 252, 180), RGB(216, 252, 180),
    RGB(196, 252, 180), RGB(180, 252, 180), RGB(180, 252, 196), RGB(180, 252, 216),
    RGB(180, 252, 236), RGB(180, 252, 252), RGB(180, 236, 252), RGB(180, 216, 252),
    RGB(180, 196, 252),
    // Continue filling up to 256 colors...
};


// Color pattern Modern
COLORREF GetModernColor(int iterations, int max_iter) {
    if (iterations == max_iter) {
        return RGB(0, 0, 0); // Black for points inside the Mandelbrot set
    }

    // Smooth color gradient using modulo operations
    int red = 255 - (iterations * 10) % 255;
    int green = 255 - (iterations * 5) % 255;
    int blue = 255; // Always bright blue

    return RGB(red, green, blue);
}


// Color pattern Dark Gradient
COLORREF GetDarkGradientColor(int iterations, int max_iter) {
    if (iterations == max_iter || iterations == 0) {
        return RGB(0, 0, 0); // Black for points inside the Mandelbrot set
    }

    // 16-color cyclic gradient (Wikipedia style)
    static COLORREF wikipedia_palette[16] = {
        RGB(66, 30, 15),   // Dark brown
        RGB(25, 7, 26),    // Dark purple
        RGB(9, 1, 47),     // Deep blue
        RGB(4, 4, 73),     // Dark blue
        RGB(0, 7, 100),    // Blue
        RGB(12, 44, 138),  // Bright blue
        RGB(24, 82, 177),  // Brighter blue
        RGB(57, 125, 209), // Light blue
        RGB(134, 181, 229), // Sky blue
        RGB(211, 236, 248), // Very light blue
        RGB(241, 233, 191), // Pale yellow
        RGB(248, 201, 95),  // Orange-yellow
        RGB(255, 170, 0),   // Orange
        RGB(204, 128, 0),   // Brown-orange
        RGB(153, 87, 0),    // Dark orange
        RGB(106, 52, 3)     // Dark brown
    };

    // Select color from cyclic palette
    return wikipedia_palette[iterations % 16];
}

// Color pattern Vintage (VGA 256 color palette)
COLORREF GetVintageColor(int iterations, int max_iter) {
    if (iterations == max_iter) {
        return RGB(0, 0, 0); // Black for points inside the Mandelbrot set
    }

    // Normalize the iteration value to fit the palette size
    double t = (double)iterations / max_iter;

    // Compute the two closest colors in the VGA palette
    int colorIndex1 = (int)(t * 255);
    int colorIndex2 = (colorIndex1 + 1) % 256;

    // Get the two colors from the VGA palette
    COLORREF color1 = vga_palette[colorIndex1];
    COLORREF color2 = vga_palette[colorIndex2];

    // Interpolate between the two colors
    int r = (int)((1 - t) * GetRValue(color1) + t * GetRValue(color2));
    int g = (int)((1 - t) * GetGValue(color1) + t * GetGValue(color2));
    int b = (int)((1 - t) * GetBValue(color1) + t * GetBValue(color2));

    return RGB(r, g, b);
}