var _    = require('underscore')
  , lib  = require('./index')

function newDeck() {
  return _.shuffle(lib.Card.DECK);
}

function testEnumGame(type, deck, nplayers, inHand, onBoard, samples) {
  var players = []
    , board = deck.splice(0, onBoard)
  for (var i = 0; i < nplayers; i++) {
    players.push(deck.splice(0, inHand));
  }
  if (samples == null) samples = 100000;
  var result = lib.enumGame(type, players, board, {samples:samples});
  for (var i = result.length - 1; i >= 0; i--) {
    var hand = new lib.Hand(players[i], board);
    hand.type = lib.Hand.Types[hand.type];
    hand.cards = players[i].join(', ');
    result[i].hand = hand;
  };
  result.unshift({board:board.join(', ')});
  return result;
}

function testEvalGame(type, deck, nplayers, inHand, onBoard) {
  function fillInfo(info) {
    if (!info) return;
    if (info.type != null) info.type = lib.Hand.Types[info.type]
    if (info.sig) info.sig = info.sig.map(function(rank) { return lib.Card.NRANKS[rank]; }).join('');
  }
  var players = []
    , board = deck.splice(0, onBoard)
  for (var i = 0; i < nplayers; i++) {
    players.push(deck.splice(0, inHand));
  }
  result = lib.evalGame(type, players, board);
  for (var i = result.length - 1; i >= 0; i--) {
    result[i].cards = players[i].join(', ');
    fillInfo(result[i].hi);
    fillInfo(result[i].lo);
  };
  result.unshift({board:board.join(', ')});
  return result;
}


function testEnumHand(type, deck, nplayers, inHand, onBoard) {
  var result = []
    , board = deck.splice(0, onBoard)
    , hand = deck.splice(0, inHand)
    , res = lib.enumHand(type, hand, board, nplayers)
  var h = new lib.Hand(hand, board);
  h.type = lib.Hand.Types[h.type];
  h.cards = hand.join(', ');
  res.hand  = h;
  res.board = board.join(', ');
  res.players = nplayers;
  return res;
}

console.log(testEnumHand('5draw', newDeck(), Number(process.argv[2] || 2), 5, 0));
// console.log(testEnumGame('5draw', newDeck(), Number(process.argv[2] || 2), 5, 0));
