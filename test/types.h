/* types.h -- TODO */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

#define container_of(ptr, type, field) \
  ((type *)((char *)(ptr)-offsetof(type, field)))

#endif /* TYPES_H */

