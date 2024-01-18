#!/bin/env python3

from common import BenchmarkParameters, CollectData
from parse_sol import Parser
from plot import LinesPlot, BackendDataset

if __name__ == "__main__":
  params = BenchmarkParameters(["-w", "-m", "-s", "-d", "-o", "-p", "-r", "-n", "-t"])
  params.set_params("-w", [32]) # nb warehouses
  params.set_params("-m", [32]) # max nb warehouses (put the same as -w)
  params.set_params("-t", [5])
  
  # From SI-HTM paper: mixed scenario
  # params.set_params("-s", [4], True)
  # params.set_params("-d", [4], True)
  # params.set_params("-o", [4], True)
  # params.set_params("-p", [43], True)
  # params.set_params("-r", [45], True)

# #   From SI-HTM paper: read-dominated scenario
#   params.set_params("-s", [4], True)
#   params.set_params("-d", [4], True)
#   params.set_params("-o", [80], True)
#   params.set_params("-p", [4], True)
#   params.set_params("-r", [8], True)
  
  # # From SPHT paper
  # params.set_params("-s", [0], True)
  # params.set_params("-o", [0], True)
  # params.set_params("-p", [95], True)
  # params.set_params("-r", [2], True)
  # params.set_params("-d", [3], True)

# Misc configurations:
  params.set_params("-s", [10], True)
  params.set_params("-o", [0], True)
  params.set_params("-p", [85], True)
  params.set_params("-r", [2], True)
  params.set_params("-d", [3], True)

  # params.set_params("-s", [4, 8], True) # to pass more than 1 combination of values
  # params.set_params("-d", [4, 4], True)
  # params.set_params("-o", [4, 4], True)
  # params.set_params("-p", [43, 39], True)
  # params.set_params("-r", [45, 45], True)
  params.set_params("-n", [4, 16, 32])
  #params.set_params("-n", [1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64])
  nb_samples = 1
  locations = [
    "../power8tm-pisces/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
  ]
  # The backend name goes here (don't forget to match the position in the
  # "backends" list with the position in the "locations" list)
  backends = [
    "pisces",
    # "psi",
    # # "psi-strong",
    # "spht",
    # "htm-sgl",
    # "htm-sgl-sr",
    # "si-htm",
    # "ureads-strong",
    # "ureads-p8tm"
  ]

  # Label names in the plots
  name_map = {
    "psi" : "DUMBO-SI",
    "psi-strong" : "DUMBO-Opa",
    "pisces" : "Pisces",
    "htm-sgl" : "HTM",
    "htm-sgl-sr" : "HTM+sus",
    "spht" : "SPHT",
    "si-htm" : "SI-TM",
    "ureads-strong": "ureads-strong", 
    "ureads-p8tm": "ureads-p8tm"
  }
  
  data_folder = "dataTPCC_spht_scenario"

  datasets_thr = {}
  datasets_aborts = {}
  for loc,backend in zip(locations,backends):
    for sample in range(nb_samples):
      data = CollectData(
          loc,
          "code/tpcc",
          "build-tpcc.sh",
          backend,
          f"{data_folder}/{backend}-s{sample}"
        )
      data.run_sample(params) # TODO: not running samples
      parser = Parser(f"{data_folder}/{backend}-s{sample}")
      parser.parse_all(f"{data_folder}/{backend}-s{sample}.csv")
    lst_each = params.list_for_each_param(["-s", "-d", "-o", "-p", "-r"])
    # print(lst_each)
    for s,d,o,p,r in lst_each:
      if (s,d,o,p,r) not in datasets_thr:
        datasets_thr[(s,d,o,p,r)] = []
      ds = BackendDataset(
        name_map[backend],
        [f"{data_folder}/{backend}-s{sample}.csv" for sample in range(nb_samples)],
        lambda e: e["-n"], "Nb. Threads",
        lambda e: e["txs-tpcc"]/e["time-tpcc"], "Throughput (T/s)",
        {"-s": s, "-d": d, "-o": o, "-p": p, "-r": r}
      )

      def filter_threads(t) -> bool:
        x, y, sd = t
        # breakpoint()
        return True if x in [2, 8, 16, 24, 32, 64] else False
          
      ds.add_stack("Commits vs Aborts", "Count", {
            "ROT-commits": lambda e: e["rot-commits"] if "rot-commits" in e else 0,
            "HTM-commits": lambda e: e["htm-commits"],
            "SGL-commits": lambda e: e["gl-commits"],
            "aborts": lambda e: e["total-aborts"]
          })

      # Adds a bar plot for the abort type.
      ds.add_stack("Abort types", "Nb. aborts", {
        "conflict-transactional": lambda e: e["confl-trans"] + e["rot-trans-aborts"],
        "conflict-non-transactional": lambda e: e["confl-non-trans"] + e["rot-non-trans-aborts"],
        "self": lambda e: e["confl-self"] + e["rot-self-aborts"],
        "capacity": lambda e: e["capac-aborts"] + e["rot-capac-aborts"],
        "persistent": lambda e: e["persis-aborts"] + e["rot-persis-aborts"],
        "user": lambda e: e["user-aborts"] + e["rot-user-aborts"],
        "other": lambda e: e["other-aborts"] + e["rot-other-aborts"]
      })

      # Adds a bar plot for the profile information.
      def divByNumUpdTxs(e, attr):
        if (e["htm-commits"]+e["rot-commits"] == 0).any():
          return 0
        else:
          return (e[attr] / (e["htm-commits"]+e["rot-commits"]))
      ds.add_stack("Latency profile (update txs)", "Time (clock ticks)", {
        "tx proc.": lambda e: divByNumUpdTxs(e, "total-upd-tx-time"),
        "isolation wait": lambda e: divByNumUpdTxs(e, "total-sus-time"),
        "redo log flush": lambda e: divByNumUpdTxs(e, "total-flush-time"),
        "durability wait": lambda e: divByNumUpdTxs(e, "total-dur-commit-time")
      })

      # Adds a bar plot for the profile information.
      def divByNumROTxs(e, attr):
        if (e["read-commits"] == 0).any():
          return 0
        else:
          return (e[attr] / (e["read-commits"]))
      ds.add_stack("Latency profile (read-only txs)", "Time (clock ticks)", {
        "tx proc.": lambda e: divByNumROTxs(e, "total-ro-tx-time"),
        "durability wait": lambda e: divByNumROTxs(e, "total-ro-dur-wait-time")
      })
      
      # ds.add_stack("Commits vs Aborts", "Count", {
      #   "ROT-commits": lambda e: e["rot-commits"] if "rot-commits" in e else 0,
      #   "HTM-commits": lambda e: e["htm-commits"],
      #   "SGL-commits": lambda e: e["gl-commits"],
      #   "aborts": lambda e: e["total-aborts"]
      # }, filter_x_fn = filter_threads)


      # ds.add_stack("Abort types", "Nb. aborts", {
      #   "conflict-transactional": lambda e: e["confl-trans"],
      #   "conflict-non-transactional": lambda e: e["confl-non-trans"],
      #   "self": lambda e: e["confl-self"],
      #   "capacity": lambda e: e["capac-aborts"],
      #   "persistent": lambda e: e["persis-aborts"],
      #   "user": lambda e: e["user-aborts"],
      #   "other": lambda e: e["other-aborts"]
      # }, filter_x_fn = filter_threads)
      datasets_thr[(s,d,o,p,r)] += [ds]
    
  for u,v in datasets_thr.items():
    # print(u)
    # print(v)
    lines_plot = LinesPlot(f"[-s, -d, -o, -p, -r] = {u}", f"tpcc_{u}.pdf", figsize=(8, 4))
    lines_plot.plot(v)
    lines_plot.plot_stack(v)
