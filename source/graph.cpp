#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

extern SDL_Renderer *render;
extern bool active_button_Imitate;
extern int visionMode;
bool displayCursor;
float U = 0; // display U ( Amp * Mz.).
float T = 0; // cur. value of T.
int c1, c2, c3;

// Labels for XY.
SDL_Rect R_label_X;                          // label of poiner X.
SDL_Rect R_label_Y;                          // label of poiner Y.
extern vector<SDL_Texture *> texts_XY;       // his texts.
extern vector<SDL_Texture *> texts_values_X; // labels for X.

// Scale of XY.
float scaleX = 1;
float scaleY = 1;
float relatively_y;

SDL_Rect R_graph;
int lineX_length;
int lineY_length;

SDL_Point drawMP;
SDL_Point relativelyPoint;

// TTH of sinusoid.
int cursorX;
int Mz = 10;
int Amplitude = 1000;
float omega_t;
float Phase = 2 * M_PI;
float curPhase = 0;
int speed_Phase = 90;

// First color - daily, second - nightly.
int colors_Axis[2][3] = {{0, 0, 0}, {255, 255, 255}};
int colors_Sinusoid[2][3] = {{0, 200, 255}, {255, 204, 0}};

void initGraph(int W, int H) {
  R_graph.w = 1000;
  R_graph.h = H;
  R_graph.x = 280;
  R_graph.y = 0;

  lineX_length = R_graph.w * 0.96;
  lineY_length = R_graph.h * 0.95;

  drawMP.x = 20;
  drawMP.y = R_graph.h / 2;

  relativelyPoint = drawMP;

  // Set label for Axis.
  SDL_QueryTexture(texts_XY[0], NULL, NULL, &R_label_X.w, &R_label_X.h);
  SDL_QueryTexture(texts_XY[1], NULL, NULL, &R_label_Y.w, &R_label_Y.h);
}

void drawPointerX(int lineW) {

  SDL_Rect line;
  SDL_Point arrow[3];
  int arrow_L = 20;
  int arrow_W = 10;

  line.h = lineW;
  line.w = lineX_length * scaleX;
  line.x = drawMP.x;
  line.y = drawMP.y - lineW / 2;

  // Set up colors. For Axis.
  c1 = colors_Axis[visionMode][0];
  c2 = colors_Axis[visionMode][1];
  c3 = colors_Axis[visionMode][2];
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);

  // Draw line.
  SDL_RenderFillRect(render, &line);

  while (lineW > 0) {

    // Center point of arrow.
    arrow[1].x = drawMP.x + line.w - lineW + 4; // why +4? check.
    arrow[1].y = drawMP.y;

    // Here relatively point of upper line.
    arrow[0].x = arrow[1].x - arrow_L - lineW;
    arrow[0].y = drawMP.y - arrow_W;

    // Here relatively point of downer line.
    arrow[2].x = arrow[1].x - arrow_L - lineW;
    arrow[2].y = drawMP.y + arrow_W;

    // THE MOST BOOOLD!
    lineW--;

    SDL_RenderDrawLines(render, arrow, 3);
  }

  // Set label.
  R_label_X.x = arrow[1].x - R_label_X.w * 0.8;
  R_label_X.y = arrow[1].y + 10;
  SDL_RenderCopy(render, texts_XY[0], NULL, &R_label_X);
}

void drawPointerY(int lineW) {

  SDL_Rect line;
  SDL_Point arrow[3];
  int arrow_L = 20;
  int arrow_W = 10;

  line.h = (scaleY >= 1) ? lineY_length * scaleY : lineY_length;
  line.w = lineW;
  line.x = drawMP.x - lineW / 2;
  line.y = drawMP.y - line.h / 2;

  // Set up colors. For Axis.
  c1 = colors_Axis[visionMode][0];
  c2 = colors_Axis[visionMode][1];
  c3 = colors_Axis[visionMode][2];
  SDL_SetRenderDrawColor(render, c2, c2, c3, SDL_ALPHA_OPAQUE);

  // Draw line.
  SDL_RenderFillRect(render, &line);

  while (lineW > 0) {

    // Center point of arrow.
    arrow[1].x = drawMP.x; // why +4? check.
    arrow[1].y = line.y + lineW - 4;

    // Here relatively point of left line.
    arrow[0].x = arrow[1].x - arrow_W;
    arrow[0].y = arrow[1].y + arrow_L + lineW;

    // Here relatively point of right line.
    arrow[2].x = arrow[1].x + arrow_W;
    arrow[2].y = arrow[1].y + arrow_L + lineW;

    // THE MOST BOOOLD!
    lineW--;

    SDL_RenderDrawLines(render, arrow, 3);
  }

  // Set label.
  R_label_Y.x = arrow[1].x + R_label_Y.w;
  R_label_Y.y = arrow[1].y - 5;
  SDL_RenderCopy(render, texts_XY[1], NULL, &R_label_Y);
}

