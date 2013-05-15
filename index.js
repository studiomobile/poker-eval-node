var binding = require('./build/Release/wrapper');

module.exports.Hand = require('./lib/hand');
module.exports.Card = require('./lib/card');

module.exports.knownGames = binding.knownGames;
module.exports.enumGame   = binding.enumGame;
module.exports.evalGame   = binding.evalGame;
module.exports.enumHand   = binding.enumHand;
