#include <poker-eval/poker_defs.h>
#include <poker-eval/inlines/eval.h>
#include <poker-eval/inlines/eval_low.h>
#include <poker-eval/inlines/eval_low8.h>
#include <poker-eval/deck_std.h>
#include <poker-eval/rules_std.h>
#include <poker-eval/enumerate.h>
#include <poker-eval/enumdefs.h>
#include "enum.h"


#define INNER_LOOP(evalwrap)            \
    do {                \
      int i;                \
      HandVal hival[ENUM_MAXPLAYERS];         \
      LowHandVal loval[ENUM_MAXPLAYERS];        \
      HandVal besthi = HandVal_NOTHING;         \
      LowHandVal bestlo = LowHandVal_NOTHING;       \
      int hishare = 0;              \
      int loshare = 0;              \
      double hipot, lopot;            \
      /* find winning hands for high and low */       \
      for (i=0; i<npockets; i++) {          \
  int err;              \
        { evalwrap }              \
        if (err != 0)             \
          return 1000 + err;            \
        if (hival[i] != HandVal_NOTHING) {        \
          if (hival[i] > besthi) {          \
            besthi = hival[i];            \
            hishare = 1;            \
          } else if (hival[i] == besthi) {        \
            hishare++;              \
          }               \
        }               \
        if (loval[i] != LowHandVal_NOTHING) {       \
          if (loval[i] < bestlo) {          \
            bestlo = loval[i];            \
            loshare = 1;            \
          } else if (loval[i] == bestlo) {        \
            loshare++;              \
          }               \
        }               \
      }                 \
      /* now award pot fractions to winning hands */      \
      if (bestlo != LowHandVal_NOTHING &&       \
          besthi != HandVal_NOTHING) {          \
        hipot = 0.5 / hishare;            \
        lopot = 0.5 / loshare;            \
      } else if (bestlo == LowHandVal_NOTHING &&      \
                 besthi != HandVal_NOTHING) {       \
        hipot = 1.0 / hishare;            \
        lopot = 0;              \
      } else if (bestlo != LowHandVal_NOTHING &&      \
                 besthi == HandVal_NOTHING) {       \
        hipot = 0;              \
        lopot = 1.0 / loshare;            \
      } else {                \
        hipot = lopot = 0;            \
      }                 \
      for (i=0; i<npockets; i++) {          \
        double potfrac = 0;           \
        int H = 0, L = 0;           \
        if (hival[i] != HandVal_NOTHING) {        \
          if (hival[i] == besthi) {         \
            H = hishare;            \
            potfrac += hipot;           \
            if (hishare == 1)           \
              result->nwinhi[i]++;          \
            else              \
              result->ntiehi[i]++;          \
          } else {              \
            result->nlosehi[i]++;         \
          }               \
        }               \
        if (loval[i] != LowHandVal_NOTHING) {       \
          if (loval[i] == bestlo) {         \
            L = loshare;            \
            potfrac += lopot;           \
            if (loshare == 1)           \
              result->nwinlo[i]++;          \
            else              \
              result->ntielo[i]++;          \
          } else {              \
            result->nloselo[i]++;         \
          }               \
        }               \
        result->nsharehi[i][H]++;         \
        result->nsharelo[i][L]++;         \
        result->nshare[i][H][L]++;          \
        if (potfrac > 0.99)           \
          result->nscoop[i]++;            \
        result->ev[i] += potfrac;         \
      }                 \
      if (result->ordering != NULL) {         \
        if (result->ordering->mode == enum_ordering_mode_hi) {    \
          int hiranks[ENUM_ORDERING_MAXPLAYERS];      \
          ENUM_ORDERING_RANK_HI(hival, HandVal_NOTHING, npockets, hiranks);\
          ENUM_ORDERING_INCREMENT(result->ordering, npockets, hiranks); \
        }               \
        if (result->ordering->mode == enum_ordering_mode_lo) {    \
          int loranks[ENUM_ORDERING_MAXPLAYERS];      \
          ENUM_ORDERING_RANK_LO(loval, LowHandVal_NOTHING, npockets, loranks);\
          ENUM_ORDERING_INCREMENT(result->ordering, npockets, loranks); \
        }               \
        if (result->ordering->mode == enum_ordering_mode_hilo) {  \
          int hiranks[ENUM_ORDERING_MAXPLAYERS_HILO];     \
          int loranks[ENUM_ORDERING_MAXPLAYERS_HILO];     \
          ENUM_ORDERING_RANK_HI(hival, HandVal_NOTHING, npockets, hiranks);\
          ENUM_ORDERING_RANK_LO(loval, LowHandVal_NOTHING, npockets, loranks);\
          ENUM_ORDERING_INCREMENT_HILO(result->ordering, npockets,  \
                                       hiranks, loranks);   \
        }               \
      }                 \
      result->nsamples++;           \
    } while (0);


#define INNER_LOOP_5DRAW            \
  INNER_LOOP({                \
    StdDeck_CardMask _hand;           \
    StdDeck_CardMask_OR(_hand, pockets[i], unsharedCards[i]);   \
    hival[i] = StdDeck_StdRules_EVAL_N(_hand, 5);     \
    loval[i] = LowHandVal_NOTHING;          \
    err = 0;                \
  })

