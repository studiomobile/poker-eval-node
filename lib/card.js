var Card = module.exports = {};

Card.TWO   = 0
Card.THREE = 1
Card.FOUR  = 2
Card.FIVE  = 3
Card.SIX   = 4
Card.SEVEN = 5
Card.EIGHT = 6
Card.NINE  = 7
Card.TEN   = 8
Card.JACK  = 9
Card.QUEEN = 10
Card.KING  = 11
Card.ACE   = 12

Card.NRANKS = [ 2, 3, 4, 5, 6, 7, 8, 9, 'T', 'J', 'Q', 'K', 'A' ]
Card.RANKS = {}

Card.HEARTS   = 0
Card.DIAMONDS = 1
Card.CLUBS    = 2
Card.SPADES   = 3

Card.NSUITS = [ 'H', 'D', 'C', 'S' ]
Card.SUITS = {}

Card.DECK = []

for (var i = 0; i < Card.NSUITS.length; i++) {
  var s = Card.NSUITS[i];
  Card.SUITS[s] = i;
  for (var j = 0; j < Card.NRANKS.length; j++) {
    var r = Card.NRANKS[j];
    Card.RANKS[r] = j;
    Card.DECK.push(r + s);
  }
}
