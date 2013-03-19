#include "game_enum.h"
#include "helpers.h"

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

  Handle<Array> array = Array::New(totalGames);
  if (array.IsEmpty()) {
    return scope.Close(Undefined());
  }

  Handle<String> typeStr    = String::NewSymbol("type");
  Handle<String> nameStr    = String::NewSymbol("name");
  Handle<String> hiStr      = String::NewSymbol("hi");
  Handle<String> loStr      = String::NewSymbol("lo");
  Handle<String> boardStr   = String::NewSymbol("board");
  Handle<String> minHandStr = String::NewSymbol("minHand");
  Handle<String> maxHandStr = String::NewSymbol("maxHand");

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


Handle<Value> EnumGame(const Arguments& args)
{
  HandleScope scope;

  if (args.Length() < 1 || !args[0]->IsString()) TYPE_ERROR("Please provide game type as first argument");
  if (args.Length() < 2 || !args[1]->IsArray())  TYPE_ERROR("Please provide player pockets as second argument");
  if (args.Length() < 3 || !args[2]->IsArray())  TYPE_ERROR("Please provide board cards as third argument");

  String::AsciiValue gameTypeStr(args[0]);
  enum_gameparams_t *game = findGame(*gameTypeStr);
  if (!game) TYPE_ERROR("Game type is invalid");

  StdDeck_CardMask dead;
  StdDeck_CardMask board;
  StdDeck_CardMask pockets[ENUM_MAXPLAYERS];
  int err = 0;
  int samples = 0;
  int ordering = 0; //TODO: add ordering option and results
  int on_board = 0;
  int _count = 0;
  enum_result_t result;

  enumResultClear(&result);
  StdDeck_CardMask_RESET(dead);
  StdDeck_CardMask_RESET(board);
  for (uint i = 0; i < ENUM_MAXPLAYERS; ++i)
    StdDeck_CardMask_RESET(pockets[i]);

  Local<String> deadStr    = String::NewSymbol("dead");
  Local<String> samplesStr = String::NewSymbol("samples");
  Local<String> equityStr  = String::NewSymbol("equity");
  Local<String> hiStr      = String::NewSymbol("hi");
  Local<String> loStr      = String::NewSymbol("lo");
  Local<String> winStr     = String::NewSymbol("win");
  Local<String> loseStr    = String::NewSymbol("lose");
  Local<String> tieStr     = String::NewSymbol("tie");

  if (args.Length() > 3 && args[3]->IsObject()) {
    Local<Object> opt = args[3]->ToObject();
    if (opt->HasOwnProperty(deadStr)) {
      Local<Value> d = opt->Get(deadStr);
      if (!d->IsArray()) TYPE_ERROR("Dead cards should be array of cards");
      Local<Array> _dead = Local<Array>::Cast(d);
      READ_CARD_MASK(_dead, dead, _count);
    }
    if (opt->HasOwnProperty(samplesStr)) {
      Local<Value> i = opt->Get(samplesStr);
      if (!i->IsNumber()) TYPE_ERROR("Number of samples is not a number");
      samples = i->IntegerValue();
      if (samples <= 0) TYPE_ERROR("Number of samples should be positite");
    }
  }

  Local<Array> _pockets = Local<Array>::Cast(args[1]);
  if (_pockets->Length() > ENUM_MAXPLAYERS) TYPE_ERROR("Too many players");
  for (uint32_t i = 0; i < _pockets->Length(); ++i)
  {
    Local<Value> item = _pockets->Get(i);
    if (!item->IsArray()) TYPE_ERROR("Player pocket should be array of cards");
    Local<Array> pocket = Local<Array>::Cast(item);
    READ_CARD_MASK_DEAD(pocket, pockets[i], dead, _count);
  }

  Local<Array> _board = Local<Array>::Cast(args[2]);
  READ_CARD_MASK_DEAD(_board, board, dead, on_board);
  if (on_board > game->maxboard) TYPE_ERROR("Too many cards on board");

  if (samples > 0) {
    err = enumSample(game->game, pockets, board, dead, _pockets->Length(), on_board, samples, ordering, &result);
  } else {
    err = enumExhaustive(game->game, pockets, board, dead, _pockets->Length(), on_board, ordering, &result);
  }
  if (err) TYPE_ERROR("Enumeration failed");

  Local<Array> results = Array::New(result.nplayers);
  Local<Integer> samplesNum = Integer::NewFromUnsigned(result.nsamples);

  for (uint i = 0; i < result.nplayers; ++i)
  {
    Local<Object> info = Object::New();
    results->Set(i, info);

    info->Set(equityStr, Number::New(result.ev[i] / result.nsamples));
    if (game->hashipot) {
      Local<Object> hi = Object::New();
      info->Set(hiStr, hi);
      hi->Set(winStr, Integer::NewFromUnsigned(result.nwinhi[i]));
      hi->Set(tieStr, Integer::NewFromUnsigned(result.ntiehi[i]));
      hi->Set(loseStr, Integer::NewFromUnsigned(result.nlosehi[i]));
    }
    if (game->haslopot) {
      Local<Object> lo = Object::New();
      info->Set(loStr, lo);
      lo->Set(winStr, Integer::NewFromUnsigned(result.nwinlo[i]));
      lo->Set(tieStr, Integer::NewFromUnsigned(result.ntielo[i]));
      lo->Set(loseStr, Integer::NewFromUnsigned(result.nloselo[i]));
    }
    info->Set(samplesStr, samplesNum);
  }

  return scope.Close(results);
}
