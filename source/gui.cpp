#include <SDL2/SDL.h>

#include <SDL2/SDL_render.h>
#include <exception>
#include <iostream>
#include <vector>

using namespace std;

const Uint32 FPS = 60; // FPS.
Uint32 _FPS_Timer;     // for control of frame rate.

SDL_Window *window = NULL;       // Main window of the window window
SDL_Renderer *render = NULL;     // Renderer of the drawing.
SDL_Surface *mainSurface = NULL; // Surface contained by the window.

SDL_Rect R_backbround; // rect of background.

bool initGUI(int SCR_W, int SCR_H) {
  // Initialization chesks.

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize. Error: %s\n", SDL_GetError());
    exit(1);

  } else {

    // Create main window:
    SDL_CreateWindowAndRenderer(SCR_W, SCR_H, 0, &window, &render);

    if (window == NULL) {
      printf("Main window could not be created. Error: %s\n", SDL_GetError());

      exit(1);

    } else

      mainSurface = SDL_GetWindowSurface(window);
  }

  //====================
  // Initialize RECTS.
  //====================

  // Init background rect.
  R_backbround.x = 0;
  R_backbround.y = 0;
  R_backbround.w = 1280;
  R_backbround.h = 720;

  return true;
  //====================
}

void destroyGUI(void) {
  // Destroy all objects & close window.

  try {
    if (mainSurface)
      SDL_FreeSurface(mainSurface); // Destroy main (window) surface.
    if (render)
      SDL_DestroyRenderer(render); // Destroy renderer.
    if (window)
      SDL_DestroyWindow(window); // Destroy window.

    cout << "All surface & windows was closed." << endl;
  }

  catch (exception &e) {
    cout << "Coudn`t close window. Error: " << e.what() << endl;
  }
}

bool showMain(void) {
  // Set up default option of main screen & window. Show is its.

  // Clear render.
  SDL_RenderClear(render);

  // Clear & set main options.
  SDL_RenderSetViewport(render, &R_backbround);
  SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(render, &R_backbround);

  return true;
}

void displayChanges() {

  // Frame Rate controls.
  if (SDL_GetTicks() - _FPS_Timer < FPS) {
    SDL_Delay(FPS - SDL_GetTicks() + _FPS_Timer);
    _FPS_Timer = SDL_GetTicks();
  }
  SDL_RenderPresent(render);
}
