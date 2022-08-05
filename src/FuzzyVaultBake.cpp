#include <thimble/finger/FuzzyVaultBake.h>

FuzzyVaultBake::FuzzyVaultBake(int width, int height, int dpi) : ProtectedMinutiaeTemplate(width, height, dpi) {}
FuzzyVaultBake::FuzzyVaultBake(BytesVault bv)
{
    fromBytes(bv.data, bv.size);
}

