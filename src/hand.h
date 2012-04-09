#include <node.h>
#include <v8.h>
#include "poker-eval.h"

class Hand : public node::ObjectWrap
{
public:
  static void Init(v8::Handle<v8::Object> module);

  static bool IsInstance(v8::Handle<v8::Value> obj);

private:
  Hand();
  ~Hand();

  static v8::Persistent<v8::FunctionTemplate> ctor;
  static v8::Handle<v8::Value> New(const v8::Arguments& args);
  static v8::Handle<v8::Value> Append(const v8::Arguments& args);
  static v8::Handle<v8::Value> Remove(const v8::Arguments& args);
  static v8::Handle<v8::Value> Reset(const v8::Arguments& args);
  static v8::Handle<v8::Value> Length(v8::Local<v8::String> property, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> HandValue(v8::Local<v8::String> property, const v8::AccessorInfo& info);

  Deck_CardMask cards;
  uint32_t      count;
  HandVal       value;
};
