#pragma once

#define NUMOF(x) (uint32_t)((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))