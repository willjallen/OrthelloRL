
import logging
import os
import sys
from collections import deque
from pickle import Pickler, Unpickler
from random import shuffle
import json

import numpy as np
from tqdm import tqdm

import subprocess

log = logging.getLogger(__name__)


class Coach():
    """
    This class executes the self-play + learning. It uses the functions defined
    in Game and NeuralNet. args are specified in main.py.
    """

    def __init__(self, nnet, args):
        self.nnet = nnet
        self.pnet = self.nnet.__class__()  # the competitor network
        self.args = args
        self.trainExamplesHistory = []  # history of examples from args.numItersForTrainExamplesHistory latest iterations
        self.skipFirstSelfPlay = False  # can be overriden in loadTrainExamples()



    def learn(self):
        """
        Performs numIters iterations with numEps episodes of self-play in each
        iteration. After every iteration, it retrains neural network with
        examples in trainExamples (which has a maximum length of maxlenofQueue).
        It then pits the new neural network the old one and accepts it
        only if it wins >= updateThreshold fraction of games.
        """


        # Write init of serialized network
        # TODO: Check if one exists or pick up where we left off
        self.nnet.save_checkpoint(folder='./dev/models/ABC123/', filename= '0.pth.tar')
        self.nnet.save_checkpoint(folder='./dev/models/ABC123/', filename='best.pth.tar')
        bestCheckpoint = 0
        currCheckpoint = 0
        
        bestModel = 'best.pth.tar'

        for i in range(1, self.args.numIters + 1):
            # bookkeeping
            log.info(f'Starting Iter #{i} ...')
            
            # Always use the best model for generating examples
            log.info('Calling selfplay subprocess')
            subprocess.run(["./othello/build/othello",
                            "selfplay",
                            str(self.args.selfplayGames),
                            str(self.args.selfplayMCTSSims),
                            './dev/models/ABC123/' + bestModel + '.pt',
                            './dev/models/ABC123/examples.json'])
        

            log.info('Loading training examples')
            self.loadTrainExamples()

            # Ensure we have < maxlenOfQueue examples, delete the oldest ones for space
            print('len hist', len(self.trainExamplesHistory))
            if(len(self.trainExamplesHistory) >= self.args.maxlenOfQueue):
                difference = len(self.trainExamplesHistory) - self.args.maxlenOfQueue
                del self.trainExamplesHistory[0:difference]
                print('len hist', len(self.trainExamplesHistory))


            # Copy examples then shuffle them
            trainExamples = []
            for e in self.trainExamplesHistory:
                trainExamples.append(e)
            shuffle(trainExamples)

            # training new network, keeping a copy of the old one
            # self.nnet.save_checkpoint(folder='./temp/', filename='old.pth.tar')
            # self.pnet.load_checkpoint(folder=self.args.checkpoint, filename='temp.pth.tar')

            self.nnet.train(trainExamples)
            
            currCheckpoint += 1
            currModel =  str(currCheckpoint) + '.pth.tar'
            self.nnet.save_checkpoint(folder='./dev/models/ABC123/', filename=currModel)
            self.nnet.save_checkpoint(folder='./dev/models/ABC123/', filename='best.pth.tar')

            # log.info('Calling arena subprocess')
            # subprocess.run(["./othello/build/othello",
            #                 "arena",
            #                 str(self.args.arenaGames),
            #                 str(self.args.arenaMCTSSims),
            #                 './dev/models/ABC123/' + bestModel + '.pt',
            #                 './dev/models/ABC123/' + currModel +'.pt',
            #                 './dev/models/ABC123/arena.json'])
            #
            # bestWins, currWins, draws = self.loadArenaData()

            # log.info('BEST/CURR WINS : %d / %d ; DRAWS : %d' % (bestWins, currWins, draws))
            # if bestWins + currWins == 0 or float(currWins) / (bestWins + currWins) < self.args.updateThreshold:
            #     log.info('Rejecting new model')
            #     # self.nnet.load_checkpoint(folder=self.args.checkpoint, filename='temp.pth.tar')
            # else:
            #     log.info('Accepting new model')
            #     # self.nnet.save_checkpoint(folder=self.args.checkpoint, filename=self.getCheckpointFile(i))
            #     self.nnet.save_checkpoint(folder='./dev/models/ABC123/', filename=bestModel)

    def getCheckpointFile(self, iteration):
        return 'checkpoint_' + str(iteration) + '.pth.tar'


    def loadTrainExamples(self):
        # modelFile = os.path.join(self.args.load_folder_file[0], self.args.load_folder_file[1])
        # examplesFile = modelFile + ".examples"
        examplesFile = "./dev/models/ABC123/examples.json"
        if not os.path.isfile(examplesFile):
            log.warning(f'File "{examplesFile}" with trainExamples not found!')
            r = input("Continue? [y|n]")
            if r != "y":
                sys.exit()
        else:
            log.info("File with trainExamples found. Loading it...")
            with open(examplesFile, "rb") as f:
                data = json.load(f)
                examples = data['examples']
                for example in examples:
                   self.trainExamplesHistory.append((example['contiguousGameState'], example['pi'], example['reward']))
                 
                    

            log.info('Loading done!')

            
            
            # examples based on the model were already collected (loaded)
            # self.skipFirstSelfPlay = True

    def loadArenaData(self):
        # modelFile = os.path.join(self.args.load_folder_file[0], self.args.load_folder_file[1])
        # examplesFile = modelFile + ".examples"
        arenaFile = './dev/models/ABC123/arena.json'
        if not os.path.isfile(arenaFile):
            log.warning(f'File "{arenaFile}" not found!')
            r = input("Continue? [y|n]")
            if r != "y":
                sys.exit()
        else:
            log.info("Arena file found. Loading it...")
            with open(arenaFile, "rb") as f:
                data = json.load(f)
                return data['numP1Wins'], data['numP2Wins'], data['numTies']
            
            
            # examples based on the model were already collected (loaded)
            # self.skipFirstSelfPlay = True
