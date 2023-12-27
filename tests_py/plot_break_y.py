import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# TODO: come up with some more automatic way of doing this

# Read data
files = [
    "dataH1H2_test/pisces-s0.csv",
    "dataH1H2_test/psi-s0.csv",
    "dataH1H2_test/htm-sgl-s0.csv",
    "dataH1H2_test/psi-strong-s0.csv",
    "dataH1H2_test/si-htm-s0.csv",
    "dataH1H2_test/spht-s0.csv"
]
solutions = [
    "Pisces",
    "PSI",
    "HTM",
    "PSI-strong",
    "SI-HTM",
    "SPHT"
]

read_all = [pd.read_csv(f, sep="\t") for f in files]
Xs = []
Ys = []
for df in read_all:
    d = df
    d = d[d["-u"] == 50]
    d = d[d["-i"] == 1000000]
    #breakpoint()
    x = d["-n"]
    y = d["-d"]/d["time"]
    t = list(zip(x,y))
    t.sort(key=lambda e : e[0])
    x = []
    y = []
    for x_t,y_t in t:
        x += [x_t]
        y += [y_t]
    Xs += [x]
    Ys += [y]

# Create the first subplot
fig, axs = plt.subplots(nrows=2, ncols=1, sharex=True, gridspec_kw={'height_ratios': [1, 3]})
axs[0].spines.bottom.set_visible(False)
axs[1].spines.top.set_visible(False)
#axs[0].tick_params(labeltop=False)

# Plot the first segment

for x,y,s in zip(Xs,Ys,solutions):
    axs[1].plot(x, y, label=s)
    axs[0].plot(x, y, label=s)
axs[1].set_ylabel('Throughput (TXs/s)')
axs[1].set_xlabel('Threads')
#axs[1].set_xticklabels([])

# 200000 items
#axs[0].set_ylim([2.0e6,8.0e6]) # below
#axs[1].set_ylim([0,2.0e6]) # top

# 50000 items
#axs[0].set_ylim([4.0e6,2.0e7]) # below
#axs[1].set_ylim([0,4.0e6]) # top

# 1M items
axs[0].set_ylim([2.0e5,5.0e5]) # below
axs[1].set_ylim([0,2.0e5]) # top

# Adjust layout to prevent clipping of the second y-axis label
plt.tight_layout()
plt.subplots_adjust(hspace=0, wspace=0.05)

# Show the plot
axs[1].legend()
plt.savefig(f"hashmap_50_upts_1M_items.pdf")
#plt.show()
