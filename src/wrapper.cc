#include <node.h>
#include <v8.h>
#include "poker-eval.h"
#include "helpers.h"

#define ADD_CARD(c) if (index < cards->Length()) cards->Set(index++, Integer::NewFromUnsigned(c))

using namespace v8;

Handle<Value> EvalCards(const Arguments& args)
{
  HandleScope scope;
  Deck_CardMask cards;
  CardMask_RESET(cards);
  uint32_t count = 0;
  int card;
  Local<Value> v;
  EXPAND_ARGS(args, v, {
    String::AsciiValue ascii(v->ToString());
    if (Deck_stringToCard(*ascii, &card) && !CardMask_CARD_IS_SET(cards, card)) {
      CardMask_SET(cards, card);
      ++count;
    }
  });
  HandVal value = Hand_EVAL_N(cards, count);
  return scope.Close(Integer::NewFromUnsigned(value));
}

Handle<Value> HandValType(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsNumber()) {
    return scope.Close(Undefined());
  }
  Local<Integer> v = Local<Integer>::Cast(args[0]);
  return scope.Close(Integer::NewFromUnsigned(HandVal_HANDTYPE(v->Value())));
}

Handle<Value> HandValSigCards(const Arguments& args)
{
  HandleScope scope;
  if (args.Length() != 1 || !args[0]->IsNumber()) {
    return scope.Close(Undefined());
  }
  Local<Integer> v = Local<Integer>::Cast(args[0]);
  HandVal  value = v->Value();
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

void initModule(Handle<Object> target)
{
  HandleScope scope;
  target->Set(String::NewSymbol("evalCards"), FunctionTemplate::New(EvalCards)->GetFunction());
  target->Set(String::NewSymbol("handvalType"), FunctionTemplate::New(HandValType)->GetFunction());
  target->Set(String::NewSymbol("handvalSigCards"), FunctionTemplate::New(HandValSigCards)->GetFunction());
}

NODE_MODULE(wrapper, initModule)
