#include <SDL2/SDL.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#define DETALIZATION 3000

using namespace std;

namespace fs = std::filesystem;

// Connect points & other TTH of Sinusoid.
extern SDL_Point points[DETALIZATION];
extern int Mz;
extern int Amplitude;
extern float omega_t;
extern float curPhase;
extern bool active_button_Imitate;
extern int scaleX;

fs::path testDir = fs::current_path() / "uploads";

bool uploadData(string fileName) {
  // Upload data in file.

  active_button_Imitate = false;

  fs::path filePath = testDir / fileName;

  // Create of linker.
  // If file exists, clear here.
  ofstream uFile(filePath.u8string(), ios_base::trunc);

  // Checker of success opened file.
  if (!uFile.is_open()) {
    cout << "I don`t open the file for write: " << filePath << endl;
    return false;
  }

  // Write start value.
  uFile << curPhase << endl << Mz << endl << Amplitude << endl;

  // Indent.
  uFile << endl;

  // For i generate value of U.
  for (int i = 0; i < DETALIZATION; ++i) {
    uFile << Amplitude * sin(omega_t * i + curPhase) << endl;
  }
  cout << "All points was uploaded." << endl;

  // Close file.
  uFile.close();
  cout << "File was closed." << endl;

  return true;
}

bool loadData(string fileName) {
  // Load data in file.

  // Turn off imitated.
  active_button_Imitate = false;

  float valueBuf;
  fs::path filePath = testDir / fileName;

  // Create of linker.
  // If file exists, clear here.
  ifstream uFile(filePath.u8string(), ios_base::in);

  // Checker of success opened file.
  if (!uFile.is_open()) {
    cout << "I don`t open the file for save: " << filePath << endl;
    return false;
  }

  // Load values.
  // for (uFile >> valueBuf; !uFile.eof(); uFile >> valueBuf) {
  //   cout << "I wish read value: " << valueBuf << endl;
  // }

  uFile >> valueBuf;
  cout << "I wish read value: " << valueBuf << endl;
  uFile >> valueBuf;
  cout << "I wish read value: " << valueBuf << endl;
  uFile >> valueBuf;
  cout << "I wish read value: " << valueBuf << endl;
  uFile >> valueBuf;
  cout << "I wish read value: " << valueBuf << endl;

  uFile.close();

  return true;
}
