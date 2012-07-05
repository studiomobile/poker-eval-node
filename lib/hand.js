var Eval = require('../build/Release/wrapper')
  , Card = require('./card')


var _flat = function(a, b) { return 'function' == typeof b.reduce ? b.reduce(_flat, a) : a.concat(b); }
  , _flatten = function(a) { return 'function' == typeof a.reduce ? a.reduce(_flat, []) : [a]; }


var Hand = function(cards) {
  if ( !(this instanceof Hand) ) {
    return new Hand(cards);
  }
  this.cards = _flatten(cards);
  this.value = Eval.evalCards(this.cards);
  this.type  = Eval.handvalType(this.value);
  this.sig   = Eval.handvalSigCards(this.value).map(function(rank) { return Card.NRANKS[rank]; }).join('');
};

module.exports = Hand;

Hand.prototype.toString = function() { return "" + Hand.Types[this.type] + " (" + this.sig + ")"; };


Hand.NOPAIR    = 0;
Hand.ONEPAIR   = 1;
Hand.TWOPAIR   = 2;
Hand.TRIPS     = 3;
Hand.STRAIGHT  = 4;
Hand.FLUSH     = 5;
Hand.FULLHOUSE = 6;
Hand.QUADS     = 7;
Hand.STFLUSH   = 8;

Hand.Types = [ "NoPair", "OnePair", "TwoPair",  "Trips", "Straight", "Flush", "FullHouse", "Quads", "StFlush" ];
