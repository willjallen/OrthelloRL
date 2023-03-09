
import matplotlib.pyplot as plt
import json
import numpy as np
import date
legend_elements = [plt.Line2D([0], [0], marker='o', color='w', label=model, 
                              markerfacecolor=model_color_dict[model], markersize=10) 
                   for model in unique_models]
plt.legend(handles=legend_elements, loc='upper left')

# save the plot with a filename that includes the current date and time
filename = f"./data/elo_per_second_plot_{datetime.datetime.now().strftime('%y-%m-%d_%h-%m-%s')}.png"
plt.savefig(filename)
plt.clf()

# A1 model time ~ 13 minutes / itr



# A2 model time ~ 36 minutes / itr

# B1 model time ~ 8 minutes / itr

# 
