#include "common.h"

extern test_conf conf;

void run_test(void (*test)(psf *), test_conf conf) {
  FILE *file = fopen(conf.inputfile, "r");
  psf *psf = psf_read(file);
  
  test(psf);

  psf_free(psf);
  fclose(file);
}
