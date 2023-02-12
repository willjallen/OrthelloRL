from elo import *

elo = Elo(K_FACTOR=32)

# rate_1vs1(self, rating1, rating2, drawn=False):
# scores = (DRAW, DRAW) if drawn else (WIN, LOSS)
new_ratings = elo.rate_1vs1(1200, 900)

# Call arena with a list of directories, for each directory give iteration range, number of games each model should play
# stuff like that

# arena will return a json file with an array of matches
# each match will give
# who it played against
# the outcome of the game

# maybe do it bracket style
# this allows winner to shine
# have a losers bracket too

# tournament # of games say 20, needs to be some tradeoff between the cost of loading the model
# whoever has higher win percent wins tourny
