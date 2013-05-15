#include "hand_enum.h"
#include "game.h"
#include "helpers.h"


Handle<Value> EnumHand(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() < 1 || !args[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (args.Length() < 2 || !args[1]->IsArray())  TYPE_ERROR("Please provide player hand as second argument");
  if (args.Length() < 3 || !args[2]->IsArray())  TYPE_ERROR("Please provide board cards as third argument");
  if (args.Length() < 4 || !args[3]->IsNumber()) TYPE_ERROR("Please provide number of other players");

  int players = 0;
  int in_hand = 0;
  int on_board = 0;
  int are_dead = 0;
  int poket_sizes[ENUM_MAXPLAYERS];
  StdDeck_CardMask pockets[ENUM_MAXPLAYERS];
  StdDeck_CardMask dead;
  StdDeck_CardMask board;

  StdDeck_CardMask_RESET(dead);
  StdDeck_CardMask_RESET(board);

  for (uint i = 0; i < ENUM_MAXPLAYERS; ++i) {
    StdDeck_CardMask_RESET(pockets[i]);
  }

  String::AsciiValue gameTypeStr(args[0]);
  enum_gameparams_t *game = findGame(*gameTypeStr);
  if (!game) TYPE_ERROR("Game type is invalid");
  switch (game->game) {
    case game_holdem:
    case game_omaha:
      break;
    default:
      TYPE_ERROR("Game type is unsupported");
  }

  Local<Array> pocket = Local<Array>::Cast(args[1]);
  READ_CARD_MASK_WITH_COLLECTOR(pocket, pockets[0], dead, in_hand);
  if (in_hand < game->minpocket) TYPE_ERROR("Too few cards in hand");
  if (in_hand > game->maxpocket) TYPE_ERROR("Too many cards in hand");

  for (uint i = 0; i < ENUM_MAXPLAYERS; ++i) {
    poket_sizes[i] = in_hand;
  }

  Local<Array> _board = Local<Array>::Cast(args[2]);
  READ_CARD_MASK_WITH_COLLECTOR(_board, board, dead, on_board);
  if (on_board > game->maxboard) TYPE_ERROR("Too many cards on board");

  Local<String> deadStr    = String::NewSymbol("dead");
  Local<String> samplesStr = String::NewSymbol("samples");
  Local<String> equityStr  = String::NewSymbol("equity");
  Local<String> hiStr      = String::NewSymbol("hi");
  Local<String> loStr      = String::NewSymbol("lo");
  Local<String> winStr     = String::NewSymbol("win");
  Local<String> loseStr    = String::NewSymbol("lose");
  Local<String> tieStr     = String::NewSymbol("tie");

  players = args[3]->IntegerValue();
  if (players < 2) TYPE_ERROR("Should be at least 2 players in game");
  if (players > ENUM_MAXPLAYERS) TYPE_ERROR("Too many players in game");

  if (args.Length() > 4 && args[4]->IsObject()) {
    Local<Object> opt = args[4]->ToObject();
    if (opt->HasOwnProperty(deadStr)) {
      Local<Value> d = opt->Get(deadStr);
      if (!d->IsArray()) TYPE_ERROR("Dead cards should be array of cards");
      Local<Array> _dead = Local<Array>::Cast(d);
      READ_CARD_MASK(_dead, dead, are_dead);
    }
  }

  // TODO: find better scheme to calculate number of iterations
  int exhausive = !(on_board < game->maxboard);
  int handSamples = (players - 1) * 1000 * (exhausive ? 30 : 1);
  int boardSamples = 700000 / handSamples;
  int ordering = 0;
  int err = 0;

  enum_result_t result;
  enumResultClear(&result);
  enum_result_t intermediate;

  int _players = players - 1;
  int *_pocket_sizes = poket_sizes + 1;
  StdDeck_CardMask *_pockets = pockets + 1;

  DECK_MONTECARLO_PERMUTATIONS_D(StdDeck, _pockets, _players, _pocket_sizes, dead, handSamples, {
    if (exhausive) {
      err = enumExhaustive(game->game, pockets, board, _used, players, on_board, ordering, &intermediate);
    } else {
      err = enumSample(game->game, pockets, board, _used, players, on_board, boardSamples, ordering, &intermediate);
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


  Local<Object> info = Object::New();

  info->Set(equityStr, Number::New(result.ev[0] / result.nsamples));
  info->Set(samplesStr, Integer::NewFromUnsigned(result.nsamples));

  if (game->hashipot) {
    Local<Object> hi = Object::New();
    info->Set(hiStr, hi);
    hi->Set(winStr, Integer::NewFromUnsigned(result.nwinhi[0]));
    hi->Set(tieStr, Integer::NewFromUnsigned(result.ntiehi[0]));
    hi->Set(loseStr, Integer::NewFromUnsigned(result.nlosehi[0]));
  }
  if (game->haslopot) {
    Local<Object> lo = Object::New();
    info->Set(loStr, lo);
    lo->Set(winStr, Integer::NewFromUnsigned(result.nwinlo[0]));
    lo->Set(tieStr, Integer::NewFromUnsigned(result.ntielo[0]));
    lo->Set(loseStr, Integer::NewFromUnsigned(result.nloselo[0]));
  }

  return scope.Close(info);
}
