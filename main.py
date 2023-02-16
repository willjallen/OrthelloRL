import logging

import coloredlogs

from coach import Coach
from networks.NNet import NNetWrapper as nn
from utils import *

log = logging.getLogger(__name__)

coloredlogs.install(level='INFO')  # Change this to DEBUG to see more info.

args = dotdict({
    'numIters': 1000,
    'tempThreshold': 25,            # Number of MCTS itr during selfplay before temp is set to 0
    'maxlenOfQueue': 200000,        # Number of game examples to train the neural networks.
    'selfplayGames': 100,            # Number of games to be played during self play
    'selfplayMCTSSims': 25,      # Number of games moves for MCTS to simulate for self play.
    'arenaMCTSSims': 50,         # Number of games moves for MCTS to simulate for arena.
    'cpuct': 1,
    'checkpoint': './temp/',
    'model_folder': './dev/models/ABC123/',
    'load_model': True,
    'starting_itr': 89,
    'load_folder_file': ('./dev/models/ABC123/','examples.json'),
    'numItersForTrainExamplesHistory': 20,

})


def main():
    # log.info('Loading %s...', Game.__name__)
    # g = Game(6)

    log.info('Loading %s...', nn.__name__)
    nnet = nn()

    if args.load_model:
        log.info('Loading checkpoint "%s/%s"...', args.model_folder, 'best.pth.tar')
        nnet.load_checkpoint(folder=args.model_folder, filename='best.pth.tar')
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
