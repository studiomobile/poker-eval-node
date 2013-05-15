#include "game.h"
#include "game_enum.h"
#include "hand_enum.h"
#include "game_eval.h"
#include "helpers.h"
#include <poker-eval/inlines/eval.h>


#define ADD_CARD(c) if (index < _cards->Length()) _cards->Set(index++, Integer::NewFromUnsigned(c))

Handle<Value> EvalCards(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsArray()) TYPE_ERROR("Expected array of cards as one argument");
  Local<Array> _cards = Local<Array>::Cast(args[0]);
  Deck_CardMask cards;
  uint count = 0;
  CardMask_RESET(cards);
  READ_CARD_MASK(_cards, cards, count);
  uint index = 0;
  uint value = Hand_EVAL_N(cards, count);
  uint type  = HandVal_HANDTYPE(value);
  _cards = Array::New(nSigCards[type]);
  ADD_CARD(HandVal_TOP_CARD(value));
  ADD_CARD(HandVal_SECOND_CARD(value));
  ADD_CARD(HandVal_THIRD_CARD(value));
  ADD_CARD(HandVal_FOURTH_CARD(value));
  ADD_CARD(HandVal_FIFTH_CARD(value));

  Local<Object> result = Object::New();
  result->Set(String::NewSymbol("value"), Integer::NewFromUnsigned(value));
  result->Set(String::NewSymbol("type"),  Integer::NewFromUnsigned(type));
  result->Set(String::NewSymbol("sig"),   _cards);
  return scope.Close(result);
}


void initModule(Handle<Object> target)
{
  HandleScope scope;
  NODE_SET_METHOD(target, "evalCards", EvalCards);
  NODE_SET_METHOD(target, "knownGames", KnownGames);
  NODE_SET_METHOD(target, "enumGame", EnumGame);
  NODE_SET_METHOD(target, "enumHand", EnumHand);
  NODE_SET_METHOD(target, "evalGame", EvalGame);
}

NODE_MODULE(wrapper, initModule)
