import logging

import coloredlogs

from coach import Coach
from networks.NNet import NNetWrapper as nn
from utils import *

log = logging.getLogger(__name__)

coloredlogs.install(level='INFO')  # Change this to DEBUG to see more info.

args = dotdict({
    'numIters': 1000,
    'tempThreshold': 15,            #
    'updateThreshold': 0.6,         # During arena playoff, new neural net will be accepted if threshold or more of games are won.
    'maxlenOfQueue': 200000,        # Number of game examples to train the neural networks.
    'selfplayGames': 10,            # Number of games to be played during self play
    'selfplayMCTSSims': 25,      # Number of games moves for MCTS to simulate for self play.
    'arenaMCTSSims': 25,         # Number of games moves for MCTS to simulate for arena.
    'arenaGames': 10,             # Number of games to play during arena play to determine if new net will be accepted.
    'cpuct': 1,

    'checkpoint': './temp/',
    'load_model': False,
    'load_folder_file': ('./dev/models/ABC123/','examples.json'),
    'numItersForTrainExamplesHistory': 20,

})


def main():
    # log.info('Loading %s...', Game.__name__)
    # g = Game(6)

    log.info('Loading %s...', nn.__name__)
    nnet = nn()

    if args.load_model:
        log.info('Loading checkpoint "%s/%s"...', args.load_folder_file[0], args.load_folder_file[1])
        nnet.load_checkpoint(args.load_folder_file[0], args.load_folder_file[1])
    else:
        log.warning('Not loading a checkpoint!')

    log.info('Loading the Coach...')
    c = Coach(nnet, args)

    if args.load_model:
        log.info("Loading 'trainExamples' from file...")
        c.loadTrainExamples()

    log.info('Starting the learning process 🎉')
    c.learn()


if __name__ == "__main__":
    main()
