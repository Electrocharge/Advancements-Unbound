#pragma once
#if defined(__GNUC__)
#define PL_EXPORT __attribute__((visibility("default")))
#else
#define PL_EXPORT
#endif
