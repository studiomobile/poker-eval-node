#ifndef __GAME_H__
#define __GAME_H__

#include "includes.h"

enum_gameparams_t* findGame(const char *gameId);

Handle<Value> KnownGames(const Arguments& args);

#endif // __GAME_H__
