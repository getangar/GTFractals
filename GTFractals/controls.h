#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>
#include <commctrl.h>

extern HWND hStatusBar;		// Handle to the status bar
extern HWND hProgressBar;	// Handle to the progress bar

// Function to create the status bar
void CreateStatusBar(HWND);



#endif // !CONTROLS_H
