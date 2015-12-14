#include "game.h"
#include "game_enum.h"
#include "hand_enum.h"
#include "game_eval.h"
#include "helpers.h"
#include <poker-eval/inlines/eval.h>


#define ADD_CARD(c) if (index < _cards->Length()) Nan::Set(_cards, index++, Nan::New<Integer>(c))

NAN_METHOD(EvalCards)
{
  Nan::HandleScope scope;
  if (info.Length() != 1 || !info[0]->IsArray()) TYPE_ERROR("Expected array of cards as one argument");
  Local<Array> _cards = Local<Array>::Cast(info[0]);
  Deck_CardMask cards;
  uint count = 0;
  CardMask_RESET(cards);
  READ_CARD_MASK(_cards, cards, count);
  uint index = 0;
  uint value = Hand_EVAL_N(cards, count);
  uint type  = HandVal_HANDTYPE(value);
  _cards = Nan::New<Array>(nSigCards[type]);
  ADD_CARD(HandVal_TOP_CARD(value));
  ADD_CARD(HandVal_SECOND_CARD(value));
  ADD_CARD(HandVal_THIRD_CARD(value));
  ADD_CARD(HandVal_FOURTH_CARD(value));
  ADD_CARD(HandVal_FIFTH_CARD(value));

  Local<Object> result = Nan::New<Object>();
  Nan::Set(result, Nan::New<String>("value").ToLocalChecked(), Nan::New<Integer>(value));
  Nan::Set(result, Nan::New<String>("type").ToLocalChecked(),  Nan::New<Integer>(type));
  Nan::Set(result, Nan::New<String>("sig").ToLocalChecked(),   _cards);

  info.GetReturnValue().Set(result);
}


#define EXPORT(name, fn) \
  Nan::Set(target, Nan::New(name).ToLocalChecked(), \
                   Nan::GetFunction(Nan::New<FunctionTemplate>(fn)).ToLocalChecked())

NAN_MODULE_INIT(initModule)
{
  EXPORT("evalCards", EvalCards);
  EXPORT("knownGames", KnownGames);
  EXPORT("enumGame", EnumGame);
  EXPORT("enumHand", EnumHand);
  EXPORT("evalGame", EvalGame);
}

NODE_MODULE(wrapper, initModule)
