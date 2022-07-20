// Processor of key press, event.

#include "headers/graph.hpp"
#include "headers/gui.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <iostream>

using namespace std;

extern SDL_Renderer *render;

int i;
float scaleX = 1;
float scaleY = 1;

SDL_Point moveCursor; // if mouse is moving.
SDL_Point deltaMove;  // delta betwenn move Cursor & coord of Button Pressed.
SDL_Point coordBP;    // if mouse button is pressed - write coords (for move).

static bool moveMouse = false; // detection of move mouse if pressed button.

bool processor(SDL_Event event) {
  // Processor event.

  switch (event.type) {

  // If game was close:
  case SDL_QUIT:
    return false;

  // If mouse is moving:
  case SDL_MOUSEMOTION:

    // If left button pressed:
    if (moveMouse) {
      SDL_GetMouseState(&moveCursor.x, &moveCursor.y);
      deltaMove.x = (coordBP.x - moveCursor.x) / 2;
      deltaMove.y = (coordBP.y - moveCursor.y) / 2;

      if (setDeltaRelatively(deltaMove)) {
        // Show background.
        showMain();

        // Prepare-calc of animated Axis.
        drawAxis();
      }
    }

    return true;

  // If mouse click:
  case SDL_MOUSEBUTTONDOWN:

    i = 0;
    moveMouse = true;
    SDL_GetMouseState(&coordBP.x, &coordBP.y);
    return true;

  case SDL_MOUSEBUTTONUP:

    // Set last draw point as Main. (for save position of moving).
    setCurrentPointAsMain();
    // ...
    // And set up defalult value.
    moveMouse = false;
    deltaMove.x = deltaMove.y = 0;

    return true;

  // If key was pressed:
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {

    // Close game by key: SPACE.
    case SDLK_q:
      return false;

    // Display stars.
    case SDLK_SPACE:
      return true;

      // if arrow DOWN is pressed.
    case SDLK_DOWN:

      if (scaleX > 0.1) {

        if (scaleX < 10 && scaleX >= 1) {
          scaleX -= 1;
          set_ScaleX(scaleX);

        } else if (scaleX < 1) {
          scaleX -= 0.1;
        }
      }

      return true;

    // if arrow UP is pressed.
    case SDLK_UP:

      if (scaleX > 0.1) {

        if (scaleX < 10 && scaleX >= 1) {
          scaleX += 1;
          set_ScaleX(scaleX);

        } else if (scaleX < 1) {
          scaleX += 0.1;
        }
      }

      return true;
    }

  default:
    return true;
  }
}
