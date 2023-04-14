#ifndef _common_h
#define _common_h

#define ASSERT(x, e) do { \
    if (!(x)) { \
        if (error) \
            *error = e; \
        \
        return 0; \
    } \
} while (0)

#endif