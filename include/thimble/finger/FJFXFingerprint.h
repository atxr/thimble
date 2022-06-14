/*
 *  THIMBLE --- Research Library for Development and Analysis of
 *  Fingerprint-Based Biometric Cryptosystems.
 *
 *  Copyright 2014 Benjamin Tams
 *
 *  THIMBLE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  THIMBLE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with THIMBLE. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file FJFXFingerprint.h
 *
 * @brief
 *            Provides a class that represents a fingerprint and
 *            mechanisms for estimating certain fingerprint features.
 *
 * @details
 *            Extends the Fingerprint class with the FJFX minutiae extractor.
 *            Required libfjfx.a available at
 *            https://github.com/FingerJetFXOSE/FingerJetFXOSE
 *
 * @author Alexandre Tullot
 **/

#include "thimble/finger/Fingerprint.h"
#include <fjfx/fjfx.h>

using namespace std;
using namespace thimble;

/**
 * @brief
 *            An instance of this class represents a fingerprint
 **/
class FJFXFingerprint : public Fingerprint {
public:
  // Overrides the empty minutiae estimation method from the 'Fingerprint' class
  /**
   * @brief
   *          Overrides the empty minutiae estimation method from the
   *          'Fingerprint' class Provides a minutiae points extraction
   *          thanks to the FJFX library.
   *
   * @author Alexandre Tullot
   **/
  virtual MinutiaeView estimateMinutiae();
};
