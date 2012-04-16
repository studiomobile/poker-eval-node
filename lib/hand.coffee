wrapper = require '../build/Release/wrapper'
_       = require 'underscore'
Card    = require './card'

module.exports = class Hand
  constructor: (cards) ->
    @cards = _.flatten cards
    @value = wrapper.evalCards @cards
    @type  = wrapper.handvalType @value
    @sig   = _.map(wrapper.handvalSigCards(@value), (rank) -> Card.NRANKS[rank]).join('')

  toString: -> "#{Hand.Types[@type]} (#{@sig})"

Hand.NOPAIR    = 0
Hand.ONEPAIR   = 1
Hand.TWOPAIR   = 2
Hand.TRIPS     = 3
Hand.STRAIGHT  = 4
Hand.FLUSH     = 5
Hand.FULLHOUSE = 6
Hand.QUADS     = 7
Hand.STFLUSH   = 8

Hand.Types = [ "NoPair", "OnePair", "TwoPair",  "Trips", "Straight", "Flush", "FullHouse", "Quads", "StFlush" ]
