from elo import *
import os
import logging
import json
import coloredlogs
import copy
import random
from utils import dotdict
import subprocess

log = logging.getLogger(__name__)

coloredlogs.install(level='INFO')  # Change this to DEBUG to see more info.


K_FACTOR = 32

# TODO:
# Check to make sure arena.json has all the models in it
# Eventually only match up players around the same elo

class Ranker():
    def __init__(self, args):
        self.args = args
        self.elo = Elo(k_factor=32)
        self.initArenaData()
        pass

    '''
        Initialize/update the models arena.json 
    '''
    def initArenaData(self):
        arenaFile = self.args.arena_folder + 'arena.json'
        
        arenaData = [] 
        if not os.path.isfile(arenaFile):
            if not os.path.isdir('./arena/'):
                os.mkdir('./arena/')
            log.warning(f'File "{arenaFile}" not found!')
            log.info('Creating area.json')
        else:
            with open(arenaFile, 'rb') as file:
                arenaData = json.load(file)

       
        # Collect all models
        models = []
        models = [d for d in os.listdir(self.args.models_dir) if os.path.isdir(os.path.join(self.args.models_dir, d))]
        
        for model in models:
            modelPerformanceTemplate = {
                "Model": model,
                "path": "./dev/models/ABC123/7.pth.tar",
                "Iteration": 0,
                "ID": "",
                "ELO": 1200,
                "Matches": []
            }

            modelsItr = []
            modelsItr = [f for f in os.listdir(self.args.models_dir + model) if os.path.isfile(os.path.join(self.args.models_dir + model, f))]
            for modelItr in modelsItr:
                if(modelItr.endswith('.pt') and not 'best' in modelItr):
                    # Take every 5th model
                    iteration = int(modelItr.split('.')[0])
                    if(iteration % 5 == 0):
                        
                        modelPerformanceData = copy.copy(modelPerformanceTemplate)
                        modelPerformanceData["path"] = os.path.join(self.args.models_dir + model, modelItr)
                        modelPerformanceData["Iteration"] = iteration
                        modelPerformanceData["ID"] = model + '-' + str(iteration)

                        if(not any(d['ID'] == modelPerformanceData["ID"] for d in arenaData)):
                            arenaData.append(modelPerformanceData)

            with open(arenaFile, "w") as file:
                json.dump(arenaData, file, indent=4)

        else:
            log.info('arena.json found')


    def loadArenaResults(self, arenaResultsFile):
        # modelFile = os.path.join(self.args.load_folder_file[0], self.args.load_folder_file[1])
        # examplesFile = modelFile + ".examples"
        if not os.path.isfile(arenaResultsFile):
            log.warning(f'File "{arenaResultsFile}" not found!')
            r = input("Continue? [y|n]")
            if r != "y":
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
            arenaFile = self.args.arena_folder + 'arena.json'
            if not os.path.isfile(arenaFile):
                log.warning(f'File "{arenaFile}" not found!')
                return  

            # Load data
            arenaData = None 
            with open(arenaFile, 'rb') as file:
                arenaData = json.load(file)
            
            # print(arenaData)

            # Choose model with least number of matches
            numMatches = float('inf') 
            playerOne = None
            for model in arenaData:
                if(len(model["Matches"]) < numMatches):
                    playerOne = model
                    numMatches = len(model["Matches"])

            matchQuality = 0
            playerTwo = random.choice(arenaData)
            while(matchQuality < args.match_quality and playerOne["ID"] != playerTwo["ID"]):
                playerTwo = random.choice(arenaData)
                matchQuality = quality_1vs1(playerOne["ELO"], playerTwo["ELO"]) 
            log.info("Player 1 chosen: " + playerOne["ID"])
            log.info("Player 2 chosen: " + playerTwo["ID"])
            log.info("Match quality: " + str(matchQuality))
            # Fight
            arenaResultsFile = self.args.arena_folder + 'out.json'
            subprocess.run(["./othello/build/othello",
                                "arena",
                                str(self.args.games_per_match),
                                str(self.args.MCTSsims),
                                playerOne['path'],
                                playerTwo['path'],
                                arenaResultsFile])

            # Load results
            playerOneWins, playerTwoWins, draws = self.loadArenaResults(arenaResultsFile)

            match = {
                "PlayerOneID": playerOne["ID"],
                "PlayerTwoID": playerTwo["ID"],
                "games": self.args.games_per_match,
                "playerOneWins": playerOneWins,
                "playerTwoWins": playerTwoWins,
                "draws": draws,
                "outcome": 0
            }

            # print(playerOneWins, playerTwoWins, draws)
            playerOneUpdatedRating = 0
            playerTwoUpdatedRating = 0
            new_ratings = None
            if(float(playerOneWins / self.args.games_per_match) >= self.args.percent_win_threshold):
                # P1 wins, P2 loses
                new_ratings = rate_1vs1(Rating(playerOne["ELO"]), Rating(playerTwo["ELO"]), drawn=False)
                playerOneUpdatedRating = new_ratings[0]
                playerTwoUpdatedRating = new_ratings[1]
                match["outcome"] = playerOne["ID"]
            elif(float(playerTwoWins / self.args.games_per_match) >= self.args.percent_win_threshold):
                # P2 wins, P1 loses
                new_ratings = rate_1vs1(Rating(playerTwo["ELO"]), Rating(playerOne["ELO"]), drawn=False)
                playerOneUpdatedRating = new_ratings[1]
                playerTwoUpdatedRating = new_ratings[0]
                match["outcome"] = playerTwo["ID"]
            else:
                new_ratings = rate_1vs1(Rating(playerOne["ELO"]), Rating(playerTwo["ELO"]), drawn=True)
                playerOneUpdatedRating = new_ratings[0]
                playerTwoUpdatedRating = new_ratings[1]

            log.info(playerOne["ID"] + " new ELO: " + str(playerOneUpdatedRating) + ", (prev ELO: " + str(playerOne["ELO"]))
            log.info(playerTwo["ID"] + " new ELO: " + str(playerTwoUpdatedRating) + ", (prev ELO: " + str(playerTwo["ELO"]))

            for model in arenaData:
                if(model["ID"] == playerOne["ID"]):
                    model["ELO"] = playerOneUpdatedRating
                    model["Matches"].append(match)
                if(model["ID"] == playerTwo["ID"]):
                    model["ELO"] = playerTwoUpdatedRating
                    model["Matches"].append(match)

            arenaFile = self.args.arena_folder + 'arena.json'
            with open(arenaFile, "w") as file:
                json.dump(arenaData, file, indent=4)

            # Save plots
            self.savePlots()

    def savePlots(self):
        with open('./arena/arena.json', 'rb') as file:
            arenaData = json.load(file)
            # Extract the data
            iterations = []
            elos = []
            models = []
            for model_data in arenaData:
                iterations.extend([model_data["Iteration"]])
                elos.extend([model_data["ELO"]])
                models.extend([model_data["Model"]])

        # Create a dictionary to map unique model names to unique colors
        unique_models = np.unique(models)
        colors = plt.cm.get_cmap('tab20c', len(unique_models))
        model_color_dict = dict(zip(unique_models, colors(range(len(unique_models)))))

        # Create the scatter plot with colored points
        model_colors = [model_color_dict[model] for model in models]
        plt.scatter(iterations, elos, c=model_colors)
        plt.xlabel("Iteration")
        plt.ylabel("ELO")
        plt.title("ELO vs Iteration for all models")


        # Add legend to the plot
        legend_elements = [plt.Line2D([0], [0], marker='o', color='w', label=model, 
                                      markerfacecolor=model_color_dict[model], markersize=10) 
                           for model in unique_models]
        plt.legend(handles=legend_elements, loc='best')

        # Save the plot with a filename that includes the current date and time
        filename = f".data/elo_plot_{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.png"
        plt.savefig(filename)


        # rate_1vs1(self, rating1, rating2, drawn=False):
# scores = (DRAW, DRAW) if drawn else (WIN, LOSS)
# new_ratings = elo.rate_1vs1(1200, 900)
# 


args = dotdict({
    'arena_folder': './arena/',
    'models_dir': './dev/models/',
    'selection_method': 'uniform',
    'games_per_match': 20.0,
    'MCTSsims': 50,
    'percent_win_threshold': .6, 
    'match_quality': .8
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
