
/*

Copyright (C) 2010 Martin Häger

This file is part of libpsf.

libpsf is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libpsf is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public
License along with libpsf. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _LIBPSF_H_
#define _LIBPSF_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
  PSF_PSF1 = 0x01, // PlayStation
  PSF_PSF2 = 0x02, // PlayStation 2
  PSF_SSF = 0x11,  // Sega Saturn
  PSF_DSF = 0x12,  // Sega Dreamcast
  PSF_USF = 0x21,  // Nintendo 64
  PSF_GSF = 0x22,  // GameBoy Advance
  PSF_SNSF = 0x23, // Super NES
  PSF_QSF = 0x41   // Capcom QSound
} psf_version;

typedef struct {
  char *title;
  char *artist;
  char *game;
  char *year;
  char *genre;
  char *comment;
  char *copyright;
  char *created_by;
  float volume;
  char *length;
  char *fade;
  bool utf8_enc;
} psf_tag;

typedef struct {
  psf_version version;
  size_t rsvd_size;
  size_t prg_size;
  uint32_t crc32;
  uint8_t *rsvd;
  uint8_t *prg;
  psf_tag *tag;
} psf;

psf *psf_read(FILE *fp);
void psf_free(psf *p);
int psf_decompress(psf *p);

#endif /* _LIBPSF_H_ */
