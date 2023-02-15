
import matplotlib.pyplot as plt
import json
import numpy as np

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

plt.show()
