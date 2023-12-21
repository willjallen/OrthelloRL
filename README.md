# OthelloRL

This code complements the undergraduate thesis project [_Applications of Neural Architecture Search to Deep Reinforcement Learning Methods_](https://oaktrust.library.tamu.edu/handle/1969.1/200275).



We implemented a deep reinforcement learning model similar to the one defined in the
AlphaGo/AlphaZero. Our iterative process matches that of AlphaZero as it consists of only two
steps: self-play and training. We do not include an evaluation step between iterations to choose
which model will produce training data for the next iteration. Instead, we augment the most recent
model with training data. Additionally, the MCTS for AlphaZero was constructed asynchronously using root node parallelization

More details regarding the specifics of this implementation can be found in the **Methods** section of the paper.

## Self Play

During a self-play episode, the initial state of the game will be set to one of two possible opening states, with a 50% chance of being either. Othello is symmetric across 7 transformations and we augment our example data with these symmetries. The MCTS exploration parameter λ is set
to 1. The temperature τ is set to 1 on the first turn and remains that way until it is set to 0 at turn 25. At the end of the episode, all n articles of example data are written to a file as tuples.

## Arena

Two models may be compared via the arena. During this evaluation we set the temperature according to the distribution τ = 0.65^(t+1), where t is the turn number. This ensures that the first move will be chosen pseudo-uniformly and by move 6 the agents will choose moves based on the highest number of visits as usual.

# Setup (Common)

- Ensure the relevant CUDA system libraries and developer tools are installed on your system.

- Clone the repository.

  ```
  git clone git@github.com:willjallen/OthelloRL.git
  ```

- Create a virtual environment (`python<=10.0` required for torch)

  ```
  py -m venv .venv
  ```

- Install the required libraries

  ```
  py -m pip install -r requirements.txt
  ```

- Download the appropriate LibTorch package from https://pytorch.org/ and place it under `./othello/`

# Build (Windows)

- Open `Developer Powershell for VS xxxx`
- Navigate to the `othello` directory
- Run `./build.bat`

# Build (Unix)

- Navigate to the `othello` directory
- Run `./build.sh`

# Usage

Set the desired model in the args of `main.py` and `networks/NNet.py`.

Run `py main.py` to begin the training process.

To call the othello MCTS engine directly:

```
usage: othello <command> [<args>]
commands:
  selfplay [numGames] [MCTSsims] [modelPath] [outputPath]
  arena [numGames] [MCTSsims] [modelPathOne] [modelPathTwo] [outputPath]
```

where `modelPath` is a path to the torch jit traced model and `outputPath` is the path to the resulting json file of either selfplay examples or arena outcomes.
