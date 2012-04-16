var lib = require('./index.js')
  , _   = require('underscore')

var cards = _.shuffle(lib.Card.DECK).slice(0, 7);

var hand = lib.evalCards(cards);

console.log("" + hand);
console.log(hand);
