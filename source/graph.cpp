#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cmath>

extern SDL_Renderer *render;

SDL_Rect R_graph;
SDL_Point drawMP;
SDL_Point relativelyPoint;

// TTH of sinusoid.
int Mz = 100;
int Amplitude = 100;
float Phase = 2 * M_PI;
float curPhase = 0;
int speed_Phase = 90;

// First color - daily, second - nightly.
int colors_Axis[2][3] = {{0, 0, 0}, {255, 255, 255}};

// Scale of XY.
float scaleX = 1;
float scaleY = 1;

void initGraph(int W, int H) {
  R_graph.w = 1000;
  R_graph.h = H;
  R_graph.x = 280;
  R_graph.y = 0;

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
  line.w = R_graph.w * scaleX;
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

  line.h = R_graph.h * scaleX;
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
void drawSinusoid(int lineW) {
  // Drawing of sinusoid.

  int detalization = 1000;
  float k_of_scale = (R_graph.w / detalization);

  SDL_Point points[detalization];

  int i;
  float u;
  float omega_t;

  curPhase = (curPhase == 0) ? Phase : curPhase - Phase / speed_Phase;

  while (lineW > 0) {

    omega_t = Phase * Mz / detalization;

    for (i = 0; i < detalization; ++i) {

      u = Amplitude * sin(omega_t * i + curPhase);
      points[i].x = drawMP.x + i * k_of_scale * scaleX + lineW;
      points[i].y = drawMP.y + u;
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
  drawSinusoid(3);
}

void setDeltaRelatively(SDL_Point deltaMove) {

  drawMP.x = relativelyPoint.x - deltaMove.x;
  drawMP.y = relativelyPoint.y - deltaMove.y;
}

void setCurrentPointAsMain() { relativelyPoint = drawMP; }
