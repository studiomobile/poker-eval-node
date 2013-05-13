#include <string.h>
#include "game.h"


typedef struct {
  enum_game_t id;
  const char * type;
} known_game_t;

#define KNOWN_GAME(id) {game_##id, #id}

static known_game_t knownGames[] = {
  KNOWN_GAME(holdem),
  KNOWN_GAME(holdem8),
  KNOWN_GAME(omaha),
  KNOWN_GAME(omaha8),
  KNOWN_GAME(7stud),
  KNOWN_GAME(7stud8),
  KNOWN_GAME(7studnsq),
  KNOWN_GAME(razz),
  KNOWN_GAME(5draw),
  KNOWN_GAME(5draw8),
  KNOWN_GAME(5drawnsq),
  KNOWN_GAME(lowball),
  KNOWN_GAME(lowball27)
};

enum_gameparams_t* findGame(const char *gameId)
{
  size_t totalGames = sizeof(knownGames)/sizeof(knownGames[0]);
  for (size_t i = 0; i < totalGames; ++i)
  {
    known_game_t game = knownGames[i];
    if (0 == strcmp(game.type, gameId)) {
      return enumGameParams(game.id);
    }
  }
  return NULL;
}

Handle<Value> KnownGames(const Arguments& args)
{
  HandleScope scope;
  size_t totalGames = sizeof(knownGames)/sizeof(knownGames[0]);

  Handle<String> typeStr    = String::NewSymbol("type");
  Handle<String> nameStr    = String::NewSymbol("name");
  Handle<String> hiStr      = String::NewSymbol("hi");
  Handle<String> loStr      = String::NewSymbol("lo");
  Handle<String> boardStr   = String::NewSymbol("board");
  Handle<String> minHandStr = String::NewSymbol("minHand");
  Handle<String> maxHandStr = String::NewSymbol("maxHand");

  Handle<Array> array = Array::New(totalGames);
  for (size_t i = 0; i < totalGames; ++i)
  {
    known_game_t game = knownGames[i];
    enum_gameparams_t *params = enumGameParams(game.id);

    Handle<Object> info = Object::New();
    array->Set(i, info);

    info->Set(typeStr,    String::New(game.type));
    info->Set(nameStr,    String::New(params->name));
    info->Set(hiStr,      Boolean::New(params->hashipot));
    info->Set(loStr,      Boolean::New(params->haslopot));
    info->Set(boardStr,   Integer::New(params->maxboard));
    info->Set(minHandStr, Integer::New(params->minpocket));
    info->Set(maxHandStr, Integer::New(params->maxpocket));
  }
  return scope.Close(array);
}
