#ifndef __ENUM_H__
#define __ENUM_H__

#ifdef __cplusplus
extern "C" {
#endif

int stdEnumExhaustive(enum_game_t game, StdDeck_CardMask pockets[], StdDeck_CardMask board, StdDeck_CardMask dead,
                      int npockets, int nboard, int orderflag, enum_result_t *result);

int stdEnumSample(enum_game_t game, StdDeck_CardMask pockets[], StdDeck_CardMask board, StdDeck_CardMask dead,
                  int npockets, int nboard, int niter, int orderflag, enum_result_t *result);

#ifdef __cplusplus
}
#endif

#endif // __ENUM_H__
