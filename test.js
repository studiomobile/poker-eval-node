var _    = require('underscore')
  , lib  = require('./index')


var deck  = _.shuffle(lib.Card.DECK)
  , nplayers = 5
  , inHand = 2
  , onBoard = 3
  , dead = 0
  , board = deck.splice(0, onBoard)
  , dead = deck.splice(0, dead)
  , players = []
  , samples = 100000


for (var i = 0; i < nplayers; i++) {
  players.push(deck.splice(0, inHand));
}

// console.log(lib.knownGames());

var result = lib.enumGame("holdem", players, board, {dead:dead, samples:samples});

for (var i = result.length - 1; i >= 0; i--) {
  var hand = new lib.Hand(players[i], board);
  hand.type = lib.Hand.Types[hand.type];
  result[i].cards = players[i];
  result[i].hand  = hand;
};

console.log({board:board, dead:dead});
console.log(result);
