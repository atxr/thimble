#include <iostream>
#include <thimble/finger/FuzzyVaultBake.h>

using namespace std;

FuzzyVaultBake::FuzzyVaultBake(int width, int height, int dpi) : ProtectedMinutiaeTemplate(width, height, dpi) {}
FuzzyVaultBake::FuzzyVaultBake(BytesVault bv)
{
    fromBytes(bv.data, bv.size);
}

BytesVault FuzzyVaultBake::toBytesVault()
{
    uint8_t *data;
    int size, wsize;

    // Initialize byte array ...
    size = getSizeInBytes();
    data = (uint8_t *)malloc(size * sizeof(uint8_t));
    if (data == NULL)
    {
        cerr << "byteFuzzyVault: "
             << "out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    // ... in which this protected minutiae template is packed.
    toBytes(data);
    return BytesVault(data, size);
}
