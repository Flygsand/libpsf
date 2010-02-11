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

#include "psf2.h"
#include <stdlib.h>
#include <string.h>

void recursive_print_fs_tree(psf *p, size_t offset, unsigned int indent_level) {
  printf("/\n");
  
  uint8_t *fs = p->rsvd + offset;
  
  unsigned int dir_count;
  char *filename = (char *) malloc(36);
  size_t data_offset, dcmp_size, block_size;

  memcpy(&dir_count, fs, 4);
  fs += 4;

  for (unsigned int dir = 0; dir < dir_count; dir++) {
    for (unsigned int ind = 0; ind < indent_level - 1; ind++)
      printf("  ");

    printf("|-");
    
    strncpy(filename, (const char *)fs, 36); fs += 36;
    memcpy(&data_offset, fs, 4); fs += 4;
    memcpy(&dcmp_size, fs, 4); fs += 4;
    memcpy(&block_size, fs, 4); fs += 4;
    
    printf("%s", filename);

    if (dcmp_size == 0 && block_size == 0 && data_offset != 0)
      recursive_print_fs_tree(p, data_offset, indent_level + 1);
    else
      printf("\n");
  }
}

void psf2_print_fs_tree(psf *p) {
  recursive_print_fs_tree(p, 0, 1);
}


