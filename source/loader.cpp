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
extern pair<int, float> pointsToCalc[DETALIZATION];
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

  int i;
  // For i generate value of U.
  for (i = 0; i < DETALIZATION; ++i) {
    uFile << Amplitude * sin(omega_t * i + curPhase) << endl;
  }
  cout << i << " points was uploaded." << endl;

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
  ifstream lFile(filePath.u8string(), ios_base::in);

  // Checker of success opened file.
  if (!lFile.is_open()) {
    cout << "I don`t open the file for save: " << filePath << endl;
    return false;
  }

  // Replace main values.
  cout << "Replace values." << endl;
  cout << "curPhase: " << curPhase << " Mz: " << Mz << " A: " << Amplitude
       << endl;
  //
  lFile >> curPhase;
  lFile >> Mz;
  lFile >> Amplitude;
  //
  cout << "curPhase: " << curPhase << " Mz: " << Mz << " A: " << Amplitude
       << endl
       << endl;
  // **********************

  // Load values.
  int i;

  // Escape Space.
  lFile >> valueBuf;

  // Load all mainpoints.
  for (i = 0; !lFile.eof(); ++i) {
    pointsToCalc[i].first = i;
    lFile >> pointsToCalc[i].second;
  }

  // Close & information.
  lFile.close();
  cout << "Counter of loaded values: " << i << endl;

  return true;
}
