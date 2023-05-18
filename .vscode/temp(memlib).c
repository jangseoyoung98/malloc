#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "memlib.h"
#include "config.h"

static char* mem_start_brk;
static char* mem_brk;
static char* mem_max_addr;


