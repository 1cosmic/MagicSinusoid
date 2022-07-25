#include "headers/graph.hpp"
#include <SDL2/SDL.h>

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

extern string UI_Mz;        // User Input (UI) for Mz.
extern string UI_Amplitude; // UI for Amplitude.
extern string *UI;          // UI pointer.
extern bool UI_run;         // UI is changed?
extern bool lastChanges;    // if values was limited.
extern int cursorX;         // cur.cursor X.
extern float U;
extern float T;

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
    {200, 200, 200},
};

SDL_Color colors_texts_negative[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
SDL_Color colors_texts[2] = {{255, 255, 255, 255}, {0, 0, 0, 255}};

const Uint32 FPS = 60; // FPS.
Uint32 _FPS_Timer;     // for control of frame rate.

SDL_Window *window = NULL;         // Main window of the window window
SDL_Renderer *render = NULL;       // Renderer of the drawing.
SDL_Surface *mainSurface = NULL;   // Surface contained by the window.
vector<SDL_Texture *> gC_textrure; // grub collector for destroy texture.
SDL_Rect R_backbround;             // rect of background.
SDL_Rect R_ControlPanel;           // rect of control panel.
SDL_Rect R_block_UI;               // block of UI.
SDL_Rect R_block_curValues;        // block of cur. values.
SDL_Rect decorate_TTH;
bool displayTTH;          // display TTH?
SDL_Rect R_label_U;       // label of U.
SDL_Rect R_label_T;       // label of T.
SDL_Point decorate_UI[5]; // decoration of UI.

SDL_Rect R_button_Apply;   // rect of button apply UI.
SDL_Rect R_button_Imitate; // rect of button stat imitate.
SDL_Rect R_label_bA;       // label of UI.
SDL_Rect R_label_UI;       // label of UI.

SDL_Rect R_field_Mz;        // field of UI Mz.
SDL_Rect R_field_Amplitude; // field of Amplitude.
SDL_Rect R_label_A;         // label of A.
SDL_Rect R_label_Mz;        // label of Mz.

SDL_Rect R_data_A;  // user data of A.
SDL_Rect R_data_Mz; // user data of Mz.

bool active_button_Apply = false;   // check of active button.
bool active_button_Imitate = false; // check of active button Imitate.
// Dirs for loading.
const fs::path cur_path =
    fs::current_path().parent_path();         // get cur/parent path.
const fs::path src_path = cur_path / "fonts"; // images path.

// TTF loading.
vector<TTF_Font *> fonts;
vector<SDL_Texture *> texts_InputData;
vector<SDL_Texture *> texts_UserValues;
vector<SDL_Texture *> texts_Imitation;
vector<SDL_Texture *> texts_ImportExport;
vector<SDL_Texture *> texts_XY;
vector<SDL_Texture *> texts_values_X;
vector<SDL_Texture *> texts_values_TTH;

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

  // Collect all textures.
  gC_textrure.push_back(texture);

  return texture;
}

