#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include "globaldef.h"

_u64 _rdtsc_(void);
_u64 _bsr64_(_u64 value);