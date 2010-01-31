/* 

Copyright (C) 2010 Martin Häger

This program extracts a PlayStation executable from a PSF file.

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.
     
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#include <psf.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <zlib.h>

#define EXIT_SUCCESS 0
#define EXIT_USAGE 1
#define EXIT_IO_ERROR 2
#define EXIT_INVALID_PSF 3
#define EXIT_ZLIB_ERROR 4

#define COMPLAIN_AND_EXIT(EXIT_CODE, FORMAT, ...) { \
fprintf(stderr, FORMAT "\n", ##__VA_ARGS__);         \
exit(EXIT_CODE);                                \
}

int main(int argc, char *argv[])
{
  /* Check input parameters */
  if (argc != 3)
    COMPLAIN_AND_EXIT(EXIT_USAGE, "usage: %s <input.psf> <output.exe>", basename(argv[0]));

  /* Read infile (PSF) */
  FILE *infile = fopen(argv[1], "rb");
  if (infile == NULL)
    COMPLAIN_AND_EXIT(EXIT_IO_ERROR, "Could not read from \"%s\".", argv[1]);

  psf *p = psf_read(infile);
  fclose(infile);
  if (p == NULL)
    COMPLAIN_AND_EXIT(EXIT_INVALID_PSF, "\"%s\" not a valid PSF file.", argv[1]);

  /* Decompress program */
  if (psf_decompress(p) != Z_OK) {
    psf_free(p);
    COMPLAIN_AND_EXIT(EXIT_ZLIB_ERROR, "Decompression failed.");
  }
  
  FILE *outfile = fopen(argv[2], "wb");
  if (outfile == NULL) {
    COMPLAIN_AND_EXIT(EXIT_IO_ERROR, "Could not write to \"%s\"", argv[2]);
    psf_free(p);
  }

  /* Write program to outfile */
  fwrite(p->prg, 1, p->prg_size, outfile);
  fclose(outfile);
  psf_free(p);

  return EXIT_SUCCESS;
}
