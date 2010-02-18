/* 
 
Copyright (C) 2010 Martin Häger
 
This file is part of the libpsf test suite.
 
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

#include "common.h"
#include "test_psf2.h"
#include "../psf/psf2.h"
#include <stdio.h>

test_conf conf = {
  .inputfile = "/tmp/song.psf2",
};

void test_psf2_print_fs_tree(psf *p) {
  psf2_print_fs_tree(p);
}

void test_psf2() {
  run_test(test_psf2_print_fs_tree, conf);
}
