import matplotlib.pyplot as plt
import numpy as np

dataLbls = ["RND",  "P_MAX",  "P_RND",  "infoGain_MAX",  "infoGain_RND", "DIAGONAL", "FLEXI"];

filename = "../out/turnsTaken.out";
with open(filename) as f:
    data = [[int(y) for y in x.rstrip(',').split(',')] for x in f.read().splitlines()]


#region plot
fig, ax1 = plt.subplots(figsize=(10, 6))

bp = ax1.boxplot(data, sym='+', vert=True, whis=1.5)

ax1.yaxis.grid(True, linestyle='-', which='major', color='lightgrey', alpha=0.5)

ax1.set_xticklabels(dataLbls)
ax1.set_ylabel("Turns taken")

plt.show()
# endregion plot
