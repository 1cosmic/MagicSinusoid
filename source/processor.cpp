// Processor of key press, event.

#include "headers/graph.hpp"
#include "headers/gui.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;

extern SDL_Renderer *render;
extern SDL_Rect R_button_Apply;
extern SDL_Rect R_button_Imitate;
extern bool active_button_Apply;
extern bool active_button_Imitate; // check of active button Imitate.
extern SDL_Rect R_field_Mz;        // rect of UI Mz.
extern SDL_Rect R_field_Amplitude; // rect of Amplitude.

int i;
extern float scaleY;
extern float scaleX;

extern int Mz;
extern int Amplitude;
int value_Mz = 10;
int value_A = 1000;
bool lastChanges;

SDL_Point moveCursor; // if mouse is moving.
SDL_Point deltaMove;  // delta betwenn move Cursor & coord of Button Pressed.
SDL_Point coordBP;    // if mouse button is pressed - write coords (for move).
string UI_Mz;         // User Input (UI) for Mz.
string UI_Amplitude;  // UI for Amplitude.
string *UI;           // pointer in two up lines.
char *text;
bool UI_run = false; // UI is start?
int UI_count;

void limitValue() {
  // Check of UI, limit values & update display data.
  // For enter & auto checker.

  if (UI == &UI_Amplitude || UI == &UI_Mz) {
    UI_run = false;
    SDL_StopTextInput();

    // Auto - limitation the value.
    if (!UI->empty()) {

      // Save ONLY numbers.

      if (UI == &UI_Mz) {
        value_Mz = stoi(*UI, nullptr);

        if (value_Mz > 100)
          value_Mz = 100;
        else if (value_Mz == 0)
          value_Mz = 1;

        UI->clear();
        UI->append(to_string(value_Mz));

      } else if (UI == &UI_Amplitude) {
        value_A = stoi(*UI, nullptr);

        if (value_A > 10000)
          value_A = 10000;
        else if (value_A == 0)
          value_A = 1;

        UI->clear();
        UI->append(to_string(value_A));
      }

      lastChanges = true;
      changeUI();
    }
  }
}

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

    // If mouse moving in graphic area.
    if (coordBP.x > 280) {
      moveMouse = true;
      return true;
    }

    // Check out pressed button.
    if (SDL_PointInRect(&coordBP, &R_button_Imitate) == SDL_TRUE) {
      active_button_Imitate = (active_button_Imitate) ? false : true;
    }

    // UI in field.
    // ************************************************************
    // ************************************************************
    // Turn of UI saver.

    if (UI_run) {
      limitValue();
    }

    if (SDL_PointInRect(&coordBP, &R_field_Amplitude) == SDL_TRUE) {
      UI_run = true;
      UI_count = 5;

      UI = &UI_Amplitude;
      UI->clear();
      SDL_StartTextInput();
      cout << "UI Amplitude: " << endl;
      return true;
    }

    if (SDL_PointInRect(&coordBP, &R_field_Mz) == SDL_TRUE) {
      UI_run = true;
      UI_count = 3;

      UI = &UI_Mz;
      UI->clear();
      SDL_StartTextInput();

      cout << "UI Mz: " << endl;
      return true;
    }
    if (SDL_PointInRect(&coordBP, &R_button_Apply) == SDL_TRUE) {
      Mz = value_Mz;
      Amplitude = value_A;

      cout << "Set up new values." << endl;
      cout << "Mz: " << Mz << endl;
      cout << "Amplitude: " << Amplitude << endl;
      return true;
    }

    // ************************************************************
    // ************************************************************
    return true;

  case SDL_MOUSEBUTTONUP:

    // Set last draw point as Main. (for save position of moving).
    setCurrentPointAsMain();
    // ...
    // And set up defalult value.
    moveMouse = false;
    active_button_Apply = false;
    deltaMove.x = deltaMove.y = 0;

    return true;

  // UI when field of Input is pressed.
  case SDL_TEXTINPUT:
    // Save priority is numbers.

    if (UI == &UI_Amplitude || UI == &UI_Mz)
      if (UI->size() < UI_count)
        if (*event.text.text >= '0' && *event.text.text <= '9')
          UI->push_back(*event.text.text);

    return true;

  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    // Close game by key: SPACE.
    case SDLK_q:
      return false;

    case SDLK_BACKSPACE:
      if (!UI->empty())
        UI->pop_back();
      return true;

      // Start / stop imitation of signal.
    case SDLK_SPACE:
      active_button_Imitate = (active_button_Imitate) ? false : true;
      cout << "Debug." << endl;
      return true;

    case SDLK_RETURN:
      limitValue();

      Mz = value_Mz;
      Amplitude = value_A;

      cout << "Set up new values." << endl;
      cout << "Mz: " << Mz << endl;
      cout << "Amplitude: " << Amplitude << endl;
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
