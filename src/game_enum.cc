#include "game_enum.h"
#include "game.h"
#include "helpers.h"
#include "enum.h"

NAN_METHOD(EnumGame)
{
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (info.Length() < 2 || !info[1]->IsArray())  TYPE_ERROR("Please provide player pockets as second argument");
  if (info.Length() < 3 || !info[2]->IsArray())  TYPE_ERROR("Please provide board cards as third argument");

  Nan::Utf8String gameTypeStr(info[0]);
  enum_gameparams_t *game = findGame(*gameTypeStr);
  if (!game) TYPE_ERROR("Game type is invalid");
  enum_game_t game_type = game->game;

  StdDeck_CardMask dead;
  StdDeck_CardMask board;
  StdDeck_CardMask pockets[ENUM_MAXPLAYERS];
  int err = 0;
  int samples = 0;
  int ordering = 0; //TODO: add ordering option and results
  int on_board = 0;
  int _count = 0;
  enum_result_t enumResult;

  enumResultClear(&enumResult);
  StdDeck_CardMask_RESET(dead);
  StdDeck_CardMask_RESET(board);
  for (uint i = 0; i < ENUM_MAXPLAYERS; ++i)
    StdDeck_CardMask_RESET(pockets[i]);

  Local<String> deadStr    = Nan::New<String>("dead").ToLocalChecked();
  Local<String> samplesStr = Nan::New<String>("samples").ToLocalChecked();
  Local<String> equityStr  = Nan::New<String>("equity").ToLocalChecked();
  Local<String> hiStr      = Nan::New<String>("hi").ToLocalChecked();
  Local<String> loStr      = Nan::New<String>("lo").ToLocalChecked();
  Local<String> winStr     = Nan::New<String>("win").ToLocalChecked();
  Local<String> loseStr    = Nan::New<String>("lose").ToLocalChecked();
  Local<String> tieStr     = Nan::New<String>("tie").ToLocalChecked();

  if (info.Length() > 3 && info[3]->IsObject()) {
    Local<Object> opt = info[3]->ToObject();
    if (opt->HasOwnProperty(deadStr)) {
      Local<Value> d = opt->Get(deadStr);
      if (!d->IsArray()) TYPE_ERROR("Dead cards should be array of cards");
      Local<Array> _dead = Local<Array>::Cast(d);
      READ_CARD_MASK(_dead, dead, _count);
    }
    if (opt->HasOwnProperty(samplesStr)) {
      Local<Value> i = opt->Get(samplesStr);
      if (!i->IsNumber()) TYPE_ERROR("Number of samples is not a number");
      samples = i->IntegerValue();
      if (samples <= 0) TYPE_ERROR("Number of samples should be positite");
    }
  }

  Local<Array> _pockets = Local<Array>::Cast(info[1]);
  if (_pockets->Length() > ENUM_MAXPLAYERS) TYPE_ERROR("Too many players");
  for (uint32_t i = 0; i < _pockets->Length(); ++i)
  {
    Local<Value> item = _pockets->Get(i);
    if (!item->IsArray()) TYPE_ERROR("Player pocket should be array of cards");
    Local<Array> pocket = Local<Array>::Cast(item);
    READ_CARD_MASK_WITH_COLLECTOR(pocket, pockets[i], dead, _count);
  }

  Local<Array> _board = Local<Array>::Cast(info[2]);
  READ_CARD_MASK_WITH_COLLECTOR(_board, board, dead, on_board);
  if (on_board > game->maxboard) TYPE_ERROR("Too many cards on board");

  if (samples > 0 && on_board < game->maxboard) {
    err = stdEnumSample(game_type, pockets, board, dead, _pockets->Length(), on_board, samples, ordering, &enumResult);
  } else {
    err = stdEnumExhaustive(game_type, pockets, board, dead, _pockets->Length(), on_board, ordering, &enumResult);
  }
  if (err) TYPE_ERROR("Enumeration failed");

  Local<Array> results = Nan::New<Array>(enumResult.nplayers);
  Local<Integer> samplesNum = Nan::New<Integer>(enumResult.nsamples);

  for (uint i = 0; i < enumResult.nplayers; ++i)
  {
    Local<Object> result = Nan::New<Object>();
    Nan::Set(results, i, result);

    Nan::Set(result, equityStr, Nan::New<Number>(enumResult.ev[i] / enumResult.nsamples));
    if (game->hashipot) {
      Local<Object> hi = Nan::New<Object>();
      Nan::Set(result, hiStr, hi);
      Nan::Set(hi, winStr, Nan::New<Integer>(enumResult.nwinhi[i]));
      Nan::Set(hi, tieStr, Nan::New<Integer>(enumResult.ntiehi[i]));
      Nan::Set(hi, loseStr, Nan::New<Integer>(enumResult.nlosehi[i]));
    }
    if (game->haslopot) {
      Local<Object> lo = Nan::New<Object>();
      Nan::Set(result, loStr, lo);
      Nan::Set(lo, winStr, Nan::New<Integer>(enumResult.nwinlo[i]));
      Nan::Set(lo, tieStr, Nan::New<Integer>(enumResult.ntielo[i]));
      Nan::Set(lo, loseStr, Nan::New<Integer>(enumResult.nloselo[i]));
    }
    Nan::Set(result, samplesStr, samplesNum);
  }

  info.GetReturnValue().Set(results);
}
