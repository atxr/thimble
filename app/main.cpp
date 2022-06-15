#include <dirent.h>
#include <fjfx/fjfx.h>
#include <fjfx/frfxll.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <thimble/all.h>

using namespace std;
using namespace thimble;

int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "Need two arguments." << endl;
    cerr << "main <path/to/the/dataset/pgm/images> "
            "<path/output/txt/minutiae/points>"
         << endl;
    exit(EXIT_FAILURE);
  }

  string path = argv[1];
  string output = argv[2];

  DIR *dir;
  struct dirent *ent;
  int i=1;
  if ((dir = opendir(path.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      string image(ent->d_name);
      if (image.find(".pgm", 0) != string::npos) {
        // Read fingerprint image
        FJFXFingerprint fingerprint;
        bool success = fingerprint.fromImageFile(path + "/" + image);
        if (!success) {
          cerr << "Could not read " << image << endl;
          exit(EXIT_FAILURE);
        }

        // Access the non-empty minutiae template
        MinutiaeView view = fingerprint.getMinutiaeView();

        string outputPath(output + "/" +
                          image.replace(image.end() - 3, image.end(), "txt"));
        ofstream outputFile(outputPath);
        outputFile << view;
        outputFile.close();
        cout << i << ": Writing " << outputPath << endl;
        i++;
      }
    }
    closedir(dir);
  } else {
    /* could not open directory */
    perror("");
    return EXIT_FAILURE;
  }

  return 0;
}
