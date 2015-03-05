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

NAN_METHOD(KnownGames)
{
  NanScope();
  size_t totalGames = sizeof(knownGames)/sizeof(knownGames[0]);

  Handle<String> typeStr    = NanNew<String>("type");
  Handle<String> nameStr    = NanNew<String>("name");
  Handle<String> hiStr      = NanNew<String>("hi");
  Handle<String> loStr      = NanNew<String>("lo");
  Handle<String> boardStr   = NanNew<String>("board");
  Handle<String> minHandStr = NanNew<String>("minHand");
  Handle<String> maxHandStr = NanNew<String>("maxHand");

  Handle<Array> array = NanNew<Array>(totalGames);
  for (size_t i = 0; i < totalGames; ++i)
  {
    known_game_t game = knownGames[i];
    enum_gameparams_t *params = enumGameParams(game.id);

    Handle<Object> info = NanNew<Object>();
    array->Set(i, info);

    info->Set(typeStr,    NanNew<String>(game.type));
    info->Set(nameStr,    NanNew<String>(params->name));
    info->Set(hiStr,      NanNew<Boolean>(params->hashipot));
    info->Set(loStr,      NanNew<Boolean>(params->haslopot));
    info->Set(boardStr,   NanNew<Integer>(params->maxboard));
    info->Set(minHandStr, NanNew<Integer>(params->minpocket));
    info->Set(maxHandStr, NanNew<Integer>(params->maxpocket));
  }

  NanReturnValue(array);
}
