module.exports = class Card

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

for s, i in Card.NSUITS
  Card.SUITS[s] = i
  Card.NSUITS[Card.SUITS[s]] = s
  for r, i in Card.NRANKS
    Card.RANKS[r] = i
    Card.DECK.push(r + s);
