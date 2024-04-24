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
  params.set_params("-s", [0], True)
  params.set_params("-o", [100], True)
  params.set_params("-p", [0], True)
  params.set_params("-r", [0], True)
  params.set_params("-d", [0], True)

  # params.set_params("-s", [4, 8], True) # to pass more than 1 combination of values
  # params.set_params("-d", [4, 4], True)
  # params.set_params("-o", [4, 4], True)
  # params.set_params("-p", [43, 39], True)
  # params.set_params("-r", [45, 45], True)
  params.set_params("-n", [1, 4, 8, 16, 32, 64])
  #params.set_params("-n", [1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64])
  nb_samples = 1
  locations = [
    "../POWER8TM/benchmarks/tpcc",
    "../POWER8TM/benchmarks/tpcc",
    "../POWER8TM/benchmarks/tpcc",
    "../POWER8TM/benchmarks/tpcc",
    "../power8tm-pisces/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
    # "../POWER8TM/benchmarks/tpcc",
  ]
  # The backend name goes here (don't forget to match the position in the
  # "backends" list with the position in the "locations" list)
  backends = [
    "psi",
    "psi-strong",
    "spht-dumbo-readers",
    "spht",
    "pisces",
    # "htm-sgl",
    # "htm-sgl-sr",
    # "si-htm",
    # "ureads-strong",
    # "ureads-p8tm"
  ]
# Label names in the plots
  name_map = {
    "psi" : "DUMBO-SI",
    "psi-strong" : "DUMBO--opaq",
    "spht-dumbo-readers" : "DUMBO-read",
    "spht" : "SPHT",
    "spht-log-linking" : "SPHT-LL",
    "pisces" : "Pisces",
    "htm-sgl" : "HTM",
    "htm-sgl-sr" : "HTM+sus",
    "si-htm" : "SI-TM",
    "ureads-strong": "ureads-strong", 
    "ureads-p8tm": "ureads-p8tm"
  }
  
  
  data_folder = "data-tpcc-orderstatus"
 
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
          
      ds.add_stack("Prob. of different outcomes for a transaction", "Percentage of started transactions", {
          "non-tx commit": lambda e: (e["nontx-commits"])/(e["total-commits"]+e["total-aborts"]),
          "ROT commit": lambda e: (e["rot-commits"])/(e["total-commits"]+e["total-aborts"]),
          "HTM commit": lambda e: (e["htm-commits"])/(e["total-commits"]+e["total-aborts"]),
          "SGL commit": lambda e: (e["gl-commits"])/(e["total-commits"]+e["total-aborts"]),
          "STM commit": lambda e: (e["stm-commits"])/(e["total-commits"]+e["total-aborts"]),
          "Abort": lambda e: (e["total-aborts"])/(e["total-commits"]+e["total-aborts"]),
        }, is_percent=True)
    
      def divByAborts(e, attr):
        # if (e["total-aborts"] == 0).any():
        #   return 0
        # else:
        return (e[attr]/(e["total-aborts"]+e["total-commits"]-e["gl-commits"]))

                
      # Adds a bar plot for the abort type.          
      ds.add_stack("Abort types", "Percentage of aborts", {
        "tx conflict": lambda e: (divByAborts(e, "confl-trans") + divByAborts(e, "rot-trans-aborts")),
        "non-tx conflict": lambda e: (divByAborts(e, "confl-non-trans") + divByAborts(e, "rot-non-trans-aborts")),
        "capacity": lambda e: (divByAborts(e, "capac-aborts") + divByAborts(e, "rot-capac-aborts")), 
        "other": lambda e: (divByAborts(e, "other-aborts") + divByAborts(e, "confl-self") + divByAborts(e, "rot-self-aborts") + divByAborts(e, "user-aborts") + divByAborts(e, "rot-user-aborts")), 
      }, is_percent=True, fix_100=True)

      
      # Adds a bar plot for the profile information.
      def divByUpdTxtime(e, attr):
        if (e["total-upd-tx-time"] == 0).any():
          return 0
        else:
          return (e[attr])
      ds.add_stack("Latency profile (update txs)", "Time (clock ticks)", {
        "processing committed txs.": lambda e: divByUpdTxtime(e, "total-upd-tx-time"),
        "isolation wait": lambda e: divByUpdTxtime(e, "total-sus-time"),
        "redo log flush": lambda e: divByUpdTxtime(e, "total-flush-time"),
        "durability wait": lambda e: divByUpdTxtime(e, "total-dur-commit-time"),
        # TODO
        # "proc. aborted txs": lambda e: divByUpdTxtime(e, "total-abort-upd-tx-time")
      })

      # Adds a bar plot for the profile information.
      def divByROTxtime(e, attr):
        if (e["total-ro-tx-time"] == 0).any():
          return 0
        else:
          return (e[attr])
          # return (e[attr] / (e["total-ro-tx-time"]))
      ds.add_stack("Latency profile (read-only txs)", "Time (clock ticks)", {
        "proc. committed txs": lambda e: divByROTxtime(e, "total-ro-tx-time"),
        "durability wait": lambda e: divByROTxtime(e, "total-ro-dur-wait-time"),
        # TODO
        # "proc. aborted txs": lambda e: divByROTxtime(e, "total-abort-ro-tx-time")
      })
      
      datasets_thr[(s,d,o,p,r)] += [ds]

  colors = {
    "SGL commit" : "#a83232",
    "Abort": "#404040", 
    "DUMBO-opaq" : "#000066",
    "DUMBO-SI" : "#0000e6",
    "DUMBO-read" : "#9999ff",
    "Pisces" : "#77b300", 

  }
    
  for u,v in datasets_thr.items():
    # print(u)
    # print(v)
    lines_plot = LinesPlot(f"[-s, -d, -o, -p, -r] = {u}", f"tpcc_{u}.pdf", figsize=(8, 4), colors=colors)
    lines_plot.plot(v)
    lines_plot.plot_stack(v)
