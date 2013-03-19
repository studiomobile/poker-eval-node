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
#define EVAL_GAME(game, code) case game_##game: { code; } break;
#define END_EVAL() case game_NUMGAMES: break; /*default: error = 1; break;*/ }

#define ADD_CARD(c) if (index < sig->Length()) sig->Set(index++, Integer::NewFromUnsigned(c))
#define LOW_CARD_RANK(c) ((c) == StdDeck_Rank_2 ? StdDeck_Rank_ACE : (c)-1)


Handle<Value> EvalGame(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (args.Length() < 2 || !args[1]->IsArray())  TYPE_ERROR("Please provide array of players cards as second argument");

  String::AsciiValue gameTypeStr(args[0]);
  enum_gameparams_t* gameInfo = findGame(*gameTypeStr);
  if (!gameInfo) TYPE_ERROR("Game type is invalid");

  StdDeck_CardMask board;
  StdDeck_CardMask_RESET(board);
  uint onBoard = 0;

  if (gameInfo->maxboard > 0) {
    if (args.Length() < 3 || !args[2]->IsArray())  TYPE_ERROR("Please provide array of cards on board as third argument");
    Local<Array> _board = Local<Array>::Cast(args[2]);
    if (_board->Length() != (uint)gameInfo->maxboard) TYPE_ERROR("Wrong number of cards on board");
    READ_CARD_MASK(_board, board, onBoard);
  }

  Local<Array> hands = Local<Array>::Cast(args[1]);
  if (hands->Length() < 1) TYPE_ERROR("Please provide at least one player hand");

  Local<String> hiStr   = String::NewSymbol("hi");
  Local<String> loStr   = String::NewSymbol("lo");
  Local<String> valStr  = String::NewSymbol("value");
  Local<String> typeStr = String::NewSymbol("type");
  Local<String> sigStr  = String::NewSymbol("sig");

  Local<Array> results = Array::New(hands->Length());
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

    Local<Object> result = Object::New();

    if (gameInfo->hashipot) {
      Local<Object> hi = Object::New();
      result->Set(hiStr, hi);
      uint htype = HandVal_HANDTYPE(hiVal);
      uint index = 0;
      Local<Array> sig = Array::New(StdRules_nSigCards[htype]);
      ADD_CARD(HandVal_TOP_CARD(hiVal));
      ADD_CARD(HandVal_SECOND_CARD(hiVal));
      ADD_CARD(HandVal_THIRD_CARD(hiVal));
      ADD_CARD(HandVal_FOURTH_CARD(hiVal));
      ADD_CARD(HandVal_FIFTH_CARD(hiVal));
      hi->Set(valStr,  Integer::NewFromUnsigned(hiVal));
      hi->Set(typeStr, Integer::NewFromUnsigned(htype));
      hi->Set(sigStr,  sig);
    }

    if (gameInfo->haslopot) {
      Local<Object> lo = Object::New();
      result->Set(loStr, lo);
      if (loVal != LowHandVal_NOTHING) {
        uint index = 0;
        Local<Array> sig = Array::New(StdRules_nSigCards[HandVal_HANDTYPE(loVal)]);
        ADD_CARD(LOW_CARD_RANK(HandVal_TOP_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_SECOND_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_THIRD_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_FOURTH_CARD(loVal)));
        ADD_CARD(LOW_CARD_RANK(HandVal_FIFTH_CARD(loVal)));
        lo->Set(valStr, Integer::NewFromUnsigned(loVal));
        lo->Set(sigStr, sig);
      } else {
        lo->Set(valStr, Integer::NewFromUnsigned(0));
      }
    }

    results->Set(i, result);
  }

  return scope.Close(results);
}
