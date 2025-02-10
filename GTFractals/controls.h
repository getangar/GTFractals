#ifndef CONTROLS_H
#define CONTROLS_H

#include <windows.h>
#pragma comment(lib, "Comctl32.lib")
#include <commctrl.h>

#include <stdio.h>

#include "global.h"


extern HWND hStatusBar;		// Handle to the status bar
extern HWND hProgressBar;	// Handle to the progress bar

// Function to update the status bar
void UpdateStatusBarText();

// Function to create the status bar
void CreateStatusBar(HWND);

// Function to create the toolbar
void CreateToolBar(HWND);

#endif // !CONTROLS_H
