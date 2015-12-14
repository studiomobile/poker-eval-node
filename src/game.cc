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
  Nan::HandleScope scope;
  size_t totalGames = sizeof(knownGames)/sizeof(knownGames[0]);

  Local<String> typeStr    = Nan::New<String>("type").ToLocalChecked();
  Local<String> nameStr    = Nan::New<String>("name").ToLocalChecked();
  Local<String> hiStr      = Nan::New<String>("hi").ToLocalChecked();
  Local<String> loStr      = Nan::New<String>("lo").ToLocalChecked();
  Local<String> boardStr   = Nan::New<String>("board").ToLocalChecked();
  Local<String> minHandStr = Nan::New<String>("minHand").ToLocalChecked();
  Local<String> maxHandStr = Nan::New<String>("maxHand").ToLocalChecked();

  Local<Array> array = Nan::New<Array>(totalGames);
  for (size_t i = 0; i < totalGames; ++i)
  {
    known_game_t game = knownGames[i];
    enum_gameparams_t *params = enumGameParams(game.id);

    Local<Object> info = Nan::New<Object>();
    Nan::Set(array, i, info);

    Nan::Set(info, typeStr,    Nan::New<String>(game.type).ToLocalChecked());
    Nan::Set(info, nameStr,    Nan::New<String>(params->name).ToLocalChecked());
    Nan::Set(info, hiStr,      Nan::New<Boolean>(params->hashipot));
    Nan::Set(info, loStr,      Nan::New<Boolean>(params->haslopot));
    Nan::Set(info, boardStr,   Nan::New<Integer>(params->maxboard));
    Nan::Set(info, minHandStr, Nan::New<Integer>(params->minpocket));
    Nan::Set(info, maxHandStr, Nan::New<Integer>(params->maxpocket));
  }

  info.GetReturnValue().Set(array);
}
