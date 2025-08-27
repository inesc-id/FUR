import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Read data
NB_FILES = 10
files = [
    "replayer/naive-s{}.csv",
    "replayer/log_link-s{}.csv",
    "replayer/seq_log-s{}.csv"
]
solutions = [
    "prior PHTs",
    "SPHT",
    "FUR"
]

X = []
Ys = []
err = []


for f in files:
    r = pd.DataFrame()
    for i in range(NB_FILES):
        aux = pd.read_csv(f.format(i), sep="\t").sort_values(by=["-n"])
        aux["throughput"] = 1e9 / aux["latency"]
        r = pd.concat((r, aux))
    if len(X) == 0:
        X = r["-n"].unique()
    Ys += [[r[r["-n"] == x].mean()["throughput"] / 1000 for x in X]]
    err += [[r[r["-n"] == x].std()["throughput"] / 1000 for x in X]]

# Create the first subplot
fig, axs = plt.subplots(figsize=(6,2.5), nrows=1, ncols=1)
# axs.set_ylim([2e5,2.9e5])
axs.set_ylim([0,2.9e2])

for y,e,s in zip(Ys,err,solutions):
	if s == "prior PHTs":
		axs.errorbar(X, y, yerr=e, label=s, lw=3, markersize=11, marker="o")
	if s == "SPHT":
		axs.errorbar(X, y, yerr=e, label=s, lw=3, markersize=11, marker="d")
	if s == "FUR":
		axs.errorbar(X, y, yerr=e, label=s, lw=3, markersize=11, marker="*")

axs.set_ylabel('Throughput\n(x1000 TXs/s)', fontsize=13)
axs.yaxis.set_label_coords(-0.08, 0.5)
axs.set_xlabel('No. of per-thread write logs', fontsize=13)
#axs[1].set_xticklabels([])

# Adjust layout to prevent clipping of the second y-axis label
plt.subplots_adjust(left=0.14, right=0.995, top=0.995, bottom=0.19, hspace=0.05, wspace=0.05)
axs.margins(x=0.01)

# Show the plot
axs.legend(fontsize=12, loc="lower left", bbox_to_anchor=(0.0, 0.0))
axs.set_title("Replay throughput")
plt.savefig(f"replayer_70_delay.pdf")
#plt.show()
