import os
import subprocess

nb_samples = 2
thr_list = [2, 4, 8, 12, 16, 24, 32, 48, 64]
# For the plots in the paper we used the following:
# thr_list = [1, 2, 4, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56, 64]

# path where the backend is implemented
locations = [
  "../power8tm-pisces/benchmarks/tpcc",
  "../power8tm-pisces/benchmarks/tpcc",
  "../POWER8TM/benchmarks/tpcc",
  "../POWER8TM/benchmarks/tpcc",
  "../POWER8TM/benchmarks/tpcc",
  "../POWER8TM/benchmarks/tpcc",
]

# The backend name goes here (don't forget to match the position in the "backends" list with the position in the "locations" list)
backends = [
  "specpmtPLUStiny",
  "pisces",
  "psi",
  "psi-strong",
  "htm-sgl",
  "spht-log-linking"
]

# Label names in the plots
name_map = {
  "specpmtPLUStiny": "SpecPMT",
  "pisces" : "Pisces",
  "psi" : "FUR-SI",
  "psi-strong" : "FUR-opaq",
  "htm-sgl" : "HTM",
  "spht-log-linking" : "SPHT",
}

class RunnableBench:
  def run_benchmark(self, exec):
    pass

class BenchmarkParameters:
  def __init__(self, parameter_names:list[str]):
    self.params = {}
    self.non_comb_params = {}
    self.min_non_comb = float("+inf")
    for name in parameter_names:
      self.params[name] = []

  def set_params(self, name:str, params:list, non_comb=False):
    if name not in self.params:
      raise Exception(f"Parameter {name} not found")
    if non_comb:
      self.non_comb_params[name] = params
      self.min_non_comb = min(self.min_non_comb, len(params))
    else:
      self.params[name] = params

  def exec_for_each_param(self, exec_fn:RunnableBench, exec):
    params = list(self.params.keys())
    lst_p = self.list_for_each_param(params)

    for l_p in lst_p:
      args = exec
      for n,p in zip(params, l_p):
        args += f" {n} {p}"
      print(args)
      exec_fn.run_benchmark(args)

  def aux_list(self, idx_name, lst_param:list[str], args, lst_resp, non_comb = 0):
    if idx_name >= len(lst_param):
      lst_resp += [[a for a in args]]
      return
    name = lst_param[idx_name]
    if name in self.non_comb_params:
      self.aux_list(idx_name+1, lst_param, args + [self.non_comb_params[name][non_comb]], lst_resp, non_comb)
    else:
      for p in self.params[name]:
        self.aux_list(idx_name+1, lst_param, args + [p], lst_resp, non_comb)

  def list_for_each_param(self, lst_param:list[str]):
    res = []
    loop = 1
    if self.min_non_comb != float("+inf"):
      loop = self.min_non_comb
    for i in range(loop):
      self.aux_list(0, lst_param, [], res, i)
    return res

class CollectData(RunnableBench):
  def __init__(self, benchmark_location, benchmark_exec, build_script, backend_name, sample_output_folder = "data"):
    self.benchmark_location = benchmark_location
    self.benchmark_exec = benchmark_exec
    self.build_script = build_script
    self.backend_name = backend_name
    self.curr_dir = os.getcwd()
    self.sample_output_folder = f"{os.getcwd()}/{sample_output_folder}"
    os.system(f"mkdir -p {sample_output_folder}")

  def compile(self):
    os.chdir(self.benchmark_location)
    out = subprocess.run(
      f"./{self.build_script} {self.backend_name}".split(),
      stdout=subprocess.PIPE,
      text=True)
    print(out.stdout)
    print(out.stderr)
    print(f"return code == {out.returncode}")
    os.chdir(self.curr_dir)

  def run_benchmark(self, exec):
    attempts = 0
    out = None
    my_env = os.environ.copy()
    my_env["LD_LIBRARY_PATH"] = "/home/eurosys/FUR/power8tm-pisces/stms/specpmt/"
    while out == None or out.returncode != 0 and attempts < 3:
      out = subprocess.run(f"timeout 10m {exec}".split(), stdout=subprocess.PIPE, text=True, env=my_env)
      attempts += 1
      
    with open(f"{self.sample_output_folder}/{''.join(exec.split()).split('/')[-1]}", "w+") as f:
      f.write(out.stdout)

  def run_sample(self, parameters:BenchmarkParameters):
    self.compile()
    os.chdir(self.benchmark_location)
    parameters.exec_for_each_param(self, f"./{self.benchmark_exec}")
    os.chdir(self.curr_dir)