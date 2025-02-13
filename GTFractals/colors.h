#ifndef COLORS_H
#define COLORS_H

#include <windows.h>

// Color pattern Modern
COLORREF GetModernColor(int, int);

// Color pattern Vintage (VGA 256 color palette)
COLORREF GetVintageColor(int, int);

// Function to generate a smooth VGA gradient using 16 million colors
COLORREF GetSmoothVGAColor(int, int);

// Color pattern Dark Gradient
COLORREF GetDarkGradientColor(int, int);

#endif