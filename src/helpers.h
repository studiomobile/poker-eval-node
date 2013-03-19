#ifndef __HELPERS_H__
#define __HELPERS_H__


#define TYPE_ERROR(message) { ThrowException(Exception::TypeError(String::New(message))); return scope.Close(Undefined()); }


#define READ_CARD_MASK(cards, pocket, count) \
  for (uint32_t __i = 0; __i < cards->Length(); ++__i) { \
    int __card; String::AsciiValue __ascii(cards->Get(__i)); \
    if (Deck_stringToCard(*__ascii, &__card) && !CardMask_CARD_IS_SET(pocket, __card)) { \
      CardMask_SET(pocket, __card); ++count; \
    } }


#define READ_CARD_MASK_DEAD(cards, pocket, dead, count) \
  for (uint32_t __i = 0; __i < cards->Length(); ++__i) { \
    int __card; String::AsciiValue __ascii(cards->Get(__i)); \
    if (Deck_stringToCard(*__ascii, &__card) && !CardMask_CARD_IS_SET(dead, __card)) { \
      CardMask_SET(pocket, __card); CardMask_SET(dead, __card); ++count; \
    } }


#endif // __HELPERS_H__