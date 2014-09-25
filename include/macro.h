#ifndef MACRO_H
#define MACRO_H

#define offset_of(type, field) \
	((size_t) &((type *)0)->field)

#define container_of(ptr, type, field) \
	((type *)((char *)(ptr)-offset_of(type, field)))

#endif /* MACRO_H */
