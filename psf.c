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

#include "psf/psf.h"
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

psf *psf_read(FILE *fp) {
  
  const size_t buf_size = 1024;
  char buf[buf_size];
  size_t bytes_read;
  
  // First 3 bytes: ASCII signature: "PSF" (case sensitive)
  bytes_read = fread(buf, 1, 3, fp);
  buf[bytes_read] = '\0';
 
  if (strcmp(buf, "PSF") != 0)
    return NULL;

  psf *p = (psf *) malloc(sizeof(psf));
  p->version = 0;
  p->rsvd_size = p->prg_size = 0;
  p->crc32 = 0;
  p->rsvd = p->prg = NULL;
  p->tag = NULL;
  
  // Next 1 byte: Version byte
  fread(&p->version, 1, 1, fp);

  // Next 4 bytes: Size of reserved area
  fread(&p->rsvd_size, 4, 1, fp);

  // Next 4 bytes: Compressed program length
  fread(&p->prg_size, 4, 1, fp);

  // Next 4 bytes: Compressed program CRC-32
  fread(&p->crc32, 4, 1, fp);
 
  // Next rsvd_size bytes: Reserved area.
  p->rsvd = (uint8_t *) malloc(p->rsvd_size);
  fread(p->rsvd, 1, p->rsvd_size, fp);
  
  // Next prg_size bytes: Compressed program, 
  // in zlib compress() format.
  p->prg = (uint8_t *) malloc(p->prg_size);
  fread(p->prg, 1, p->prg_size, fp);

  if (p->prg != NULL && p->crc32 != crc32(0L, p->prg, p->prg_size)) {
    psf_free(p);
    return NULL;
  }
  
  // Next 5 bytes: ASCII signature: "[TAG]" (case sensitive)
  bytes_read = fread(buf, 1, 5, fp);
  buf[bytes_read] = '\0';
  
  if (strcmp(buf, "[TAG]") == 0) {
  
    // Remainder of file: Tag data.
    size_t tag_size = 0;
    while( (bytes_read = fread(buf, 1, buf_size, fp)) > 0) {
      p->tag = (char *) realloc(p->tag, tag_size + bytes_read);
      strncpy(p->tag + tag_size, buf, bytes_read);
      tag_size += bytes_read;
    }
  } 

  return p;
}

void psf_free(psf *p) {
  free(p->rsvd);
  free(p->prg);
  free(p->tag);
  free(p);
}

int psf_decompress(psf *p) {
  z_stream strm;
  int ret;

  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);

  if (ret != Z_OK)
    return ret;

  strm.avail_in = p->prg_size;
  strm.next_in = p->prg;
  
  uint8_t *dcmp_prg = NULL;
  size_t dcmp_prg_size = 0, dcmp_bytes = 0;
  size_t buf_size = 131072;
  uint8_t buf[buf_size];
    
  do {
    strm.avail_out = buf_size;
    strm.next_out = buf;
    ret = inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
    
    case Z_NEED_DICT:
      ret = Z_DATA_ERROR;
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
      (void)inflateEnd(&strm);
      free(dcmp_prg);
      return ret;
    }

    dcmp_bytes = buf_size - strm.avail_out;
    dcmp_prg = realloc(dcmp_prg, dcmp_prg_size + dcmp_bytes);
    memcpy(dcmp_prg + dcmp_prg_size, buf, dcmp_bytes);
    dcmp_prg_size += dcmp_bytes;
  } while (strm.avail_out == 0);
 
  inflateEnd(&strm);
  
  free(p->prg);
  p->prg_size = dcmp_prg_size;
  p->prg = (uint8_t *) malloc(dcmp_prg_size);
  memcpy(p->prg, dcmp_prg, dcmp_prg_size);
  free(dcmp_prg);

  return Z_OK;
}