#define INNER_LOOP_5DRAW8           \
  INNER_LOOP({                \
    StdDeck_CardMask _hand;           \
    StdDeck_CardMask_OR(_hand, pockets[i], unsharedCards[i]);   \
    hival[i] = StdDeck_StdRules_EVAL_N(_hand, 5);     \
    loval[i] = StdDeck_Lowball8_EVAL(_hand, 5);     \
    err = 0;                \
  })

#define INNER_LOOP_5DRAWNSQ           \
  INNER_LOOP({                \
    StdDeck_CardMask _hand;           \
    StdDeck_CardMask_OR(_hand, pockets[i], unsharedCards[i]);   \
    hival[i] = StdDeck_StdRules_EVAL_N(_hand, 5);     \
    loval[i] = StdDeck_Lowball_EVAL(_hand, 5);      \
    err = 0;                \
  })

#define INNER_LOOP_LOWBALL            \
  INNER_LOOP({                \
    StdDeck_CardMask _hand;           \
    StdDeck_CardMask_OR(_hand, pockets[i], unsharedCards[i]);   \
    hival[i] = HandVal_NOTHING;           \
    loval[i] = StdDeck_Lowball_EVAL(_hand, 5);      \
    err = 0;                \
  })


int 
stdEnumExhaustive(enum_game_t game, StdDeck_CardMask pockets[],
                  StdDeck_CardMask board, StdDeck_CardMask dead,
                  int npockets, int nboard, int orderflag,
                  enum_result_t *result) {
  int i;

  switch (game) {
    case game_5draw:
    case game_5draw8:
    case game_5drawnsq:
    case game_lowball:
      break;
    default:
      return enumExhaustive(game, pockets, board, dead, npockets, nboard, orderflag, result);
  }

  enumResultClear(result);
  if (npockets > ENUM_MAXPLAYERS)
    return 1;

  if (orderflag) {
    enum_ordering_mode_t mode;
    switch (game) {
      case game_5draw:
        mode = enum_ordering_mode_hi;
        break;
      case game_lowball:
        mode = enum_ordering_mode_lo;
        break;
      case game_5draw8:
      case game_5drawnsq:
        mode = enum_ordering_mode_hilo;
        break;
      default:
        return 1;
    }
    if (enumResultAlloc(result, npockets, mode))
      return 1;
  }

  if (game == game_5draw) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_ENUMERATE_COMBINATIONS_D(StdDeck, unsharedCards,
                                  npockets, numToDeal,
                                  dead, INNER_LOOP_5DRAW);

  } else if (game == game_5draw8) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_ENUMERATE_COMBINATIONS_D(StdDeck, unsharedCards,
                                  npockets, numToDeal,
                                  dead, INNER_LOOP_5DRAW8);

  } else if (game == game_5drawnsq) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_ENUMERATE_COMBINATIONS_D(StdDeck, unsharedCards,
                                  npockets, numToDeal,
                                  dead, INNER_LOOP_5DRAWNSQ);

  } else if (game == game_lowball) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_ENUMERATE_COMBINATIONS_D(StdDeck, unsharedCards,
                                  npockets, numToDeal,
                                  dead, INNER_LOOP_LOWBALL);

  } else {
    return 1;
  }

  result->game = game;
  result->nplayers = npockets;
  result->sampleType = ENUM_EXHAUSTIVE;
  return 0;  
}


int 
stdEnumSample(enum_game_t game, StdDeck_CardMask pockets[],
              StdDeck_CardMask board, StdDeck_CardMask dead,
              int npockets, int nboard, int niter, int orderflag,
              enum_result_t *result) {
  int i;

  switch (game) {
    case game_5draw:
    case game_5draw8:
    case game_5drawnsq:
    case game_lowball:
      break;
    default:
      return enumSample(game, pockets, board, dead, npockets, nboard, niter, orderflag, result);
  }

  enumResultClear(result);
  if (npockets > ENUM_MAXPLAYERS)
    return 1;
  if (orderflag) {
    enum_ordering_mode_t mode;
    switch (game) {
      case game_5draw:
        mode = enum_ordering_mode_hi;
        break;
      case game_lowball:
        mode = enum_ordering_mode_lo;
        break;
      case game_5draw8:
      case game_5drawnsq:
        mode = enum_ordering_mode_hilo;
        break;
      default:
        return 1;
    }
    if (enumResultAlloc(result, npockets, mode))
      return 1;
  }

  if (game == game_5draw) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, unsharedCards,
                                   npockets, numToDeal,
                                   dead, niter, INNER_LOOP_5DRAW);

  } else if (game == game_5draw8) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, unsharedCards,
                                   npockets, numToDeal,
                                   dead, niter, INNER_LOOP_5DRAW8);

  } else if (game == game_5drawnsq) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, unsharedCards,
                                   npockets, numToDeal,
                                   dead, niter, INNER_LOOP_5DRAWNSQ);

  } else if (game == game_lowball) {
    /* we have a type problem: pockets should be JokerDeck_CardMask[] */
    StdDeck_CardMask unsharedCards[ENUM_MAXPLAYERS];
    int numToDeal[ENUM_MAXPLAYERS];
    for (i=0; i<npockets; i++)
      numToDeal[i] = 5 - StdDeck_numCards(pockets[i]);
    DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, unsharedCards,
                                   npockets, numToDeal,
                                   dead, niter, INNER_LOOP_LOWBALL);

  } else {
    return 1;
  }

  result->game = game;
  result->nplayers = npockets;
  result->sampleType = ENUM_SAMPLE;
  return 0;  
}
