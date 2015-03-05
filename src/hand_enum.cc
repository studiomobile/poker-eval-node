#include "hand_enum.h"
#include "game.h"
#include "helpers.h"
#include "enum.h"
#include <string.h>

long cnk(int n, int k);

NAN_METHOD(EnumHand)
{
  NanScope();

  if (args.Length() < 1 || !args[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (args.Length() < 2 || !args[1]->IsArray())  TYPE_ERROR("Please provide player hand as second argument");
  if (args.Length() < 3 || !args[2]->IsArray())  TYPE_ERROR("Please provide board cards as third argument");
  if (args.Length() < 4 || !args[3]->IsNumber()) TYPE_ERROR("Please provide number of other players");

  NanAsciiString gameTypeStr(args[0]);
  enum_gameparams_t *game = findGame(*gameTypeStr);
  if (!game) TYPE_ERROR("Game type is invalid");
  enum_game_t game_type = game->game;

  int err = 0;
  int ordering = 0;
  int players = 0;
  int samples = 100000;
  int in_hand = 0;
  int on_board = 0;
  int are_dead = 0;
  StdDeck_CardMask dead;
  StdDeck_CardMask board;
  StdDeck_CardMask pockets[ENUM_MAXPLAYERS];
  int pocket_sizes[ENUM_MAXPLAYERS];

  StdDeck_CardMask_RESET(dead);
  StdDeck_CardMask_RESET(board);
  for (uint i = 0; i < ENUM_MAXPLAYERS; ++i) {
    StdDeck_CardMask_RESET(pockets[i]);
    pocket_sizes[i] = game->maxpocket;
  }

  Local<Array> pocket = Local<Array>::Cast(args[1]);
  READ_CARD_MASK_WITH_COLLECTOR(pocket, pockets[0], dead, in_hand);
  if (in_hand < game->minpocket) TYPE_ERROR("Too few cards in hand");
  if (in_hand > game->maxpocket) TYPE_ERROR("Too many cards in hand");
  pocket_sizes[0] = game->maxpocket - in_hand;

  Local<Array> _board = Local<Array>::Cast(args[2]);
  READ_CARD_MASK_WITH_COLLECTOR(_board, board, dead, on_board);
  if (on_board > game->maxboard) TYPE_ERROR("Too many cards on board");

  players = args[3]->IntegerValue();
  if (players < 2) TYPE_ERROR("Should be at least 2 players in game");
  if (players > ENUM_MAXPLAYERS) TYPE_ERROR("Too many players in game");

  Local<String> deadStr    = NanNew<String>("dead");
  Local<String> samplesStr = NanNew<String>("samples");
  Local<String> equityStr  = NanNew<String>("equity");
  Local<String> hiStr      = NanNew<String>("hi");
  Local<String> loStr      = NanNew<String>("lo");
  Local<String> winStr     = NanNew<String>("win");
  Local<String> loseStr    = NanNew<String>("lose");
  Local<String> tieStr     = NanNew<String>("tie");

  if (args.Length() > 4 && args[4]->IsObject()) {
    Local<Object> opt = args[4]->ToObject();
    if (opt->HasOwnProperty(deadStr)) {
      Local<Value> d = opt->Get(deadStr);
      if (!d->IsArray()) TYPE_ERROR("Dead cards should be array of cards");
      Local<Array> _dead = Local<Array>::Cast(d);
      READ_CARD_MASK(_dead, dead, are_dead);
    }
    if (opt->HasOwnProperty(samplesStr)) {
      Local<Value> i = opt->Get(samplesStr);
      if (!i->IsNumber()) TYPE_ERROR("Number of samples is not a number");
      samples = i->IntegerValue();
      if (samples <= 0) TYPE_ERROR("Number of samples should be positite");
    }
  }

  long handSamples = 0;
  long boardSamples = 0;
  int in_deck = StdDeck_N_CARDS - in_hand - on_board - are_dead;
  int deal_to_players = players * game->maxpocket - in_hand;
  if (game->maxboard > on_board) {
    handSamples = (players - 1) * 1000;
    boardSamples = cnk(in_deck - deal_to_players, game->maxboard - on_board);
    while (boardSamples * handSamples > samples) {
      boardSamples /= 2;
      handSamples /= 2;
      if (!boardSamples) boardSamples = 1;
      if (!handSamples)  handSamples = 1;
    }
  } else {
    handSamples = cnk(in_deck, deal_to_players);
    if (handSamples > samples) handSamples = samples;
  }

  enum_result_t result;
  enumResultClear(&result);
  enum_result_t intermediate;

  int _players = players - 1;
  int *_pocket_sizes = pocket_sizes + 1;
  StdDeck_CardMask *_pockets = pockets + 1;

  DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, _pockets, _players, _pocket_sizes, dead, handSamples, {
    if (boardSamples > 0) {
      err = stdEnumSample(game_type, pockets, board, _used, players, on_board, boardSamples, ordering, &intermediate);
    } else {
      err = stdEnumExhaustive(game_type, pockets, board, _used, players, on_board, ordering, &intermediate);
    }
    if (err) TYPE_ERROR("Enumeration failed");
    result.ev[0] += intermediate.ev[0];
    result.nsamples += intermediate.nsamples;
    if (game->hashipot) {
      result.nwinhi[0] += intermediate.nwinhi[0];
      result.ntiehi[0] += intermediate.ntiehi[0];
      result.nlosehi[0] += intermediate.nlosehi[0];
    }
    if (game->haslopot) {
      result.nwinlo[0] += intermediate.nwinlo[0];
      result.ntielo[0] += intermediate.ntielo[0];
      result.nloselo[0] += intermediate.nloselo[0];
    }
  });

  Local<Object> info = NanNew<Object>();
  info->Set(equityStr, NanNew<Number>(result.ev[0] / result.nsamples));
  info->Set(samplesStr, NanNew<Integer>(result.nsamples));
  if (game->hashipot) {
    Local<Object> hi = NanNew<Object>();
    info->Set(hiStr, hi);
    hi->Set(winStr, NanNew<Integer>(result.nwinhi[0]));
    hi->Set(tieStr, NanNew<Integer>(result.ntiehi[0]));
    hi->Set(loseStr, NanNew<Integer>(result.nlosehi[0]));
  }
  if (game->haslopot) {
    Local<Object> lo = NanNew<Object>();
    info->Set(loStr, lo);
    lo->Set(winStr, NanNew<Integer>(result.nwinlo[0]));
    lo->Set(tieStr, NanNew<Integer>(result.ntielo[0]));
    lo->Set(loseStr, NanNew<Integer>(result.nloselo[0]));
  }

  NanReturnValue(info);
}


long cnk_table[StdDeck_N_CARDS][StdDeck_N_CARDS];
int  cnk_table_initialized = 0;

long cnk(int n, int k)
{
  if (n < k) return 0;
  if (k == 0 || n == 1 || n == k) return 1;
  if (n == 2 && k == 1) return 2;
  if (!cnk_table_initialized) {
    memset(cnk_table, 0, sizeof(cnk_table));
    cnk_table_initialized = 1;
  }
  long res = cnk_table[n][k];
  if (!res) {
    res = cnk_table[n][k] = cnk(n-1, k) + cnk(n-1, k-1);
  }
  return res;
}