void changeUI() {
  // Change UI text in field.
  vector<SDL_Texture *>::iterator curInput;
  static int lastValue;

  // If UI run & not empty:
  if (UI_run || lastChanges) {

    // Fix values is limited.
    if (lastChanges)
      lastChanges = false;
    // ====================

    // Set up pointer in text field.
    if (UI == &UI_Mz)
      curInput = texts_UserValues.begin();
    else if (UI == &UI_Amplitude) {
      curInput = texts_UserValues.begin() + 1;
    }

    SDL_DestroyTexture(*curInput);
    if (!UI->empty()) {
      // Change this text.
      *curInput = createText(UI->c_str(), fonts[0], colors_texts[visionMode]);
    } else {
      *curInput = createText(" ", fonts[0], colors_texts[visionMode]);
    }
  }
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
    fonts.push_back(loadFont("KTFJermilov-Solid.ttf", 40)); // GUI.
    fonts.push_back(loadFont("KTFJermilov-Solid.ttf", 30)); // l. XY.
    fonts.push_back(loadFont("KTFJermilov-Solid.ttf", 14)); // numbers XY..

    // TEXT SAVER.
    //============================================================
    //============================================================

    // Create test text.
    texts_InputData.push_back(
        createText("Ввод данных", fonts[0], colors_texts_negative[visionMode]));
    texts_InputData.push_back(
        createText("Частота", fonts[0], colors_texts[visionMode]));
    texts_InputData.push_back(
        createText("Амплитуда", fonts[0], colors_texts[visionMode]));
    texts_InputData.push_back(
        createText("Применить", fonts[0], colors_texts[visionMode]));

    texts_UserValues.push_back(
        createText("10", fonts[0], colors_texts[visionMode]));
    texts_UserValues.push_back(
        createText("1000", fonts[0], colors_texts[visionMode]));

    texts_Imitation.push_back(
        createText("Имитация", fonts[0], colors_texts_negative[visionMode]));
    texts_Imitation.push_back(
        createText("Запустить", fonts[0], colors_texts[visionMode]));
    texts_Imitation.push_back(
        createText("Остановить", fonts[0], colors_texts[visionMode]));

    texts_ImportExport.push_back(
        createText("Импорт", fonts[0], colors_texts[visionMode]));
    texts_ImportExport.push_back(
        createText("Экспорт", fonts[0], colors_texts[visionMode]));

    texts_XY.push_back(
        createText("wt", fonts[1], colors_texts_negative[visionMode]));
    texts_XY.push_back(
        createText("A", fonts[1], colors_texts_negative[visionMode]));

    // Create labels for X.
    string value;
    for (int i = 1; i < 10; ++i) {
      value = "0." + to_string(i) + "T";
      texts_values_X.push_back(
          createText(value, fonts[2], colors_texts_negative[visionMode]));
      cout << "Value has been initialized: " << value << endl;
    }
    texts_values_X.push_back(
        createText("T", fonts[2], colors_texts_negative[visionMode]));
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

  R_block_UI.w = R_ControlPanel.w * 0.9;
  R_block_UI.h = R_ControlPanel.h / 4;
  R_block_UI.x = (R_ControlPanel.w - R_block_UI.w) / 2;
  R_block_UI.y = R_block_UI.h / 2;

  SDL_QueryTexture(texts_InputData[0], NULL, NULL, &R_label_UI.w,
                   &R_label_UI.h);

  R_label_UI.w *= 0.9;
  R_label_UI.h *= 0.9;
  R_label_UI.x = (R_ControlPanel.w - R_label_UI.w) / 2;
  R_label_UI.y = R_block_UI.y;

  R_field_Mz.w = R_label_UI.w;
  R_field_Mz.h = 30;
  R_field_Mz.x = R_label_UI.x;
  R_field_Mz.y = R_label_UI.y + R_label_UI.h + 10;

  R_field_Amplitude.w = R_field_Mz.w;
  R_field_Amplitude.h = R_field_Mz.h;
  R_field_Amplitude.x = R_field_Mz.x;
  R_field_Amplitude.y = R_field_Mz.y + R_field_Mz.h + 10;

  SDL_QueryTexture(texts_InputData[2], NULL, NULL, &R_label_A.w, &R_label_A.h);
  R_label_A.w /= 2;
  R_label_A.h = R_field_Amplitude.h;
  R_label_A.x = R_field_Amplitude.x + 5;
  R_label_A.y = R_field_Amplitude.y;

  //==============================================================================================================================================================================================================================================================================
  // Доделать отображение вводимых данных пользователя и приступить к созданию
  // мини окна с тек. данными точки графика.
  //==============================================================================================================================================================================================================================================================================

  // User data valbeues of Amplitude.
  SDL_QueryTexture(texts_UserValues[1], NULL, NULL, &R_data_A.w, &R_data_A.h);
  R_data_A.w *= 0.6;
  R_data_A.h = R_field_Amplitude.h;
  R_data_A.x = R_field_Amplitude.x + R_field_Amplitude.w - R_data_A.w - 5;
  R_data_A.y = R_field_Amplitude.y;

  SDL_QueryTexture(texts_InputData[1], NULL, NULL, &R_label_Mz.w,
                   &R_label_Mz.h);
  R_label_Mz.w /= 2;
  R_label_Mz.h = R_field_Mz.h;
  R_label_Mz.x = R_field_Mz.x + 5;
  R_label_Mz.y = R_field_Mz.y;

  // Button UI.
  SDL_QueryTexture(texts_InputData[3], NULL, NULL, &R_button_Apply.w,
                   &R_button_Apply.h);
  R_button_Apply.w = R_block_UI.w + 1;
  R_button_Apply.x = R_block_UI.x;
  R_button_Apply.y = R_field_Amplitude.y + R_field_Amplitude.h + 20;

  SDL_QueryTexture(texts_InputData[3], NULL, NULL, &R_label_bA.w,
                   &R_label_bA.h);
  R_label_bA.x = R_button_Apply.x + (R_button_Apply.w - R_label_bA.w) / 2;
  R_label_bA.y = R_button_Apply.y;

  // Block of drawing the decorate.
  decorate_UI[0].x = R_label_UI.x - (R_block_UI.w - R_label_UI.w) / 2;
  decorate_UI[0].y = R_label_UI.y + R_label_UI.h / 2;

  decorate_UI[1].x = decorate_UI[0].x + R_block_UI.w;
  decorate_UI[1].y = decorate_UI[0].y;

  decorate_UI[2].x = decorate_UI[1].x;
  decorate_UI[2].y = decorate_UI[1].y + R_block_UI.h;

  decorate_UI[3].x = decorate_UI[0].x;
  decorate_UI[3].y = decorate_UI[2].y;

  decorate_UI[4].x = decorate_UI[0].x;
  decorate_UI[4].y = decorate_UI[0].y;

  // Block of cur. values between of mouse point.
  R_block_curValues.w = 100;
  R_block_curValues.h = 40;
  R_block_curValues.x = 1000 / 2 - R_block_curValues.w / 2;
  R_block_curValues.y = 0;

  decorate_TTH.w = R_block_curValues.w + 4;
  decorate_TTH.h = R_block_curValues.h + 4;

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

    // Destroy all textures.
    int c_Texture = 0;

    vector<SDL_Texture *>::iterator i_Texture;
    for (i_Texture = gC_textrure.begin(); i_Texture != gC_textrure.end();
         ++i_Texture, ++c_Texture)
      SDL_DestroyTexture(*i_Texture);
    cout << c_Texture << " textures was destroyed." << endl;

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
  R_block.h = R_ControlPanel.h / 6;
  R_block.x = (R_ControlPanel.w - R_block.w) / 2;
  R_block.y = R_button_Apply.y + R_block.h / 2;

  // Draw Text.
  SDL_Rect R_label;

  // Draw Text.
  SDL_QueryTexture(texts_Imitation[0], NULL, NULL, &R_label.w, &R_label.h);

  // Block of drawing the decorate.
  int decorate_w = R_label.w;
  int decorate_h = R_block.h / 2;

  R_label.w *= 0.9;
  R_label.h *= 0.9;
  R_label.x = (R_ControlPanel.w - R_label.w) / 2;
  R_label.y = R_block.y;

  decorate[0].x = R_label.x - (decorate_w - R_label.w) / 2;
  decorate[0].y = R_label.y + R_label.h / 2;

  decorate[1].x = decorate[0].x + decorate_w;
  decorate[1].y = decorate[0].y;

  decorate[2].x = decorate[1].x;
  decorate[2].y = decorate[1].y + R_button_Apply.h + decorate_h / 2;

  decorate[3].x = decorate[0].x;
  decorate[3].y = decorate[2].y;

  decorate[4].x = decorate[0].x;
  decorate[4].y = decorate[0].y;

  SDL_RenderDrawLines(render, decorate, 5);
  //============================================================
  //============================================================
  //============================================================

  R_button_Imitate.w = decorate_w + 1;
  R_button_Imitate.h = R_label.h + 2;
  R_button_Imitate.x = decorate[4].x;
  R_button_Imitate.y = R_label.y + decorate_h;

  // Display the label.
  int *c;
  c = colors_background[visionMode];

  SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
  SDL_RenderFillRect(render, &R_label);
  SDL_RenderCopy(render, texts_Imitation[0], NULL, &R_label);

  // If imitated = stop, else -
  if (active_button_Imitate) {
    c = colors_buttons_with_active[visionMode];

    // Display the button.
    SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
    SDL_RenderFillRect(render, &R_button_Imitate);
    SDL_RenderCopy(render, texts_Imitation[2], NULL, &R_button_Imitate);

  } else {
    c = colors_buttons[visionMode];

    // Display the button.
    SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
    SDL_RenderFillRect(render, &R_button_Imitate);
    SDL_RenderCopy(render, texts_Imitation[1], NULL, &R_button_Imitate);
  }
}

