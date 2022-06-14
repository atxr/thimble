#include <iostream>
#include <thimble/all.h>

using namespace std;
using namespace thimble;

int main() {
  // Implementation
  SmallBinaryField gf(20);
  SmallBinaryFieldPolynomial V(gf);
  SmallBinaryFieldPolynomial f(gf);
  f = FuzzyVaultTools::createRandomInstance(V, 30, 20);
  cout << "Success!" << endl;
  return 0;
}
