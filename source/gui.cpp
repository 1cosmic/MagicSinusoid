#include "headers/graph.hpp"
#include <SDL2/SDL.h>

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include <exception>
#include <filesystem>
#include <iostream>
#include <vector>

using namespace std;
namespace fs = filesystem;

// Daily (0) or night (1) mode.
int visionMode = 1;

// First color - daily, second - nightly.
int colors_background[2][3] = {
    {255, 255, 255},
};
// Color block of buttons.
int colors_buttons[2][3] = {
    {0, 102, 255},
    {255, 204, 0},
};

int colors_buttons_with_active[2][3] = {
    {0, 153, 255},
    {255, 255, 0},
};

const Uint32 FPS = 60; // FPS.
Uint32 _FPS_Timer;     // for control of frame rate.

SDL_Window *window = NULL;       // Main window of the window window
SDL_Renderer *render = NULL;     // Renderer of the drawing.
SDL_Surface *mainSurface = NULL; // Surface contained by the window.
SDL_Rect R_backbround;           // rect of background.
SDL_Rect R_ControlPanel;         // rect of control panel.
SDL_Rect R_button_Apply;         // rect of button apply UI.
SDL_Rect R_button_Imitate;       // rect of button stat imitate.

bool active_button_Apply = false;   // check of active button.
bool active_button_Imitate = false; // check of active button Imitate.
// Dirs for loading.
const fs::path cur_path =
    fs::current_path().parent_path();         // get cur/parent path.
const fs::path src_path = cur_path / "fonts"; // images path.

// TTF loading.
vector<TTF_Font *> fonts;
vector<SDL_Texture *> texts_InputData;
vector<SDL_Texture *> texts_Imitation;
vector<SDL_Texture *> texts_ImportExport;

TTF_Font *loadFont(string fontName, int size) {
  // Loading fonts in RAM fot next drawing.

  // Set up font path.
  fontName = src_path / fontName;

  // Load in local stack this font & set up of size.
  TTF_Font *font = TTF_OpenFont(fontName.c_str(), size);
  // Check of error with loading.
  if (font == nullptr) {
    cout << "ERROR. Font " << fontName.c_str() << " is not loaded. Pass."
         << endl;
    return nullptr;
  }

  cout << "Font " << fontName.c_str() << " is loaded." << endl;

  return font;
}

SDL_Texture *createText(string text, TTF_Font *font, SDL_Color color) {
  // Save text with size & color in RAM how texture.

  // Create surface with text & set up his color.
  SDL_Surface *surfWithText = TTF_RenderUTF8_Blended(font, text.c_str(), color);

  // Check of error with creating of surface.
  if (surfWithText == nullptr) {
    cout << "ERROR. For text |" << text.c_str() << "| is not create surface. "
         << SDL_GetError() << endl;
    return nullptr;
  }

  // Create texture. Finally.
  SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surfWithText);

  // Check of error with creating of surface.
  if (texture == nullptr) {
    cout << "ERROR. For text |" << text.c_str()
         << "| is not create texture. Pass." << endl;
    return nullptr;
  }

  // Free RAM.
  SDL_FreeSurface(surfWithText);

  return texture;
}

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

  if (TTF_Init() != 0) {
    cout << "Не удалось инициализировать шрифт-систему." << endl;

  } else {

    // Load int KTFJermilov-Solid.ttf
    fonts.push_back(loadFont("KTFJermilov-Solid.ttf", 40));

    // TEXT SAVER.
    //============================================================
    //============================================================

    // Create test text.
    SDL_Color test = {0, 0, 0, 255};
    texts_InputData.push_back(createText("Ввод данных", fonts[0], test));
    texts_InputData.push_back(createText("Применить", fonts[0], test));

    texts_Imitation.push_back(createText("Имитация", fonts[0], test));
    texts_Imitation.push_back(createText("Запустить", fonts[0], test));
    texts_Imitation.push_back(createText("Остановить", fonts[0], test));

    texts_ImportExport.push_back(createText("Импорт", fonts[0], test));
    texts_ImportExport.push_back(createText("Экспорт", fonts[0], test));
    //============================================================
    //============================================================
  }

  //====================
  // Initialize RECTS.
  //====================

  // Init background rect.
  R_backbround.x = 0;
  R_backbround.y = 0;
  R_backbround.w = 1280;
  R_backbround.h = 720;

  R_ControlPanel.x = 0;
  R_ControlPanel.y = 0;
  R_ControlPanel.w = 280;
  R_ControlPanel.h = 720;

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

    // Close all loaded fonts.
    for (vector<TTF_Font *>::iterator i = fonts.begin(); i != fonts.end(); ++i)
      TTF_CloseFont(*i);

    fonts.clear();

    cout << "All fonts was closed." << endl;

  }

  catch (exception &e) {
    cout << "Coudn`t close window. Error: " << e.what() << endl;
  }
}

