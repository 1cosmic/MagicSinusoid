// Processor of key press, event.

#include "headers/graph.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <iostream>

using namespace std;

extern SDL_Renderer *render;
extern SDL_Rect R_button_Apply;
extern SDL_Rect R_button_Imitate;
extern bool active_button_Apply;
extern bool active_button_Imitate; // check of active button Imitate.

int i;
extern float scaleY;
extern float scaleX;

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
      setDeltaRelatively(deltaMove);
    }

    return true;

  // If mouse click:
  case SDL_MOUSEBUTTONDOWN:

    SDL_GetMouseState(&coordBP.x, &coordBP.y);

    if (SDL_PointInRect(&coordBP, &R_button_Imitate) == SDL_TRUE) {

      active_button_Imitate = (active_button_Imitate) ? false : true;
    }

    // If mouse moving in graphic area.
    if (coordBP.x > 280) {
      moveMouse = true;
      return true;
    }

  case SDL_MOUSEBUTTONUP:

    // Set last draw point as Main. (for save position of moving).
    setCurrentPointAsMain();
    // ...
    // And set up defalult value.
    moveMouse = false;
    active_button_Apply = false;
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
    case SDLK_LEFT:

      if (scaleX > 1)
        if (scaleX > 1)
          scaleX -= 1;

      // Fixing of the bag.
      fixCurrentDrawing();
      return true;

    // if arrow UP is pressed.
    case SDLK_RIGHT:

      if (scaleX < 10)
        if (scaleX >= 1)
          scaleX += 1;

      return true;

      // if arrow DOWN is pressed.
    case SDLK_DOWN:

      if (scaleY > 0.1) {

        if (scaleY > 1) {
          scaleY -= 1;

        } else if (scaleY <= 1) {
          scaleY -= 0.1;
        }
      }

      return true;

    // if arrow UP is pressed.
    case SDLK_UP:

      if (scaleY < 1)
        scaleY += 0.1;

      return true;
    }

  default:
    return true;
  }
}
