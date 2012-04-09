#include "hand.h"
#include "helpers.h"

#define CLASS_NAME "Hand"

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> Hand::ctor;

void Hand::Init(Handle<Object> module)
{
  HandleScope scope;
  Local<String> className = String::NewSymbol(CLASS_NAME);
  if (ctor.IsEmpty()) {
    // Constructor template
    ctor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
    ctor->SetClassName(className);
    // Prototype
    Local<ObjectTemplate> proto = ctor->PrototypeTemplate();
    proto->Set(String::NewSymbol("append"), FunctionTemplate::New(Append)->GetFunction());
    proto->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
    proto->Set(String::NewSymbol("reset"), FunctionTemplate::New(Reset)->GetFunction());
    // Properties
    Local<ObjectTemplate> inst = ctor->InstanceTemplate();
    inst->SetInternalFieldCount(1);
    inst->SetAccessor(String::NewSymbol("Length"), Length);
    inst->SetAccessor(String::NewSymbol("Value"), HandValue);
  }
  module->Set(className, ctor->GetFunction());
}

bool Hand::IsInstance(Handle<Value> obj)
{
  return !ctor.IsEmpty() && ctor->HasInstance(obj);
}

Hand::Hand() : count(0), value(HandVal_NOTHING) { CardMask_RESET(cards); }
Hand::~Hand() {}

Handle<Value> Hand::New(const Arguments& args)
{
  HandleScope scope;
  Local<Object> self = args.This();
  if (!Hand::IsInstance(self)) { // call without new
    int argc = args.Length();
    Local<Value> argv[argc];
    for (size_t i = 0; i < argc; ++i) {
      argv[i] = args[i];
    }
    return ctor->GetFunction()->NewInstance(argc, argv);
  }
  Hand *hand = new Hand();
  hand->Wrap(self);
  Append(args);
  return self;
}

Handle<Value> Hand::Append(const Arguments& args)
{
  HandleScope scope;
  Hand *hand = ObjectWrap::Unwrap<Hand>(args.This());
  if (hand) {
    int card;
    Local<Value> v;
    EXPAND_ARGS(args, v, {
      String::AsciiValue ascii(v->ToString());
      if (Deck_stringToCard(*ascii, &card) && !CardMask_CARD_IS_SET(hand->cards, card)) {
        CardMask_SET(hand->cards, card);
        hand->count++;
        hand->value = HandVal_NOTHING;
      }
    });
  }
  return args.This();
}

Handle<Value> Hand::Remove(const Arguments& args)
{
  HandleScope scope;
  Hand *hand = ObjectWrap::Unwrap<Hand>(args.This());
  if (hand) {
    for (int i = 0; i < args.Length(); ++i)
    {
      int card;
      String::AsciiValue ascii(args[i]->ToString());
      if (Deck_stringToCard(*ascii, &card) && CardMask_CARD_IS_SET(hand->cards, card)) {
        CardMask_UNSET(hand->cards, card);
        hand->count--;
        hand->value = HandVal_NOTHING;
      }
    }
  }
  return args.This();
}

Handle<Value> Hand::Reset(const Arguments& args)
{
  HandleScope scope;
  Hand *hand = ObjectWrap::Unwrap<Hand>(args.This());
  if (hand) {
    CardMask_RESET(hand->cards);
    hand->count = 0;
    hand->value = HandVal_NOTHING;
  }
  return args.This();
}

Handle<Value> Hand::Length(Local<String> property, const AccessorInfo& info)
{
  HandleScope scope;
  Hand *hand = ObjectWrap::Unwrap<Hand>(info.This());
  return hand ? Integer::NewFromUnsigned(hand->count) : Undefined();
}

Handle<Value> Hand::HandValue(Local<String> property, const AccessorInfo& info)
{
  HandleScope scope;
  Hand *hand = ObjectWrap::Unwrap<Hand>(info.This());
  if (!hand) {
    return Integer::NewFromUnsigned(HandVal_NOTHING);
  }
  if (HandVal_NOTHING == hand->value) {
    hand->value = Hand_EVAL_N(hand->cards, hand->count);
  }
  return Integer::NewFromUnsigned(hand->value);
}