bool drawBlock_imitation() {
  // Draw block od input values Amplitude & Mz.

  SDL_Rect R_block;
  SDL_Point decorate[5];

  R_block.w = R_ControlPanel.w * 0.9;
  R_block.h = R_ControlPanel.h / 4;
  R_block.x = (R_ControlPanel.w - R_block.w) / 2;
  R_block.y = R_button_Apply.y + R_block.h / 2;

  // Draw Text.
  SDL_Rect R_label;

  // Draw Text.
  SDL_QueryTexture(texts_Imitation[0], NULL, NULL, &R_label.w, &R_label.h);

  // Block of drawing the decorate.
  int decorate_w = R_label.w;
  int decorate_h = R_block.h;

  R_label.w *= 0.9;
  R_label.h *= 0.9;
  R_label.x = (R_ControlPanel.w - R_label.w) / 2;
  R_label.y = R_block.y;

  decorate[0].x = R_label.x - (decorate_w - R_label.w) / 2;
  decorate[0].y = R_label.y + R_label.h / 2;

  decorate[1].x = decorate[0].x + decorate_w;
  decorate[1].y = decorate[0].y;

  decorate[2].x = decorate[1].x;
  decorate[2].y = decorate[1].y + R_button_Apply.h + decorate_h / 3;

  decorate[3].x = decorate[0].x;
  decorate[3].y = decorate[2].y;

  decorate[4].x = decorate[0].x;
  decorate[4].y = decorate[0].y;

  SDL_RenderDrawLines(render, decorate, 5);
  //============================================================
  //============================================================
  //============================================================

  R_button_Imitate.w = R_label.w;
  R_button_Imitate.h = R_label.h;
  R_button_Imitate.x = R_label.x;
  R_button_Imitate.y = R_label.y + R_button_Imitate.h + 20;

  // Display the label.
  SDL_RenderFillRect(render, &R_label);
  SDL_RenderCopy(render, texts_Imitation[0], NULL, &R_label);

  int *c = colors_buttons[visionMode];

  // Display the button.
  SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
  SDL_RenderFillRect(render, &R_button_Imitate);

  // If imitated = stop, else -
  if (active_button_Imitate)
    SDL_RenderCopy(render, texts_Imitation[2], NULL, &R_button_Imitate);
  else
    SDL_RenderCopy(render, texts_Imitation[1], NULL, &R_button_Imitate);
}

bool drawBlock_input() {
  // Draw block od input values Amplitude & Mz.

  SDL_Rect R_block;
  SDL_Point decorate[5];

  R_block.w = R_ControlPanel.w * 0.9;
  R_block.h = R_ControlPanel.h / 3;
  R_block.x = (R_ControlPanel.w - R_block.w) / 2;
  R_block.y = R_block.h / 2;

  // Draw Text.
  SDL_Rect R_label;

  // Draw Text.
  SDL_QueryTexture(texts_InputData[0], NULL, NULL, &R_label.w, &R_label.h);

  // Block of drawing the decorate.
  int decorate_w = R_label.w;
  int decorate_h = R_block.h;

  R_label.w *= 0.9;
  R_label.h *= 0.9;
  R_label.x = (R_ControlPanel.w - R_label.w) / 2;
  R_label.y = R_block.y;

  decorate[0].x = R_label.x - (decorate_w - R_label.w) / 2;
  decorate[0].y = R_label.y + R_label.h / 2;

  decorate[1].x = decorate[0].x + decorate_w;
  decorate[1].y = decorate[0].y;

  decorate[2].x = decorate[1].x;
  decorate[2].y = decorate[1].y + decorate_h;

  decorate[3].x = decorate[0].x;
  decorate[3].y = decorate[2].y;

  decorate[4].x = decorate[0].x;
  decorate[4].y = decorate[0].y;

  SDL_RenderDrawLines(render, decorate, 5);
  //============================================================
  //============================================================
  //============================================================

  SDL_Rect R_field_Mz;
  SDL_Rect R_field_Amplitude;

  R_field_Mz.w = R_label.w;
  R_field_Mz.h = 30;
  R_field_Mz.x = R_label.x;
  R_field_Mz.y = R_label.y + R_label.h + 40;

  R_field_Amplitude.w = R_field_Mz.w;
  R_field_Amplitude.h = R_field_Mz.h;
  R_field_Amplitude.x = R_label.x;
  R_field_Amplitude.y = R_field_Mz.y + R_field_Mz.h + 10;

  SDL_RenderFillRect(render, &R_field_Mz);
  SDL_RenderFillRect(render, &R_field_Amplitude);

  R_button_Apply.w = R_label.w;
  R_button_Apply.h = R_label.h;
  R_button_Apply.x = R_label.x;
  R_button_Apply.y = R_label.y + decorate_h - R_button_Apply.h + 10;

  SDL_RenderFillRect(render, &R_label);
  SDL_RenderCopy(render, texts_InputData[0], NULL, &R_label);

  int *c;

  if (active_button_Apply)
    c = colors_buttons_with_active[visionMode];
  else
    c = colors_buttons[visionMode];

  SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
  SDL_RenderFillRect(render, &R_button_Apply);

  SDL_RenderCopy(render, texts_InputData[1], NULL, &R_button_Apply);
}

bool showMain(void) {
  // Set up default option of main screen & window. Show is its.

  int c1, c2, c3;

  // Clear render.
  SDL_RenderClear(render);

  c1 = colors_background[visionMode][0];
  c2 = colors_background[visionMode][1];
  c3 = colors_background[visionMode][2];

  // Clear & set main options.
  SDL_RenderSetViewport(render, &R_backbround);
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(render, &R_backbround);

  c1 = colors_background[0][0];
  c2 = colors_background[0][1];
  c3 = colors_background[0][2];

  // Draw block of input data.
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);
  drawBlock_input();
  drawBlock_imitation();

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

void drawGraph() { drawAxis(visionMode); }

void setVisionMode(bool mode) {
  // Set up vision mode.
  if (mode)
    visionMode = 1;
  else
    visionMode = 0;
}
