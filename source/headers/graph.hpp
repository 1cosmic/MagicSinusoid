#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <SDL2/SDL.h>

void initGraph(int SCR_W, int SCR_H);
void set_Mz_Amp(int v_Mz, int v_Amplitude);

void set_ScaleX(float x);
void set_ScaleY(float y);
bool setDeltaRelatively(SDL_Point deltaMove);
void setCurrentPointAsMain();

void drawAxis();

#endif // GRAPH_HPP
