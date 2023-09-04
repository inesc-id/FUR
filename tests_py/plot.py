import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

class BackendDataset:
  def __init__(self, name, samples, x_fn, x_label, y_fn, y_label, selector):
    self.name = name
    read_all = [pd.read_csv(s, sep="\t") for s in samples]
    self.samples = []
    for df in read_all:
      d = df
      for k,v in selector.items():
        d = d[d[k] == v]
      self.samples += [d]
    self.x_param = np.array([[x_fn(s)] for s in self.samples])
    self.y_param = np.array([[y_fn(s)] for s in self.samples])
    self.x_label = x_label
    self.y_label = y_label

class LinesPlot:
  def __init__(self, title, filename, figsize=(5, 4)):
    self.title = title
    self.filename = filename
    self.figsize = figsize

  def plot(self, datasets:list[BackendDataset]):
    fig, ax = plt.subplots(figsize=self.figsize, nrows=1, ncols=1)
    for d in datasets:
      triple = [(np.average(x),np.average(y),np.std(y)) for x,y in zip(d.x_param.transpose(), d.y_param.transpose())]
      triple.sort(key=lambda elem : elem[0]) # sort by X
      x_array, y_array, y_error = zip(*triple)
      ax.errorbar(x_array, y_array, yerr = y_error, label=d.name)
    ax.set_xlabel(d.x_label)
    ax.set_ylabel(d.y_label)
    ax.set_title(self.title)
    ax.legend()
    plt.tight_layout()
    plt.savefig(self.filename)
