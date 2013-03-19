#include "eval_cards.h"
#include "game_enum.h"

void initModule(Handle<Object> target)
{
  HandleScope scope;
  NODE_SET_METHOD(target, "evalCards", EvalCards);
  NODE_SET_METHOD(target, "handvalType", HandValType);
  NODE_SET_METHOD(target, "handvalSigCards", HandValSigCards);
  NODE_SET_METHOD(target, "knownGames", KnownGames);
  NODE_SET_METHOD(target, "enumGame", EnumGame);
}

NODE_MODULE(wrapper, initModule)
