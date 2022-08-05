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


/**
 * @brief
 *             Use a quantized minutiae template as a query to open the
 *             vault.
 *
 * @details
 *             This is a variant of the \link open()\endlink
 *             function in which a set of quantized minutiae is
 *             used to open the vault. Therein, a quantized minutia is
 *             encoded as an element of the finite field that can be
 *             accessed via \link getField()\endlink.
 *
 *             To obtain the feature set of an absolutely pre-aligned
 *             minutiae template, the \link quantize()\endlink method
 *             can be used.
 *
 * @param f
 *             Will be set to the secret polynomial on successful
 *             verification; otherwise the content will be left
 *             unchanged
 *
 * @param B
 *             The feature set.
 *
 * @param t
 *             Number of elements in the feature set.
 *
 * @return
 *             <code>true</code> if the verification procedure was
 *             successful; otherwise, if the verification was not
 *             successful, the function returns <code>false</code>.
 *
 * @warning
 *             If this \link ProtectedMinutiaeTemplate\endlink
 *             does not represent a successfully enrolled and
 *             decrypted protected minutiae template, i.e.,
 *             if \link isDecrypted()\endlink
 *             returns <code>false</code>, the function
 *             prints an error message to <code>stderr</code> and
 *             exits with status 'EXIT_FAILURE'.
 *
 * @warning
 *             If <code>B</code> does not contain at least
 *             <code>t</code> well-defined distinct feature elements,
 *             the function runs into undocumented behavior.
 *
 * @warning
 *             If not enough memory could be provided, an error
 *             message is printed to <code>stderr</code> and the
 *             program exits with status 'EXIT_FAILURE'.
 */
bool FuzzyVaultBake::open(SmallBinaryFieldPolynomial &f, const uint32_t *B, int t) const
{

    // Ensure that this instance does protect a feature set and ...
    if (!isEnrolled())
    {
        cerr << "FuzzyVaultBake::open: "
             << "no minutiae template protected by this view." << endl;
        exit(EXIT_FAILURE);
    }

    // ... contains a decrypted polynomial.
    if (isEncrypted())
    {
        cerr << "FuzzyVaultBake::open: "
             << "vault is encrypted; decrypt first." << endl;
        exit(EXIT_FAILURE);
    }

    // Allocate memory to hold the set of unlocking pairs '{(x[j],y[j])}'
    uint32_t *x, *y;
    x = (uint32_t *)malloc(t * sizeof(uint32_t));
    y = (uint32_t *)malloc(t * sizeof(uint32_t));
    if (x == NULL || y == NULL)
    {
        cerr << "FuzzyVaultBake::open: "
             << "Out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    bool success = false;

    // Iterate of candidates of slow-down values until
    // decoding is successful or the whole slow-down range
    // has been tested
    for (BigInteger slowDownVal = 0;
         BigInteger::compare(slowDownVal, this->slowDownFactor) < 0;
         add(slowDownVal, slowDownVal, 1))
    {

        SmallBinaryFieldPolynomial V = unpackVaultPolynomial(slowDownVal);

        // Build unlocking set and ...
        for (int j = 0; j < t; j++)
        {

            // ... don't forget to apply the permutation process
            x[j] = _reorder(B[j]);

            y[j] = V.eval(x[j]);
        }

        // Attempt to decode the unlocking set
        success = decode(f, x, y, t, this->k, this->hash, this->D);

        if (success)
        {
            break;
        }
    }

    free(x);
    free(y);

    return success;
}

bool FuzzyVaultBake::open(SmallBinaryFieldPolynomial &f, const MinutiaeView &view) const
{

    // Allocate memory to temporarily hold the feature set.
    uint32_t *x = (uint32_t *)malloc(this->tmax * sizeof(uint32_t));
    if (x == NULL)
    {
        cerr << "FuzzyVaultBake::open: out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    // Extract the feature set and ...
    int t = quantize(x, view);

    // ... attempt to open with the feature set.
    bool success = open(f, x, t);

    // Free temporarily allocated memory.
    free(x);

    return success;
}
