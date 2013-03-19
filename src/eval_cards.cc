#include "eval_cards.h"
#include "helpers.h"

#define ADD_CARD(c) if (index < cards->Length()) cards->Set(index++, Integer::NewFromUnsigned(c))

Handle<Value> EvalCards(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsArray()) TYPE_ERROR("Expected array of cards as one argument");
  Local<Array> _cards = Local<Array>::Cast(args[0]);
  Deck_CardMask cards;
  CardMask_RESET(cards);
  uint32_t count = 0;
  READ_CARD_MASK(_cards, cards, count);
  return scope.Close(Integer::NewFromUnsigned(Hand_EVAL_N(cards, count)));
}

Handle<Value> HandValType(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsNumber()) TYPE_ERROR("Expected integer as one argument");
  return scope.Close(Integer::NewFromUnsigned(HandVal_HANDTYPE(args[0]->Uint32Value())));
}

Handle<Value> HandValSigCards(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsNumber()) TYPE_ERROR("Expected integer as one argument");
  HandVal  value = args[0]->Int32Value();
  uint32_t htype = HandVal_HANDTYPE(value);
  uint32_t index = 0;
  Local<Array> cards = Array::New(nSigCards[htype]);
  ADD_CARD(HandVal_TOP_CARD(value));
  ADD_CARD(HandVal_SECOND_CARD(value));
  ADD_CARD(HandVal_THIRD_CARD(value));
  ADD_CARD(HandVal_FOURTH_CARD(value));
  ADD_CARD(HandVal_FIFTH_CARD(value));
  return scope.Close(cards);
}
