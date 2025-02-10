#ifndef GLOBALS_H
#define GLOBALS_H

#include "resource.h"

extern double xmin, ymin, xmax, ymax;	// Global variables for Mandelbrot boundaries
extern int max_iter;					// Global variable for iterations
extern double px, py;					// Global variables for Julia set parameters
extern bool isJulia;					// Flag to indicate if Julia set is being displayed
extern int selectedColorPattern;		// Global variable for selected color pattern
extern HWND hToolBar;					// Handle to the toolbar

#endif
