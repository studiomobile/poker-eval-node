require ('coffee-script');

exports.Hand = require('./lib/hand');
exports.Card = require('./lib/card');

exports.evalCards = function() { return new exports.Hand([].slice.apply(arguments)); };
