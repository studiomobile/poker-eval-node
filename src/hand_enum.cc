#include "hand_enum.h"
#include "game.h"
#include "helpers.h"
#include "enum.h"
#include <string.h>

long cnk(int n, int k);

NAN_METHOD(EnumHand)
{
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (info.Length() < 2 || !info[1]->IsArray())  TYPE_ERROR("Please provide player hand as second argument");
  if (info.Length() < 3 || !info[2]->IsArray())  TYPE_ERROR("Please provide board cards as third argument");
  if (info.Length() < 4 || !info[3]->IsNumber()) TYPE_ERROR("Please provide number of other players");

  Nan::Utf8String gameTypeStr(info[0]);
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

  Local<Array> pocket = Local<Array>::Cast(info[1]);
  READ_CARD_MASK_WITH_COLLECTOR(pocket, pockets[0], dead, in_hand);
  if (in_hand < game->minpocket) TYPE_ERROR("Too few cards in hand");
  if (in_hand > game->maxpocket) TYPE_ERROR("Too many cards in hand");
  pocket_sizes[0] = game->maxpocket - in_hand;

  Local<Array> _board = Local<Array>::Cast(info[2]);
  READ_CARD_MASK_WITH_COLLECTOR(_board, board, dead, on_board);
  if (on_board > game->maxboard) TYPE_ERROR("Too many cards on board");

  players = info[3]->IntegerValue();
  if (players < 2) TYPE_ERROR("Should be at least 2 players in game");
  if (players > ENUM_MAXPLAYERS) TYPE_ERROR("Too many players in game");

  Local<String> deadStr    = Nan::New<String>("dead").ToLocalChecked();
  Local<String> samplesStr = Nan::New<String>("samples").ToLocalChecked();
  Local<String> equityStr  = Nan::New<String>("equity").ToLocalChecked();
  Local<String> hiStr      = Nan::New<String>("hi").ToLocalChecked();
  Local<String> loStr      = Nan::New<String>("lo").ToLocalChecked();
  Local<String> winStr     = Nan::New<String>("win").ToLocalChecked();
  Local<String> loseStr    = Nan::New<String>("lose").ToLocalChecked();
  Local<String> tieStr     = Nan::New<String>("tie").ToLocalChecked();

  if (info.Length() > 4 && info[4]->IsObject()) {
    Local<Object> opt = info[4]->ToObject();
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

  enum_result_t enumResult;
  enumResultClear(&enumResult);
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
    enumResult.ev[0] += intermediate.ev[0];
    enumResult.nsamples += intermediate.nsamples;
    if (game->hashipot) {
      enumResult.nwinhi[0] += intermediate.nwinhi[0];
      enumResult.ntiehi[0] += intermediate.ntiehi[0];
      enumResult.nlosehi[0] += intermediate.nlosehi[0];
    }
    if (game->haslopot) {
      enumResult.nwinlo[0] += intermediate.nwinlo[0];
      enumResult.ntielo[0] += intermediate.ntielo[0];
      enumResult.nloselo[0] += intermediate.nloselo[0];
    }
  });

  Local<Object> result = Nan::New<Object>();
  Nan::Set(result, equityStr, Nan::New<Number>(enumResult.ev[0] / enumResult.nsamples));
  Nan::Set(result, samplesStr, Nan::New<Integer>(enumResult.nsamples));
  if (game->hashipot) {
    Local<Object> hi = Nan::New<Object>();
    Nan::Set(result, hiStr, hi);
    Nan::Set(hi, winStr, Nan::New<Integer>(enumResult.nwinhi[0]));
    Nan::Set(hi, tieStr, Nan::New<Integer>(enumResult.ntiehi[0]));
    Nan::Set(hi, loseStr, Nan::New<Integer>(enumResult.nlosehi[0]));
  }
  if (game->haslopot) {
    Local<Object> lo = Nan::New<Object>();
    Nan::Set(result, loStr, lo);
    Nan::Set(lo, winStr, Nan::New<Integer>(enumResult.nwinlo[0]));
    Nan::Set(lo, tieStr, Nan::New<Integer>(enumResult.ntielo[0]));
    Nan::Set(lo, loseStr, Nan::New<Integer>(enumResult.nloselo[0]));
  }

  info.GetReturnValue().Set(result);
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
