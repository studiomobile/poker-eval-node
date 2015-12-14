#include "game_eval.h"
#include "game.h"
#include "helpers.h"

#include <poker-eval/inlines/eval.h>
#include <poker-eval/inlines/eval_low.h>
#include <poker-eval/inlines/eval_low8.h>
#include <poker-eval/inlines/eval_low27.h>
#include <poker-eval/inlines/eval_joker_low.h>
#include <poker-eval/inlines/eval_omaha.h>

#define BEGIN_EVAL(game) switch (game) {
#define EVAL_GAME(game, code) case game_##game: { code; } break
#define END_EVAL() default: error = 1; break; }

#define ADD_CARD(c) if (index < sig->Length()) Nan::Set(sig, index++, Nan::New<Integer>(c))
#define LOW_CARD_RANK(c) ((c) == StdDeck_Rank_2 ? StdDeck_Rank_ACE : (c)-1)


NAN_METHOD(EvalGame)
{
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (info.Length() < 2 || !info[1]->IsArray())  TYPE_ERROR("Please provide array of players cards as second argument");

  Nan::Utf8String gameTypeStr(info[0]);
  enum_gameparams_t* gameInfo = findGame(*gameTypeStr);
  if (!gameInfo) TYPE_ERROR("Game type is invalid");

  StdDeck_CardMask board;
  StdDeck_CardMask_RESET(board);
  uint onBoard = 0;

  if (gameInfo->maxboard > 0) {
    if (info.Length() < 3 || !info[2]->IsArray()) TYPE_ERROR("Please provide array of cards on board as third argument");
    Local<Array> _board = Local<Array>::Cast(info[2]);
    if (_board->Length() > (uint)gameInfo->maxboard) TYPE_ERROR("Wrong number of cards on board");
    READ_CARD_MASK(_board, board, onBoard);
  }

  Local<Array> hands = Local<Array>::Cast(info[1]);
  if (hands->Length() < 1) TYPE_ERROR("Please provide at least one player hand");

  Local<String> hiStr   = Nan::New<String>("hi").ToLocalChecked();
  Local<String> loStr   = Nan::New<String>("lo").ToLocalChecked();
  Local<String> valStr  = Nan::New<String>("value").ToLocalChecked();
  Local<String> typeStr = Nan::New<String>("type").ToLocalChecked();
  Local<String> sigStr  = Nan::New<String>("sig").ToLocalChecked();

  Local<Array> results = Nan::New<Array>(hands->Length());
  for (uint32_t i = 0; i < hands->Length(); ++i)
  {
    Local<Value> item = hands->Get(i);
    if (!item->IsArray()) TYPE_ERROR("Players hand should be array of cards");
    Local<Array> _hand = Local<Array>::Cast(item);

    StdDeck_CardMask hand;
    StdDeck_CardMask cards;
    StdDeck_CardMask_RESET(hand);
    StdDeck_CardMask_RESET(cards);
    StdDeck_CardMask_OR(cards, hand, board);
    uint count = onBoard;

    if (_hand->Length() < (uint)gameInfo->minpocket
     || _hand->Length() > (uint)gameInfo->maxpocket) TYPE_ERROR("Wrong number of cards in hand");
    READ_CARD_MASK_WITH_COLLECTOR(_hand, hand, cards, count);

    HandVal    hiVal = HandVal_NOTHING;
    LowHandVal loVal = LowHandVal_NOTHING;
    int error = 0;

    BEGIN_EVAL(gameInfo->game)
    EVAL_GAME(omaha,     error = StdDeck_OmahaHiLow8_EVAL(hand, board, &hiVal, NULL));
    EVAL_GAME(omaha8,    error = StdDeck_OmahaHiLow8_EVAL(hand, board, &hiVal, &loVal));
    EVAL_GAME(holdem,    hiVal = StdDeck_StdRules_EVAL_N(cards, count));
    EVAL_GAME(holdem8,   hiVal = StdDeck_StdRules_EVAL_N(cards, count); loVal = StdDeck_Lowball8_EVAL(cards, count));
    EVAL_GAME(7stud,     hiVal = StdDeck_StdRules_EVAL_N(cards, count));
    EVAL_GAME(7stud8,    hiVal = StdDeck_StdRules_EVAL_N(cards, count); loVal = StdDeck_Lowball8_EVAL(cards, count));
    EVAL_GAME(7studnsq,  hiVal = StdDeck_StdRules_EVAL_N(cards, count); loVal = StdDeck_Lowball_EVAL(cards, count));
    EVAL_GAME(razz,      loVal = StdDeck_Lowball_EVAL(cards, count));
    EVAL_GAME(lowball27, loVal = StdDeck_Lowball27_EVAL_N(cards, count));
    //NOTICE: joker decks are not supported, cards will be avaluated using standard deck
    EVAL_GAME(5draw,     hiVal = StdDeck_StdRules_EVAL_N(cards, count));
    EVAL_GAME(5draw8,    hiVal = StdDeck_StdRules_EVAL_N(cards, count); loVal = StdDeck_Lowball8_EVAL(cards, count));
    EVAL_GAME(5drawnsq,  hiVal = StdDeck_StdRules_EVAL_N(cards, count); loVal = StdDeck_Lowball_EVAL(cards, count));
    EVAL_GAME(lowball,   loVal = StdDeck_Lowball_EVAL(cards, count));
    END_EVAL()

    if (error) TYPE_ERROR("Hand evaluation failed");

    Local<Object> result = Nan::New<Object>();

    if (gameInfo->hashipot) {
      Local<Object> hi = Nan::New<Object>();
      Nan::Set(result, hiStr, hi);
      uint htype = HandVal_HANDTYPE(hiVal);
      uint index = 0;
      Local<Array> sig = Nan::New<Array>(StdRules_nSigCards[htype]);
      ADD_CARD(HandVal_TOP_CARD(hiVal));
      ADD_CARD(HandVal_SECOND_CARD(hiVal));
      ADD_CARD(HandVal_THIRD_CARD(hiVal));
      ADD_CARD(HandVal_FOURTH_CARD(hiVal));
      ADD_CARD(HandVal_FIFTH_CARD(hiVal));
      Nan::Set(hi, valStr,  Nan::New<Integer>(hiVal));
      Nan::Set(hi, typeStr, Nan::New<Integer>(htype));
      Nan::Set(hi, sigStr,  sig);
    }

    if (gameInfo->haslopot) {
      Local<Object> lo = Nan::New<Object>();
      Nan::Set(result, loStr, lo);
      if (loVal != LowHandVal_NOTHING) {
        uint index = 0;
        Local<Array> sig = Nan::New<Array>(StdRules_nSigCards[HandVal_HANDTYPE(loVal)]);
        ADD_CARD(LOW_CARD_RANK(HandVal_TOP_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_SECOND_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_THIRD_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_FOURTH_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_FIFTH_CARD(loVal)));
        Nan::Set(lo, valStr, Nan::New<Integer>(loVal));
        Nan::Set(lo, sigStr, sig);
      } else {
        Nan::Set(lo, valStr, Nan::New<Integer>(0));
      }
    }

    Nan::Set(results, i, result);
  }

  info.GetReturnValue().Set(results);
}
