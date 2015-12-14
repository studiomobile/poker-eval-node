#ifndef __HELPERS_H__
#define __HELPERS_H__


#define TYPE_ERROR(message) { Nan::ThrowTypeError(message); return; }


#define READ_CARD_MASK(cards, pocket, count) \
  for (uint32_t __i = 0; __i < cards->Length(); ++__i) { \
    int __card; Nan::Utf8String __ascii(cards->Get(__i)); \
    if (Deck_stringToCard(*__ascii, &__card) && !CardMask_CARD_IS_SET(pocket, __card)) { \
      CardMask_SET(pocket, __card); ++count; \
    } }


#define READ_CARD_MASK_WITH_COLLECTOR(cards, pocket, collector, count) \
  for (uint32_t __i = 0; __i < cards->Length(); ++__i) { \
    int __card; Nan::Utf8String __ascii(cards->Get(__i)); \
    if (Deck_stringToCard(*__ascii, &__card) && !CardMask_CARD_IS_SET(collector, __card)) { \
      CardMask_SET(pocket, __card); CardMask_SET(collector, __card); ++count; \
    } }


#endif // __HELPERS_H__