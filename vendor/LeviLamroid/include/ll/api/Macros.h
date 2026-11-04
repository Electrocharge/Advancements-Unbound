#pragma once
#if defined(__GNUC__)
#define LL_API __attribute__((visibility("default")))
#else
#define LL_API
#endif
