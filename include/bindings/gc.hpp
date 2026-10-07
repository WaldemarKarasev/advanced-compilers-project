#pragma once

#ifdef __cplusplus

#define _Static_assert static_assert
#define _Noreturn [[noreturn]]
#define _Alignof alignof

extern "C" {
#include <stella/gc.h>
}

#undef _Alignof
#undef _Noreturn
#undef _Static_assert

#endif