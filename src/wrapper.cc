#include <node.h>
#include <v8.h>
#include "poker-eval.h"
#include "hand.h"
#include "helpers.h"

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

void initModule(Handle<Object> target)
{
  HandleScope scope;
  Hand::Init(target);
  target->Set(String::NewSymbol("evalCards"), FunctionTemplate::New(EvalCards)->GetFunction());
}

NODE_MODULE(wrapper, initModule)
