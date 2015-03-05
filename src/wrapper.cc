#include "game.h"
#include "game_enum.h"
#include "hand_enum.h"
#include "game_eval.h"
#include "helpers.h"
#include <poker-eval/inlines/eval.h>


#define ADD_CARD(c) if (index < _cards->Length()) _cards->Set(index++, NanNew<Integer>(c))

NAN_METHOD(EvalCards)
{
  NanScope();
  if (args.Length() != 1 || !args[0]->IsArray()) TYPE_ERROR("Expected array of cards as one argument");
  Local<Array> _cards = Local<Array>::Cast(args[0]);
  Deck_CardMask cards;
  uint count = 0;
  CardMask_RESET(cards);
  READ_CARD_MASK(_cards, cards, count);
  uint index = 0;
  uint value = Hand_EVAL_N(cards, count);
  uint type  = HandVal_HANDTYPE(value);
  _cards = NanNew<Array>(nSigCards[type]);
  ADD_CARD(HandVal_TOP_CARD(value));
  ADD_CARD(HandVal_SECOND_CARD(value));
  ADD_CARD(HandVal_THIRD_CARD(value));
  ADD_CARD(HandVal_FOURTH_CARD(value));
  ADD_CARD(HandVal_FIFTH_CARD(value));

  Local<Object> result = NanNew<Object>();
  result->Set(NanNew<String>("value"), NanNew<Integer>(value));
  result->Set(NanNew<String>("type"),  NanNew<Integer>(type));
  result->Set(NanNew<String>("sig"),   _cards);

  NanReturnValue(result);
}


#define EXPORT(name, fn) exports->Set(NanNew<String>(name), NanNew<FunctionTemplate>(fn)->GetFunction())

void initModule(Handle<Object> exports)
{
  EXPORT("evalCards", EvalCards);
  EXPORT("knownGames", KnownGames);
  EXPORT("enumGame", EnumGame);
  EXPORT("enumHand", EnumHand);
  EXPORT("evalGame", EvalGame);
}

NODE_MODULE(wrapper, initModule)
