var _    = require('underscore')
  , lib  = require('./index')


var deck  = _.shuffle(lib.Card.DECK)
  , type = 'holdem'
  , nplayers = 7
  , inHand = 2
  , onBoard = 5
  , total = 5
  , board = deck.splice(0, onBoard)
  , players = []
  , samples = 100000


for (var i = 0; i < nplayers; i++) {
  players.push(deck.splice(0, inHand));
}

// console.log(lib.knownGames());

var result = lib.enumGame(type, players, board, {samples:samples});

for (var i = result.length - 1; i >= 0; i--) {
  var hand = new lib.Hand(players[i], board);
  hand.type = lib.Hand.Types[hand.type];
  hand.cards = players[i].join(', ');
  result[i].hand  = hand;
};

result.unshift({board:board.join(', ')});
console.log(result);

board.push.apply(board, deck.splice(0, total - onBoard));
result = lib.evalGame(type, players, board);

function fillInfo(info) {
  if (!info) return;
  if (info.type != null) info.type = lib.Hand.Types[info.type]
  if (info.sig)  info.sig  = info.sig.map(function(rank) { return lib.Card.NRANKS[rank]; }).join('');
}

for (var i = result.length - 1; i >= 0; i--) {
  result[i].cards = players[i].join(', ');
  fillInfo(result[i].hi);
  fillInfo(result[i].lo);
};

result.unshift({board:board.join(', ')});
console.log(result);

result = []
for (var i = 2; i <= 5; ++i) {
  b = i == 2 ? [] : board.slice(0, i);
  res = lib.enumHand(type, players[0], b, players.length);
  res.hand  = players[0].join(', ');
  res.board = b;
  result.push(res);
}
console.log(result);