void drawGrid() {
  // Drawing of grid for analys.

  extern float relatively_y;

  SDL_Point line[2];

  int count_V = 10 * scaleX;
  float width_V = 0.97 * (float(lineX_length) * scaleX / count_V);
  float main_width_V = 0.97 * (float(lineX_length) * scaleX / 10);

  //*********************************************
  // Draw Vertical lines.
  // Fixed upper & lower points parallel line.
  line[0].y = drawMP.y - lineY_length / 2;
  line[1].y = drawMP.y + lineY_length / 2;

  SDL_SetRenderDrawColor(render, 150, 150, 150, 100);
  for (int i = 1; i <= count_V; ++i) {
    line[1].x = line[0].x = drawMP.x + width_V * i;

    SDL_RenderDrawLines(render, line, 2);
  }

  //*********************************************
  // Draw main lines.

  SDL_Rect R_values_X[10];
  SDL_Rect *p_v_X;

  SDL_SetRenderDrawColor(render, 255, 255, 255, 100);
  for (int i = 0; i < 10; i++) {
    for (int w = 0; w < 2; ++w) {

      line[0].x = drawMP.x + main_width_V * (1 + i) + w;
      line[1].x = drawMP.x + main_width_V * (1 + i) + w;

      SDL_RenderDrawLines(render, line, 2);

      // Display values.
      p_v_X = &R_values_X[i];
      SDL_QueryTexture(texts_values_X[i], NULL, NULL, &p_v_X->w, &p_v_X->h);
      p_v_X->x = line[1].x - p_v_X->w / 2;
      p_v_X->y = line[1].y;
      SDL_RenderCopy(render, texts_values_X[i], NULL, p_v_X);
    }
  }

  //*********************************************
  // Draw Horisontal lines.
  // Fixed upper & lower points parallel line.
  line[0].x = drawMP.x - 5;
  line[1].x = drawMP.x + 0.97 * lineX_length * scaleX;

  int count_H = 10;
  int width_H = 0.9 * ((relatively_y * Amplitude) / (count_H));
  // int main_width_H = 0.96 * (float(lineY_length) / 10);

  int direction = -1;
  SDL_SetRenderDrawColor(render, 150, 150, 150, 100);
  for (int i = 1; i <= count_H; ++i) {
    for (int i2 = 0; i2 < 2; ++i2) {

      // Draw line first to the down, next - to the up.
      direction = (direction > 0) ? -1 : 1;
      line[1].y = line[0].y = drawMP.y + direction * width_H * i;

      SDL_RenderDrawLines(render, line, 2);
    }
  }
}

void drawSinusoid(int lineW, bool run) {
  // Drawing of sinusoid.

  int w = 4;

  relatively_y = ((lineY_length * 0.5 + lineW) / float(Amplitude));
  int detalization = lineX_length * 3;
  float relatively_x = (float(lineX_length) / detalization);

  int i;
  float u;
  SDL_Point points[detalization];

  if (run)
    curPhase = (curPhase == 0) ? Phase : curPhase - Phase / speed_Phase;

  // Set up colors. For sinusoid.
  c1 = colors_Sinusoid[visionMode][0];
  c2 = colors_Sinusoid[visionMode][1];
  c3 = colors_Sinusoid[visionMode][2];

  int lineW_start = lineW / 2; // compensation offset && centering.
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);
  while (lineW > 0) {

    omega_t = Phase * Mz / detalization;

    for (i = 0; i < detalization; ++i) {

      u = Amplitude * sin(omega_t * i + curPhase);
      points[i].x =
          drawMP.x + i * relatively_x * scaleX * 0.97 - lineW + lineW_start;
      points[i].y =
          drawMP.y + u * relatively_y * 0.9 * scaleY + lineW - lineW_start;
    }

    SDL_RenderDrawLines(render, points, detalization);

    --lineW;
  }

  // Display cursor point to the graphic.
  // If cursor display, draw cross.
  if (displayCursor) {

    SDL_Point cross[2];

    int relativeX = cursorX - drawMP.x;

    relatively_x = lineX_length * scaleX * 0.97;
    omega_t = Phase * Mz / relatively_x;
    relatively_x /= 100;

    // Cout to TTH block.
    T = float(relativeX) / relatively_x;

    if (relativeX > 0 && (T <= 100)) {

      u = Amplitude * sin(omega_t * float(relativeX) + curPhase);

      // Cout to TTH block.
      U = u * (-1);

      // MORE BOOOLD!
      int direction = -1;
      lineW_start = w;

      SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
      for (lineW = 0; lineW < w; ++lineW) {
        for (int i2 = 0; i2 < 2; ++i2) {

          direction = (direction > 0) ? -1 : 1;

          cross[0].x =
              drawMP.x + relativeX + lineW - lineW_start - 8 * direction;
          cross[0].y = drawMP.y + u * relatively_y * 0.9 * scaleY - 8;

          cross[1].x = cross[0].x + 16 * direction;
          cross[1].y = cross[0].y + 16;

          SDL_RenderDrawLines(render, cross, 2);
        }
      }
    }
  }
}

void drawCursor() {}

void drawAxis() {
  // Draw all graph.

  SDL_RenderSetViewport(render, &R_graph);

  // Draw main graph.
  drawPointerX(3);
  drawGrid();
  drawSinusoid(3, active_button_Imitate);
  drawPointerY(3);

  drawCursor();
}

void setDeltaRelatively(SDL_Point move) {
  SDL_Point delta;
  delta.x = relativelyPoint.x - move.x;
  // delta.y = relativelyPoint.y - move.y;

  if (delta.x > lineX_length * (1 - scaleX) + 30)
    drawMP.x = delta.x;
}

void fixCurrentDrawing() {
  drawMP.x = 20;
  drawMP.y = R_graph.h / 2;

  relativelyPoint = drawMP;
}

void setCurrentPointAsMain() { relativelyPoint = drawMP; }
