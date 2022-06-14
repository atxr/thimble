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
 * @file FJFXFingerprint.cpp
 *
 * @brief
 *            Implements mechanisms for estimating certain fingerprint
 *            features as provided by the 'FJFXFingerprint.h' header file.
 *            Adds methods for minutiae points extraction from an image
 *            thanks to the FJFX library.
 *
 * @author Alexandre Tullot
 */

#include <thimble/finger/FJFXFingerprint.h>

/**
 * @brief
 *            Overrides the empty method that estimates the minutiae points.
 *            See 'FJFXFingerprint.h' for more details.
 *
 **/
MinutiaeView FJFXFingerprint::estimateMinutiae() {

  // The fingerprint's image dimension
  int m, n;
  m = getHeight();
  n = getWidth();

  // Allocate memory for raw pixel data of the fingerprint image
  uint8_t *raw_image = (uint8_t *)malloc(m * n);
  if (raw_image == NULL) {
    cerr << "FJFXFingerprint::estimateMinutiae: out of memory." << endl;
    exit(EXIT_FAILURE);
  }

  // Convert the intensities from [0.0,1.0] to 8-bit values in [0,255]
  for (int y = 0; y < m; y++) {
    for (int x = 0; x < n; x++) {
      raw_image[y * n + x] = (uint8_t)(getIntensityImage()[y * n + x] * 255.0);
    }
  }

  // fingerprint minutiae data
  uint8_t fmd[FJFX_FMD_BUFFER_SIZE];
  unsigned int size_of_fmd = FJFX_FMD_BUFFER_SIZE;

  // ***********************************************************************
  // ****** CALL OF DIGITAL PERSONA'S FINGERJETFX OSE LIBRARY FUNCTION *****
  // ***********************************************************************
  // ****** Remark: the function allocates some memory of size of orders ***
  // ****** the pixel data that is, however, not freed afterwards **********
  // ***********************************************************************
  int code =
      fjfx_create_fmd_from_raw(raw_image, getResolution(), m, n,
                               FJFX_FMD_ISO_19794_2_2005, fmd, &size_of_fmd);

  MinutiaeRecord record;

  if (code == FJFX_SUCCESS) {

    // If the data output by Digital Persona's FingerJetFX OSE function
    // cannot be read by THIMBLE's MinutiaeRecord class, we would expect
    // it to be a bug in THIMBLE; however, an error should not occur and
    // we have not experienced any for the present version of THIMBLE
    // as of now.
    if (!record.fromBytes(fmd)) {
      cerr << "ERROR: probably a bug in THIMBLE." << endl;
      exit(EXIT_FAILURE);
    }

  } else {

    // If, for which reason ever, FJFX has not output a valid minutiae
    // estimate, we use an empty view instead which might result in
    // a treatment of a 'failure to enrolment' later.
    record.addView(MinutiaeView());
  }

  // Memories, you are free
  free(raw_image);

  // Return the view
  return record.getView();
}
