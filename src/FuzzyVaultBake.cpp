#include <iostream>
#include <algorithm>
#include <utility>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <thimble/security/SHA.h>
#include <thimble/security/FuzzyVaultTools.h>
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

uint32_t FuzzyVaultBake::getf0(MinutiaeView view)
{
    SmallBinaryFieldPolynomial f(getField());
    if (!open(f, view))
    {
        return -1;
    }

    return f.eval(0);
}

/**
 * @brief Decode the secret polynomial from a query
 * Override the decode function of ProtectedMinutiaeTemplate to avoid using the stored hash of the secret polynomial
 * Hence, in FuzzyVaultBake, no need to store this hash anymore, to avoid offline attacks on it.
 *
 * @param f return the secret polynomial if the decoding is successful
 * @param x the query set
 * @param y the images of the locked vault
 * @param n  number of vault points
 * @param k size of the secret polynomial
 * @param hash useless, not used here
 * @param maxIts number of tests to do in the main loop
 * @return true if the decode is successful, but it doesn't assure that f is the right secret polynomial
 */
bool FuzzyVaultBake::decode(SmallBinaryFieldPolynomial &f, const uint32_t *x, const uint32_t *y,
                            int n, int k, const uint8_t hash[20], int maxIts) const
{

    SHA sha;

    // Check whether we can choose random points from 'n'
    // points using 'rand()'
    if (n > RAND_MAX)
    {
        cerr << "FuzzyVault::bfattack: The number of vault points must be"
             << " smaller than or equal RAND_MAX which is"
             << RAND_MAX << "." << endl;
        exit(EXIT_FAILURE);
    }

    // Check whether the vault is of reasonable parameters
    if (n <= 0 || k <= 0 || k > n)
    {
        cerr << "FuzzyVault::bfattack: The number of vault points must be"
             << " greater than zero. Furthermore, the size of the secret "
             << "polynomial must be greater than zero and smaller than "
             << "(or equal) to the vault's size" << endl;
        exit(EXIT_FAILURE);
    }

    // Keeps track whether a polynomial was yet found or not
    bool state = false;

    // Initialize space for the candidate polynomial
    SmallBinaryFieldPolynomial candidatePolynomial(f.getField());
    candidatePolynomial.ensureCapacity(k);

    // Initalize space for the hash of the candidate polynomial
    uint8_t candidateHash[20];

    uint32_t *a, *b;
    int *indices;

    // Allocate memory to select 'k' random vault
    // points
    a = (uint32_t *)malloc(k * sizeof(uint32_t));
    b = (uint32_t *)malloc(k * sizeof(uint32_t));
    indices = (int *)malloc(k * sizeof(uint32_t));
    if (a == NULL || b == NULL || indices == NULL)
    {
        cerr << "FuzzyVault::bfattack: Out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    unordered_map<uint32_t, int> result = {};
    pair<uint32_t, int> max = make_pair(0, -1);

    // Iterate at most 'maxIts' times
    for (uint64_t it = 0; it < maxIts; it++)
    {

        // Select pairwise different indices in the range
        // '0,...,n-1' and ...
        FuzzyVaultTools::fastChooseIndicesAtRandom(indices, n, k);

        // ... set the selected vault points, correspondingly.
        for (int i = 0; i < k; i++)
        {
            int j = indices[i];
            a[i] = x[j];
            b[i] = y[j];
        }

        // Determine the interpolation polynomial of the selected
        // vault points and ...
        candidatePolynomial.interpolate(a, b, k);
        uint32_t f0 = candidatePolynomial.eval(0);

        if (result.count(f0) == 0)
        {
            result[f0] = 1;
        }
        else
        {
            result[f0]++;
        }

        if (max.second == -1 || (f0 != max.first && result[f0] > max.second))
        {
            max = make_pair(f0, result[f0]);
            f.assign(candidatePolynomial);
        }
    }

    // Free memory
    free(a);
    free(b);
    free(indices);

    // vector<pair<uint32_t, int>> top3(3);
    // partial_sort_copy(
    //     begin(result), end(result),
    //     top3.begin(), top3.end(),
    //     [](auto &p1, auto &p2)
    //     {
    //         return p1.second > p2.second;
    //     });

    // cout << "Top 3 occurences: " << endl
    //      << "   1. " << top3[0].first << " with " << top3[0].second << " occurences" << endl
    //      << "   2. " << top3[1].first << " with " << top3[1].second << " occurences" << endl
    //      << "   3. " << top3[2].first << " with " << top3[2].second << " occurences" << endl
    //      << "   with a total of " << maxIts << " tests." << endl;

    return true;
}

bool FuzzyVaultBake::open(SmallBinaryFieldPolynomial &f, const MinutiaeView &view) const
{
    // Allocate memory to temporarily hold the feature set.
    uint32_t *B = (uint32_t *)malloc(this->tmax * sizeof(uint32_t));
    if (B == NULL)
    {
        cerr << "ProtectedMinutiaeTemplate::open: out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    // Extract the feature set and ...
    int t = quantize(B, view);

    // Ensure that this instance does protect a feature set and ...
    if (!isEnrolled())
    {
        cerr << "ProtectedMinutiaeTemplate::open: "
             << "no minutiae template protected by this view." << endl;
        exit(EXIT_FAILURE);
    }

    // ... contains a decrypted polynomial.
    if (isEncrypted())
    {
        cerr << "ProtectedMinutiaeTemplate::open: "
             << "vault is encrypted; decrypt first." << endl;
        exit(EXIT_FAILURE);
    }

    // Allocate memory to hold the set of unlocking pairs '{(x[j],y[j])}'
    uint32_t *x, *y;
    x = (uint32_t *)malloc(t * sizeof(uint32_t));
    y = (uint32_t *)malloc(t * sizeof(uint32_t));
    if (x == NULL || y == NULL)
    {
        cerr << "ProtectedMinutiaeTemplate::open: "
             << "Out of memory." << endl;
        exit(EXIT_FAILURE);
    }

    bool success = false;

    // TODO talk about slowDown
    // The slowDown utility is not used with the FuzzyVaultBake
    // If the slowDownFactor is higher than 1, the program decoding shouldn't work
    if (BigInteger::compare(slowDownFactor, BigInteger(1)) != 0)
    {
        cerr << "Error: You cannot use the slowDown utility with FuzzyVaultBake"
             << "You must set slowDownFactor to 1" << endl;
        exit(EXIT_FAILURE);
    }
    SmallBinaryFieldPolynomial V = unpackVaultPolynomial();

    // Build unlocking set and ...
    for (int j = 0; j < t; j++)
    {
        // ... don't forget to apply the permutation process
        x[j] = _reorder(B[j]);
        y[j] = V.eval(x[j]);
    }

    // Attempt to decode the unlocking set
    success = decode(f, x, y, t, this->k, this->hash, this->D);

    free(x);
    free(y);
    free(B);

    return success;
}
