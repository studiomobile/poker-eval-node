#ifndef __HELPERS_H__
#define __HELPERS_H__

#define EXPAND_ARGS(args, var, code) \
for (int i = 0; i < args.Length(); ++i) { \
  if (args[i]->IsArray()) { \
    Local<Array> __arr = Array::Cast(*args[i]); \
    for (uint32_t __n = 0; __n < __arr->Length(); ++__n) { \
      var = __arr->Get(__n); \
      code; \
    } \
  } else { \
    var = args[i]; \
    code; \
  } \
}

#endif
