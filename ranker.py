import elo
from elo import Rating

import os
import logging
import json
import coloredlogs
import copy
import random
from utils import dotdict
import subprocess
import numpy as np
import matplotlib.pyplot as plt
import datetime
log = logging.getLogger(__name__)

coloredlogs.install(level='INFO')  # Change this to DEBUG to see more info.


K_FACTOR = 32

# TODO:
# Check to make sure arena.json has all the models in it
# Eventually only match up players around the same elo

class Ranker():
    def __init__(self, args):
        self.args = args
        self.elo = elo.Elo(k_factor=self.args.k_factor)
        self.arenaIteration = 0
        self.initArenaData()


    '''
        Initialize/update the models recent.json 
    '''
    def initArenaData(self):
        arenaFile = self.args.arena_folder + self.args.arena_file 
        
        arenaData = {}
        if not os.path.isfile(arenaFile):
            if not os.path.isdir('./arena/'):
                os.mkdir('./arena/')
            log.warning(f'File "{arenaFile}" not found!')
            log.info('Creating recent.json')

            arenaData['arenaIteration'] = self.arenaIteration
            arenaData['models'] = []
        else:
            with open(arenaFile, 'rb') as file:
                arenaData = json.load(file)
                self.arenaIteration = arenaData['arenaIteration'] + 1

       
        # Collect all models
        models = []
        models = [d for d in os.listdir(self.args.models_dir) if os.path.isdir(os.path.join(self.args.models_dir, d))]
        
        for model in models:
            modelPerformanceTemplate = {
                "model": model,
                "path": "",
                "iteration": 0,
                "ID": "",
                "ELO": 1200,
                "matches": []
            }

            modelsItr = []
            modelsItr = [f for f in os.listdir(self.args.models_dir + model) if os.path.isfile(os.path.join(self.args.models_dir + model, f))]
            for modelItr in modelsItr:
                if(modelItr.endswith('.pt') and not 'best' in modelItr):
                    # Take every 10th model
                    iteration = int(modelItr.split('.')[0])
                    if(iteration % 10 == 0):
                        
                        modelPerformanceData = copy.copy(modelPerformanceTemplate)
                        modelPerformanceData["path"] = os.path.join(self.args.models_dir + model, modelItr)
                        modelPerformanceData["iteration"] = iteration
                        modelPerformanceData["ID"] = model + '-' + str(iteration)

                        if(not any(d['ID'] == modelPerformanceData["ID"] for d in arenaData['models'])):
                            arenaData['models'].append(modelPerformanceData)

            with open(arenaFile, "w") as file:
                json.dump(arenaData, file, indent=4)




    def loadArenaResults(self, arenaResultsFile):
        # modelFile = os.path.join(self.args.load_folder_file[0], self.args.load_folder_file[1])
        # examplesFile = modelFile + ".examples"
        if not os.path.isfile(arenaResultsFile):
            log.warning(f'File "{arenaResultsFile}" not found!')
            sys.exit()
        else:
            log.info("Arena results file found. Loading it...")
            with open(arenaResultsFile, "rb") as f:
                data = json.load(f)
                return data['numP1Wins'], data['numP2Wins'], data['numTies']
            
            
            # examples based on the model were already collected (loaded)
            # self.skipFirstSelfPlay = True



    def run(self):

        while(True):
            arenaFile = self.args.arena_folder + self.args.arena_file 
            if not os.path.isfile(arenaFile):
                log.warning(f'File "{arenaFile}" not found!')
                return  

            # Load data
            arenaData = None 
            with open(arenaFile, 'rb') as file:
                arenaData = json.load(file)
            


            # Choose model with least number of matches
            numMatches = float('inf') 
            modelOne = None
            for model in arenaData['models']:
                if(len(model["matches"]) < numMatches):
                    modelOne = model
                    numMatches = len(model["matches"])
            
            # Choose the second model randmoly, so long as the match quality > args.match_quality
            matchQuality = 0
            modelTwo = random.choice(arenaData['models'])
            while(matchQuality < args.match_quality and modelOne["ID"] != modelTwo["ID"]):
                modelTwo = random.choice(arenaData['models'])
                matchQuality = self.elo.quality_1vs1(modelOne["ELO"], modelTwo["ELO"]) 
            log.info("Model 1 chosen: " + modelOne["ID"] + " (ELO: " + str(modelOne["ELO"])+ ")")
            log.info("Model 2 chosen: " + modelTwo["ID"] + " (ELO: " + str(modelTwo["ELO"]) + ")")
            log.info("Match quality: " + str(matchQuality))
            # Fight
            arenaResultsFile = self.args.arena_folder + 'out.json'
            subprocess.run(["./othello/build/othello",
                                "arena",
                                str(self.args.games_per_match),
                                str(self.args.MCTSsims),
                                modelOne['path'],
                                modelTwo['path'],
                                arenaResultsFile])

            # Load results
            modelOneWins, modelTwoWins, draws = self.loadArenaResults(arenaResultsFile)
            log.info("Model 1 wins: " + str(modelOneWins) + ", Models 2 wins: " + str(modelTwoWins) + ", Draws: " + str(draws)) 
            
            # Calculate new ELOs
            modelOneOriginalELO = modelOne["ELO"]
            modelTwoOriginalELO = modelTwo["ELO"]

            modelOneUpdatedELO = 0
            modelTwoUpdatedELO = 0

            # ELO calculation is abelian
            
            modelOneOutcomes = []
            modelTwoOutcomes = []
            for _ in range(0, modelOneWins):
                modelOneOutcomes.append((1, modelTwoOriginalELO))
                modelTwoOutcomes.append((0, modelOneOriginalELO))
            for _ in range(0, modelTwoWins):
                modelOneOutcomes.append((0, modelTwoOriginalELO))
                modelTwoOutcomes.append((1, modelOneOriginalELO))
            for _ in range(0, draws):
                modelOneOutcomes.append((0.5, modelTwoOriginalELO))
                modelTwoOutcomes.append((0.5, modelOneOriginalELO))

            modelOneUpdatedELO = self.elo.rate(Rating(modelOneOriginalELO), modelOneOutcomes)
            modelTwoUpdatedELO = self.elo.rate(Rating(modelTwoOriginalELO), modelTwoOutcomes)
            
            # Save match
            match = {
                "arenaIteration": self.arenaIteration,
                "modelOneID": modelOne["ID"],
                "modelTwoID": modelTwo["ID"],
                "numGames": self.args.games_per_match,
                "modelOneWins": modelOneWins,
                "modelTwoWins": modelTwoWins,
                "draws": draws,
                "modelOnePreviousELO": modelOneOriginalELO,
                "modelOneNewELO": modelOneUpdatedELO,
                "modelTwoPreviousElo": modelTwoOriginalELO,
                "modelTwoNewELO": modelTwoUpdatedELO,
            }

            log.info(modelOne["ID"] + " new ELO: " + str(modelOneUpdatedELO) + ", (prev ELO: " + str(modelOneOriginalELO) + ")")
            log.info(modelTwo["ID"] + " new ELO: " + str(modelTwoUpdatedELO) + ", (prev ELO: " + str(modelTwoOriginalELO) + ")")
            
            # Update data
            for model in arenaData['models']:
                if(model["ID"] == modelOne["ID"]):
                    model["ELO"] = modelOneUpdatedELO
                    model["matches"].append(match)
                if(model["ID"] == modelTwo["ID"]):
                    model["ELO"] = modelTwoUpdatedELO
                    model["matches"].append(match)

            # Write new data to file
            arenaFileRecent = self.args.arena_folder + self.args.arena_file
            arenaFileSave = self.args.arena_folder + 'arena-' + str(self.arenaIteration) + '.json'

            arenaData['arenaIteration'] = self.arenaIteration

            with open(arenaFile, "w") as file:
                json.dump(arenaData, file, indent=4)
            with open(arenaFileSave, "w") as file:
                json.dump(arenaData, file, indent=4)

            # Save plots
            self.savePlots()

            # Next iteration
            self.arenaIteration += 1

    def savePlots(self):
        arenaFile = self.args.arena_folder + self.args.arena_file
        with open(arenaFile, 'rb') as file:
            arenaData = json.load(file)
        # Extract the data
        iterations = []
        elos = []
        models = []
        for model_data in arenaData['models']:
            iterations.extend([model_data["iteration"]])
            elos.extend([model_data["ELO"]])
            models.extend([model_data["model"]])

        # Create a dictionary to map unique model names to unique colors
        unique_models = np.unique(models)
        model_color_dict = {'A1': 'navy', 'A2': 'slateblue', 'B1': 'maroon'} 

        # Create the scatter plot with colored points
        model_colors = [model_color_dict[model] for model in models]
        plt.scatter(iterations, elos, c=model_colors)
        plt.xlabel("Iteration")
        plt.ylabel("ELO")
        plt.title("ELO vs Iteration for all models")

        # Set y-axis ticks and labels
        min_elo = min(elos)
        max_elo = max(elos)
        yticks = np.arange(min_elo - min_elo % 25, max_elo + 25, 25)
        plt.yticks(yticks)

        # Add legend to the plot
        legend_elements = [plt.Line2D([0], [0], marker='o', color='w', label=model, 
                                      markerfacecolor=model_color_dict[model], markersize=10) 
                           for model in unique_models]
        plt.legend(handles=legend_elements, loc='upper left')

        # Save the plot with a filename that includes the current date and time
        filename = f"./data/generated/elo_plot_{datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.png"
        plt.savefig(filename)
        plt.clf()


        # rate_1vs1(self, rating1, rating2, drawn=False):
# scores = (DRAW, DRAW) if drawn else (WIN, LOSS)
# new_ratings = elo.rate_1vs1(1200, 900)
# 


args = dotdict({
    'arena_folder': './arena/',
    'arena_file': 'recent.json',
    'models_dir': './dev/models/',
    'selection_method': 'uniform',
    'k_factor': 30,
    'games_per_match': 10.0,
    'MCTSsims': 50,
    'match_quality': .50
})


ranker = Ranker(args)
ranker.run()

    # def run():
    #     pass
    #
    # def startGames():
    #     pass
    #
    # def evaluateResults():
    #     pass
    #
    # def saveModelInfo(self):
    #
