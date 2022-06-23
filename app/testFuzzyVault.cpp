#include <fjfx/fjfx.h>
#include <fjfx/frfxll.h>
#include <thimble/all.h>

#include <iostream>

using namespace std;
using namespace thimble;

void lockFuzzyVault(MinutiaeFuzzyVault &vault, string image) {
  FJFXFingerprint fingerprint;
  if (!fingerprint.fromImageFile(image)) {
    cerr << "Could not read " << image << endl;
    exit(EXIT_FAILURE);
  }
  DirectedPoint refPoint = fingerprint.getDirectedReferencePoint();

  // Access the non-empty minutiae template
  MinutiaeView minutiaeView = fingerprint.getMinutiaeView();
  minutiaeView = FingerTools::prealign(minutiaeView, refPoint);

  // Lock the vault from this minutiae view
  if(!vault.enroll(minutiaeView)) {
    cerr << "Fail to enroll " << image << endl;
    exit(EXIT_FAILURE);
  }
  cout << "Vault locked with " << image << endl;
}

void unlockFuzzyVault(MinutiaeFuzzyVault &vault, string query) {
  // Get the query image and extract minutiae points
  FJFXFingerprint fingerprintQuery;
  if (!fingerprintQuery.fromImageFile(query)) {
    cerr << "Could not read " << query << endl;
    exit(EXIT_FAILURE);
  }
  MinutiaeView minutiaeViewQuery = fingerprintQuery.getMinutiaeView();
  MinutiaeView alignedQuery = FingerTools::prealign(minutiaeViewQuery, fingerprintQuery.getDirectedReferencePoint());

  // Try to open the vault
  // NOTE: the query is supposed aligned
  SmallBinaryFieldPolynomial f(vault.getField());
  if (!vault.open(f, alignedQuery)) {
    cerr << "Could not open the vault with " << query << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Success: vault opened with " << query << endl;
}


void testDataset(string arg1, int size_x, int size_y) {
  
}


int main(int argc, char **argv) {
  if (argc != 5) {
    cerr << "Need 4 arguments." << endl;
    cerr << "main <path/to/the/pgm/template> "
         << "<path/to/the/pgm/query> "
         << "<image_size_x> <imgae_size_y> " << endl;

    cerr << "Use flag -d to test the fuzzy vault on a dataset" << endl;
    cerr << "main -d <path/to/the/pgm/dataset> "
         << "<image_size_x> <imgae_size_y> " << endl;
    exit(EXIT_FAILURE);
  }

  string arg1 = argv[1];
  string arg2 = argv[2];
  int size_x = stoi(argv[3]);
  int size_y = stoi(argv[4]);

  if (arg1 == "-d") {
    testDataset(arg1, size_x, size_y);
  }

  else {
    MinutiaeFuzzyVault vault(size_x, size_y);
    lockFuzzyVault(vault, arg1);
    unlockFuzzyVault(vault, arg2);
  }
}
