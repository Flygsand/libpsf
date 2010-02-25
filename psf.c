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

psf_tag *__psf_tag_malloc() {
  psf_tag *tag = (psf_tag *) malloc(sizeof(psf_tag));
  
  if (!tag)
    return NULL;

  tag->title = NULL;
  tag->artist = NULL;
  tag->game = NULL;
  tag->year = NULL;
  tag->genre = NULL;
  tag->comment = NULL;
  tag->copyright = NULL;
  tag->created_by = NULL;
  tag->volume = 1.0;
  tag->length = NULL;
  tag->fade = NULL;
  tag->utf8_enc = false;

  return tag;
}

void __psf_tag_free(psf_tag *tag) {
  if (!tag)
    return;

  free(tag->title);
  free(tag->artist);
  free(tag->game);
  free(tag->year);
  free(tag->genre);
  free(tag->comment);
  free(tag->copyright);
  free(tag->created_by);
  free(tag->length);
  free(tag->fade);
  free(tag);
}

size_t  __psf_tag_parse_var(char *tag_data, 
                             char **var_name, 
                             char **value) {
  char p_var_name[256], p_value[1024], p_next_var_name[256];
  unsigned int chars_read;

  p_var_name[0] = '\0';
  p_value[0] = '\0';
  p_next_var_name[0] = '\0';
  
  if (sscanf(tag_data, "%256[^=]=%1024[^\n]\n%n%256[^=]=", 
             p_var_name, p_value, &chars_read, p_next_var_name) >= 2) {

    if (*var_name == NULL) {
      if (!(*var_name = (char *) malloc(sizeof(p_var_name))))
        return chars_read;

      strcpy(*var_name, p_var_name);
    }

    if (*value == NULL) {
      if (!(*value = (char *) malloc(sizeof(p_value))))
        return chars_read;
      
      strcpy(*value, p_value);
    } else {
      if (!realloc(*value, sizeof(*value) + sizeof(p_value)))
        return chars_read;

      strcat(*value, p_value);
    }

    if (!strcmp(p_var_name, p_next_var_name))
      return chars_read + __psf_tag_parse_var(tag_data + chars_read, var_name, value);

  }
  
  return chars_read;

}

void __psf_tag_set_var(psf_tag *tag, char *var_name, char *value) {
  if (!strcmp(var_name, "title"))
    tag->title = value;
  else if (!strcmp(var_name, "artist"))
    tag->artist = value;
  else if (!strcmp(var_name, "game"))
    tag->game = value;
  else if (!strcmp(var_name, "year"))
    tag->year = value;
  else if (!strcmp(var_name, "genre"))
    tag->genre = value;
  else if (!strcmp(var_name, "comment"))
    tag->comment = value;
  else if (!strcmp(var_name, "copyright"))
    tag->copyright = value;
  else if (!strcmp(var_name, "psfby") ||
           !strcmp(var_name, "ssfby") ||
           !strcmp(var_name, "dsfby") ||
           !strcmp(var_name, "usfby") ||
           !strcmp(var_name, "qsfby"))
    tag->created_by = value;
  else if (!strcmp(var_name, "volume"))
    tag->volume = strtof(value, NULL);
  else if (!strcmp(var_name, "length"))
    tag->length = value;
  else if (!strcmp(var_name, "fade"))
    tag->fade = value;
  else if (!strcmp(var_name, "utf8"))
    tag->utf8_enc = true;
}

psf_tag *__psf_tag_parse(char *tag_data) {
  psf_tag *tag;
  
  if (!(tag = __psf_tag_malloc()))
    return NULL;

  char *ptr = tag_data;
 
  char *var_name = NULL, *value = NULL;
  size_t tag_len = strlen(tag_data);
  size_t chars_read = 0;

  while ( (ptr - tag_data) < tag_len) {
    ptr += __psf_tag_parse_var(ptr, &var_name, &value);
  
    if (var_name != NULL) {
      __psf_tag_set_var(tag, var_name, value);
      free(var_name);
      var_name = NULL;
      value = NULL;
    }
  }

  return tag;
}

