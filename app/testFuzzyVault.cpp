#include "thimble/finger/MinutiaeFuzzyVault.h"
#include <cstdlib>
#include <fjfx/fjfx.h>
#include <fjfx/frfxll.h>
#include <iostream>
#include <thimble/all.h>

using namespace std;
using namespace thimble;

int main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Need 4 arguments." << endl;
    cerr << "main <path/to/the/pgm/template> "
         << "<path/to/the/pgm/query> "
         << "<image_size_x> <imgae_size_y> " << endl;
    exit(EXIT_FAILURE);
  }

  string image = argv[1];
  string query = argv[2];
  int size_x = stoi(argv[3]);
  int size_y = stoi(argv[4]);

  FJFXFingerprint fingerprint;
  if (!fingerprint.fromImageFile(image)) {
    cerr << "Could not read " << image << endl;
    exit(EXIT_FAILURE);
  }

  // Access the non-empty minutiae template
  MinutiaeView minutiaeView = fingerprint.getMinutiaeView();

  // Create the vault from this minutiae view
  MinutiaeFuzzyVault vault(size_x, size_y);
  if(!vault.enroll(minutiaeView)) {
    cerr << "Fail to enroll " << image << endl;
    exit(EXIT_FAILURE);
  }
  SmallBinaryFieldPolynomial f(vault.getField());
  cout << "Vault locked with " << image << endl;

  // Get the query image and extract minutiae points
  FJFXFingerprint fingerprintQuery;
  if (!fingerprintQuery.fromImageFile(query)) {
    cerr << "Could not read " << query << endl;
    exit(EXIT_FAILURE);
  }
  MinutiaeView minutiaeViewQuery = fingerprintQuery.getMinutiaeView();

  // Try to open the vault
  // NOTE: the query is supposed aligned
  if (!vault.open(f, minutiaeViewQuery)) {
    cerr << "Could not open the vault with " << query << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Success: vault opened with " << query << endl;
  return 0;
}
