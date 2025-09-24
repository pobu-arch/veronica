#pragma once
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define exit(status) renamed_exit(status)

void renamed_exit(int res);

#ifdef __cplusplus
}
#endif