psf *__psf_malloc() {
  psf *p;

  if (!(p = (psf *) malloc(sizeof(psf))))
    return NULL;

  p->version = 0;
  p->rsvd_size = p->prg_size = 0;
  p->crc32 = 0;
  p->rsvd = p->prg = NULL;
  p->tag = NULL;

  return p;
}

psf *psf_read(FILE *fp) {
  
  const size_t buf_size = 1024;
  char buf[buf_size];
  size_t bytes_read = 0;
  psf *p;

  // First 3 bytes: ASCII signature: "PSF" (case sensitive)
  bytes_read = fread(buf, 1, 3, fp);
  buf[bytes_read] = '\0';
 
  if (strcmp(buf, "PSF") != 0)
    return NULL;

  if (!(p = __psf_malloc()))
    return NULL;
  
  if (!(
      // Next 1 byte: Version byte
      fread(&p->version, 1, 1, fp) == 1 && 
      
      // Next 4 bytes: Size of reserved area
      fread(&p->rsvd_size, 4, 1, fp) == 1 &&

      // Next 4 bytes: Compressed program length
      fread(&p->prg_size, 4, 1, fp) == 1 &&

      // Next 4 bytes: Compressed program CRC-32
      fread(&p->crc32, 4, 1, fp) == 1 &&
 
      // Next rsvd_size bytes: Reserved area.
      (p->rsvd = (uint8_t *) malloc(p->rsvd_size)) &&
      fread(p->rsvd, 1, p->rsvd_size, fp) == p->rsvd_size &&
  
      // Next prg_size bytes: Compressed program, 
      // in zlib compress() format.
      (p->prg = (uint8_t *) malloc(p->prg_size)) &&
      fread(p->prg, 1, p->prg_size, fp) == p->prg_size && 
      
      // CRC
      (p->prg == NULL || p->crc32 == crc32(0L, p->prg, p->prg_size))
        )) {
    
    psf_free(p);
    return NULL;
  }
  
  // Next 5 bytes: ASCII signature: "[TAG]" (case sensitive)
  bytes_read = fread(buf, 1, 5, fp);
  buf[bytes_read] = '\0';
  
  if (strcmp(buf, "[TAG]") == 0) {
 
    // Remainder of file: Tag data.
    size_t tag_size = 0;
    char *tag_data = NULL, *tag_data_realloc = NULL;
    while( (bytes_read = fread(buf, 1, buf_size, fp)) > 0) {
      if (!(tag_data_realloc = (char *) realloc(tag_data, tag_size + bytes_read))) {
        free(tag_data);
        tag_data = NULL;
        break;
      }

      tag_data = tag_data_realloc;
      
      strncpy(tag_data + tag_size, buf, bytes_read);
      tag_size += bytes_read;
    }

    if (tag_data) {
      p->tag = __psf_tag_parse(tag_data);
      free(tag_data);
    }
  } 

  return p;
}

void psf_free(psf *p) {
  free(p->rsvd);
  free(p->prg);
  __psf_tag_free(p->tag);
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
  
  uint8_t *dcmp_prg = NULL, *dcmp_prg_realloc = NULL;
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
    if (!(dcmp_prg_realloc = realloc(dcmp_prg, dcmp_prg_size + dcmp_bytes))) {
      free(dcmp_prg);
      return Z_MEM_ERROR;
    }
    
    dcmp_prg = dcmp_prg_realloc;
    memcpy(dcmp_prg + dcmp_prg_size, buf, dcmp_bytes);
    dcmp_prg_size += dcmp_bytes;
  } while (strm.avail_out == 0);
 
  inflateEnd(&strm);
  
  free(p->prg);
  p->prg_size = dcmp_prg_size;
  if (!(p->prg = (uint8_t *) malloc(dcmp_prg_size)))
    return Z_MEM_ERROR;

  memcpy(p->prg, dcmp_prg, dcmp_prg_size);
  free(dcmp_prg);

  return Z_OK;
}
