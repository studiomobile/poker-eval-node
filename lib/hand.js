var lib  = require('../build/Release/wrapper')
  , Card = require('./card')


var flat = function(a, b) {
  if ('function' == typeof b.reduce) {
    return b.reduce(flat, a);
  }
  a.push(b);
  return a;
}


var Hand = function() {
  if ( !(this instanceof Hand) ) {
    return new Hand([].slice.apply(arguments));
  }
  this.cards = [].slice.apply(arguments).reduce(flat, []);
  var result = lib.evalCards(this.cards);
  this.value = result.value;
  this.type  = result.type;
  this.sig   = result.sig.map(function(rank) { return Card.NRANKS[rank]; }).join('');
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
