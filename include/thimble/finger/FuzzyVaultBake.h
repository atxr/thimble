#ifndef THIMBLE_FUZZYVAULTBAKE
#define THIMBLE_FUZZYVAULTBAKE

#include "ProtectedMinutiaeTemplate.h"

using namespace thimble;

struct BytesVault
{
    uint8_t *data;
    int size;

    BytesVault(uint8_t *x, int y)
    {
        data = x;
        size = y;
    }
};

/**
 * @brief
 *  Extends the ProtectedMinutiaeTemplate class to provide a vault for bake
 *
 * @details
 *  Add useful function for the bake protocol to the protected vault of THIMBLE
 *  Modify the decode function to avoid storing the polynomial hash
 *
 * @author Alexandre TULLOT
 */
class FuzzyVaultBake : public ProtectedMinutiaeTemplate
{
public:
    /**
     * @brief Construct a new Fuzzy Vault Bake object
     * Call the TemplateProtectedMinutiae constructor
     *
     * @param width
     * @param height
     * @param dpi
     */
    FuzzyVaultBake(int width, int height, int dpi);

    /**
     * @brief Construct a new Fuzzy Vault Bake object with a byte representation
     *
     * @param bv: a byte representation of a fuzzy vault
     */
    FuzzyVaultBake(BytesVault bv);

    /**
     * @brief Return a compact byte representation of a fuzzy vault
     *
     * @return BytesVault
     */
    BytesVault toBytes();

    /**
     * @brief Open the vault, and if it is a success, compute f(0) where f is the secret polynomial
     *
     * @param view: the query to try to open the vault
     * @return uint32_t: value of f(0)
     */
    uint32_t getf0(MinutiaeView view);

    /**
     * @brief Overide the decode function of ProtectedMinutiaeTemplate.
     * This decode function use redondency of hash within candidates for the secret polynomial
     * instead of comparing the candidates with a stored hash of the secret polynomial.
     * Hence, no information about the secret polynomial is stored, so we eliminate
     * offline attacks possibilities during the bake protocol.
     *
     * @param f will store the most redundant polynomial within the candidates
     * @param x
     * @param y
     * @param t
     * @param k
     * @param hash: useless parameter, will be null
     * @param D
     * @return false if some errors occured 
     */
    bool decode(SmallBinaryFieldPolynomial &f, const uint32_t *x, const uint32_t *y,
                int t, int k, const uint8_t hash[20], int D) const;
};

#endif