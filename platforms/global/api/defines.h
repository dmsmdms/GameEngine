#pragma once

#define STATIC_ASSERT(cond) _Static_assert(cond, #cond)
#define GET_BY_OFFSET(val, offset) *(__typeof__(val))((char *)val + (offset))
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))
#define WEAK __attribute__((weak))

#define RK_STR(code) [code] = #code
#define RK_VAL(index, val) [index] = val
#define RK_CASE(code) case code: return #code
#define RK_CVT(src, dst) case src: return dst
#define RK_CHECK_FLAGS(exists, required) ((exists & required) == required)
