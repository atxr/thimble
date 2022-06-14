#include <fjfx/fjfx.h>
#include <fjfx/frfxll.h>
#include <iostream>
#include <thimble/all.h>

using namespace std;
using namespace thimble;

int main() {
  // Read fingerprint image
  FJFXFingerprint fingerprint;

  // Read the fingerprint image
  bool success = fingerprint.fromImageFile("fingerprint.pgm");
  if (!success) {
    cerr << "Could not read 'fingerprint.pgm'." << endl;
    exit(EXIT_FAILURE);
  }

  // Access the non-empty minutiae template
  MinutiaeView view = fingerprint.getMinutiaeView();

  // Print a text representation of the minutiae template to
  // 'cout'
  cout << view << endl;
  return 0;
}