bool drawBlock_input() {
  // Draw block od input values Amplitude & Mz.

  // Draw Text.
  SDL_RenderDrawLines(render, decorate_UI, 5);
  //============================================================
  //============================================================
  //============================================================

  // Set labels.
  SDL_RenderFillRect(render, &R_field_Mz);
  SDL_RenderCopy(render, texts_InputData[1], NULL, &R_label_Mz);

  SDL_RenderFillRect(render, &R_field_Amplitude);
  SDL_RenderCopy(render, texts_InputData[2], NULL, &R_label_A);

  int *c;
  c = colors_background[visionMode];

  SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
  SDL_RenderFillRect(render, &R_label_UI);
  SDL_RenderCopy(render, texts_InputData[0], NULL, &R_label_UI);

  if (active_button_Apply)
    c = colors_buttons_with_active[visionMode];
  else
    c = colors_buttons[visionMode];

  SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
  SDL_RenderFillRect(render, &R_button_Apply);

  SDL_RenderCopy(render, texts_InputData[3], NULL, &R_label_bA);

  // Display User values.

  // Change text while user is inputing data.
  changeUI();

  // UD values of Mz.
  SDL_QueryTexture(texts_UserValues[0], NULL, NULL, &R_data_Mz.w, &R_data_Mz.h);
  R_data_Mz.w *= 0.6;
  R_data_Mz.h = R_field_Mz.h;
  R_data_Mz.x = R_field_Mz.x + R_field_Mz.w - R_data_Mz.w - 5;
  R_data_Mz.y = R_field_Mz.y;
  SDL_RenderCopy(render, texts_UserValues[0], NULL, &R_data_Mz);

  // User data valbeues of Amplitude.
  SDL_QueryTexture(texts_UserValues[1], NULL, NULL, &R_data_A.w, &R_data_A.h);
  R_data_A.w *= 0.6;
  R_data_A.h = R_field_Amplitude.h;
  R_data_A.x = R_field_Amplitude.x + R_field_Amplitude.w - R_data_A.w - 5;
  R_data_A.y = R_field_Amplitude.y;
  SDL_RenderCopy(render, texts_UserValues[1], NULL, &R_data_A);
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

  // Display TTH when MP in field.
  if (displayTTH) {

    int *c;

    // Automate cleaning textures.
    vector<SDL_Texture *>::iterator oldValues;
    for (oldValues = texts_values_TTH.begin();
         oldValues != texts_values_TTH.end(); ++oldValues)
      SDL_DestroyTexture(*oldValues);

    texts_values_TTH.clear();
    // Create & show new values.
    string value;

    value = "U: " + to_string(U);
    texts_values_TTH.push_back(
        createText(value, fonts[2], colors_texts_negative[visionMode]));
    value.clear();

    value = to_string(T / 100) + " T";
    texts_values_TTH.push_back(
        createText(value, fonts[2], colors_texts_negative[visionMode]));
    value.clear();

    decorate_TTH.x = R_block_curValues.x + 4;
    decorate_TTH.y = R_block_curValues.y + 4;

    decorate_TTH.x = R_block_curValues.x - 2;
    decorate_TTH.y = R_block_curValues.y - 2;

    SDL_RenderFillRect(render, &decorate_TTH);

    // Display cur. TTH.
    c = colors_background[visionMode];
    SDL_SetRenderDrawColor(render, c[0], c[1], c[2], 255);
    SDL_RenderFillRect(render, &R_block_curValues);

    // Values of TTH.
    R_label_U.x = R_block_curValues.x + 5;
    R_label_U.y = R_block_curValues.y + 5;
    SDL_QueryTexture(texts_values_TTH[0], NULL, NULL, &R_label_U.w,
                     &R_label_U.h);
    SDL_RenderCopy(render, texts_values_TTH[0], NULL, &R_label_U);

    // TTH: T.
    R_label_T.x = R_block_curValues.x + 5;
    R_label_T.y = R_block_curValues.y + R_label_U.h + 5;
    SDL_QueryTexture(texts_values_TTH[1], NULL, NULL, &R_label_T.w,
                     &R_label_T.h);
    SDL_RenderCopy(render, texts_values_TTH[1], NULL, &R_label_T);
  }

  // Frame Rate controls.
  if (SDL_GetTicks() - _FPS_Timer < FPS) {
    SDL_Delay(FPS - SDL_GetTicks() + _FPS_Timer);
    _FPS_Timer = SDL_GetTicks();
  }
  SDL_RenderPresent(render);
}

void drawTTH(int x, int y) {

  int relX = x - 280 + 15;
  int relY = y + 15;

  R_block_curValues.x =
      (relX + R_block_curValues.w < 1000) ? relX : 1000 - R_block_curValues.w;
  R_block_curValues.y = relY;

  cursorX = x - 280;
}

void drawGraph() { drawAxis(); }

void setVisionMode(bool mode) {
  // Set up vision mode.
  if (mode)
    visionMode = 1;
  else
    visionMode = 0;
}
