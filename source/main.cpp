#define _USE_MATH_DEFINES

#include "headers/graph.hpp"
#include "headers/gui.hpp"
#include "headers/processor.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>

#define SCR_W 1280
#define SCR_H 720

using namespace std;

int main() {

  // extern SDL_Renderer *render; // Renderer of the drawing.

  bool run = true; // for main circle.
  SDL_Event event; // user event.

  // Main func. block.
  initGUI(SCR_W, SCR_H);

  if (showMain())
    cout << "Show window..." << endl;
  //==============================

  // Set up standard settings for Graphic.
  initGraph(SCR_W, SCR_H);

  // Set up debug value for Sinusoida.
  // set_Mz_Amp(1, 100);

  // Main circle.
  while (run) {
    // Processor user input // event.
    while (SDL_PollEvent(&event) != 0) {

      // Processor key.
      run = processor(event);

      // If Sinusoida was close:
      if (!run)
        break;
    }

    // Show background.
    showMain();

    // Display animated Axis.
    drawGraph();

    // display all changes.
    displayChanges();
  }

  // Grubber MDFker.
  destroyGUI(); // destroy SDL object.

  return 0;
}
