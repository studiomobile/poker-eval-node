var wrapper = require('./build/Release/wrapper');

exports.Hand = wrapper.Hand;
exports.evalCards = wrapper.evalCards;

var Card = exports.Card = {};

Card.TWO   = 0;
Card.THREE = 1;
Card.FOUR  = 2;
Card.FIVE  = 3;
Card.SIX   = 4;
Card.SEVEN = 5;
Card.EIGHT = 6;
Card.NINE  = 7;
Card.TEN   = 8;
Card.JACK  = 9;
Card.QUEEN = 10;
Card.KING  = 11;
Card.ACE   = 12;

Card.HEARTS   = 0;
Card.DIAMONDS = 1;
Card.CLUBS    = 2;
Card.SPADES   = 3;

Card.NUMS = {
  "2": Card.TWO,
  "3": Card.THREE,
  "4": Card.FOUR,
  "5": Card.FIVE,
  "6": Card.SIX,
  "7": Card.SEVEN,
  "8": Card.EIGHT,
  "9": Card.NINE,
  "t": Card.TEN,
  "j": Card.JACK,
  "q": Card.QUEEN,
  "k": Card.KING,
  "a": Card.ACE,
};

Card.SUITS = {
  "h": Card.HEARTS,
  "d": Card.DIAMONDS,
  "c": Card.CLUBS,
  "s": Card.SPADES,
};

Card.DECK = [];

for (var s in Card.SUITS) {
  for (var n in Card.NUMS) {
    Card.DECK.push(n + s);
  }  
}

var _ = require('underscore');

var cards = _.shuffle(Card.DECK).slice(0, 7);

console.log(cards, wrapper.evalCards(cards));
