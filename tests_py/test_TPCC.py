#!/bin/env python3

from common import BenchmarkParameters, CollectData
from parse_sol import Parser
from plot import LinesPlot, BackendDataset

if __name__ == "__main__":
  params = BenchmarkParameters(["-w", "-s", "-d", "-o", "-p", "-r", "-n"])
  params.set_params("-w", [32])
  params.set_params("-s", [4])
  params.set_params("-d", [4])
  params.set_params("-o", [4])
  params.set_params("-p", [43])
  params.set_params("-r", [45])
  params.set_params("-n", [1, 2, 4, 8, 12, 16])
  nb_samples = 10
  locations = [
    "/home/ubuntu/PersistentSiHTM/power8tm-pisces/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc",
    "/home/ubuntu/PersistentSiHTM/POWER8TM/benchmarks/tpcc"
  ]
  backends = [
    "pisces",
    "spht",
    "p8tm-si",
    "p8tm-psi-v2-ci",
    "p8tm-psi-v2-co",
    "p8tm-psi-v2-fi",
    "htm-sgl"
  ]
  data_folder = "dataTPCC"

  datasets_thr = {}
  datasets_aborts = {}
  for loc,backend in zip(locations,backends):
    for s in range(nb_samples):
      data = CollectData(
          loc,
          "code/tpcc",
          "build-tpcc.sh",
          backend,
          f"{data_folder}/{backend}-s{s}"
        )
      # data.run_sample(params) # TODO: not running samples
      # parser = Parser(f"{data_folder}/{backend}-s{s}")
      # parser.parse_all(f"{data_folder}/{backend}-s{s}.csv")
    for u in params.params["-u"]:
      if u not in datasets_thr:
        datasets_thr[u] = {}
      for i in params.params["-i"]:
        if i not in datasets_thr[u]:
          datasets_thr[u][i] = []
        ds = BackendDataset(
          backend,
          [f"{data_folder}/{backend}-s{s}.csv" for s in range(nb_samples)],
          lambda e: e["-n"], "Nb. Threads",
          lambda e: e["-d"]/e["time"], "Throughput (T/s)",
          {"-u": u, "-i": i}
        )
        if backend != "pisces":
          ds.add_stack("Commits vs Aborts", "Count", {
            "ROT-commits": lambda e: e["rot-commits"] if "rot-commits" in e else 0,
            "HTM-commits": lambda e: e["htm-commits"],
            "SGL-commits": lambda e: e["gl-commits"],
            "aborts": lambda e: e["total-aborts"]
          })
          ds.add_stack("Abort types", "Nb. aborts", {
            "conflict-transactional": lambda e: e["confl-trans"],
            "conflict-non-transactional": lambda e: e["confl-non-trans"],
            "self": lambda e: e["confl-self"],
            "capacity": lambda e: e["capac-aborts"],
            "persistent": lambda e: e["persis-aborts"],
            "user": lambda e: e["user-aborts"],
            "other": lambda e: e["other-aborts"]
          })
        datasets_thr[u][i] += [ds]
    
  for u,v in datasets_thr.items():
    for i,w in v.items():
      lines_plot = LinesPlot(f"{u}% updates, {i/1000}k initial items", f"thr_{u}upds_{i}items.pdf", figsize=(8, 4))
      lines_plot.plot(w)
      lines_plot.plot_stack(w)
