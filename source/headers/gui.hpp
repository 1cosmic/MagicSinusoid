#pragma once

#ifndef _H_GUI
#define _H_GUI

// init main object SDL.
bool initGUI(int SCR_W, int SCR_H);

// Destroy all objects of GUI.
void destroyGUI(void);

// Sugar. Run game & show it.
bool showMain(void);

// Draw window of graph.
void drawGraph();

// Display changes.
void displayChanges();

#endif // !_H_GUI
