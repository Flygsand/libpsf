#ifndef _COMMON_H_
#define _COMMON_H_

#include "../psf/psf.h"

typedef struct {
  const char *inputfile;
} test_conf;

void run_test(void (*test)(psf *), test_conf conf);

#endif /* _COMMON_H_ */
