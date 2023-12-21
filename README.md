# OthelloRL

This code complements the undergraduate thesis project [*Applications of Neural Architecture Search to Deep Reinforcement Learning Methods*](https://oaktrust.library.tamu.edu/handle/1969.1/200275).

We implemented a deep reinforcement learning model similar to the one defined in the
AlphaGo/AlphaZero. Our iterative process matches that of AlphaZero as it consists of only two
steps: self-play and training. We do not include an evaluation step between iterations to choose
which model will produce training data for the next iteration. Instead, we augment the most recent
model with training data. In parallel, we employ an ELO rating system to synchronously track the
generalized performance of the models across iterations.

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
    py -m pip install requirements.txt
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
Poop

