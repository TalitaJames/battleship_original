import matplotlib.pyplot as plt
import numpy as np

dataLbls = ["RND",  "P_MAX",  "P_RND",  "infoGain_MAX",  "infoGain_RND", "DIAGONAL", "FLEXI"];
# dataLbls = ["RND",  "P_MAX",   "infoGain_MAX",  "DIAGONAL", "FLEXI"];

filepath = "../out/"
filename = f"{filepath}turnsTaken.out"
with open(filename) as f:
    data = [[int(y) for y in x.rstrip(',').split(',')] for x in f.read().splitlines()]


#region plot
fig, ax1 = plt.subplots(figsize=(10, 6))

bp = ax1.boxplot(data, sym='+', vert=True, whis=1.5)

ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey', alpha=0.5)

ax1.set_xticklabels(dataLbls)
ax1.set_xlabel("Play Style")
ax1.set_ylabel("Turns taken")

plt.savefig(f"{filepath}turnsTaken_BoxPlot.png")
plt.show()
# endregion plot
