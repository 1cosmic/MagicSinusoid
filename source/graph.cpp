#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <iostream>

using namespace std;

extern SDL_Renderer *render;
extern bool active_button_Imitate;

// Scale of XY.
float scaleX = 1;
float scaleY = 1;

SDL_Rect R_graph;
int lineX_length;
int lineY_length;

SDL_Point drawMP;
SDL_Point relativelyPoint;

// TTH of sinusoid.
int Mz = 100;
int Amplitude = 10000;
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
}

void drawGrid() {
  // Drawing of grid for analys.
  int length = 3000;

  while (length > 0) {

    length -= lineX_length / 10;
  }
}

void drawSinusoid(int lineW, bool run) {
  // Drawing of sinusoid.

  int detalization = lineX_length * 3;
  float relatively_x = (float(lineX_length) / detalization) * 0.97;
  float relatively_y = (lineY_length * 0.5 / float(Amplitude)) * 0.9;

  int i;
  float u;
  float omega_t;
  SDL_Point points[detalization];

  if (run)
    curPhase = (curPhase == 0) ? Phase : curPhase - Phase / speed_Phase;

  while (lineW > 0) {

    omega_t = Phase * Mz / detalization;

    for (i = 0; i < detalization; ++i) {

      u = Amplitude * sin(omega_t * i + curPhase);
      points[i].x = drawMP.x + i * relatively_x * scaleX + lineW;
      points[i].y = drawMP.y + u * relatively_y * scaleY;
    }

    SDL_RenderDrawLines(render, points, detalization);

    --lineW;
  }
}

void drawAxis(int visionMode) {

  // Check of night vision.
  int c1, c2, c3;

  // Set up colors. For Axis.
  c1 = colors_Axis[visionMode][0];
  c2 = colors_Axis[visionMode][1];
  c3 = colors_Axis[visionMode][2];
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);
  SDL_RenderSetViewport(render, &R_graph);

  drawPointerX(3);
  drawPointerY(3);

  // Set up colors. For sinusoid.
  c1 = colors_Sinusoid[visionMode][0];
  c2 = colors_Sinusoid[visionMode][1];
  c3 = colors_Sinusoid[visionMode][2];
  SDL_SetRenderDrawColor(render, c1, c2, c3, SDL_ALPHA_OPAQUE);

  drawSinusoid(3, active_button_Imitate);
}

void setDeltaRelatively(SDL_Point move) {
  SDL_Point delta;
  delta.x = relativelyPoint.x - move.x;
  delta.y = relativelyPoint.y - move.y;

  if (delta.x > lineX_length * (1 - scaleX) + 30)
    drawMP = delta;
}

void fixCurrentDrawing() {
  drawMP.x = 20;
  drawMP.y = R_graph.h / 2;

  relativelyPoint = drawMP;
}

void setCurrentPointAsMain() { relativelyPoint = drawMP; }
