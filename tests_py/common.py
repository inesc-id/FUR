import os
import subprocess

class RunnableBench:
  def run_benchmark(self, exec):
    pass

class BenchmarkParameters:
  def __init__(self, parameter_names:list[str]):
    self.params = {}
    for name in parameter_names:
      self.params[name] = []

  def set_params(self, name:str, params:list):
    if name not in self.params:
      raise Exception(f"Parameter {name} not found")
    self.params[name] = params

  def aux_exec(self, idx_name, exec_fn:RunnableBench, args):
    keys = list(self.params.keys())
    if idx_name >= len(keys):
      exec_fn.run_benchmark(args)
      return
    name = keys[idx_name]
    for p in self.params[name]:
      self.aux_exec(idx_name+1, exec_fn, args + f" {name} {p} ")

  def exec_for_each_param(self, exec_fn:RunnableBench, args):
    self.aux_exec(0, exec_fn, args)

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
    out = subprocess.run(f"{exec}".split(), stdout=subprocess.PIPE, text=True)
    with open(f"{self.sample_output_folder}/{''.join(exec.split()).split('/')[-1]}", "w+") as f:
      f.write(out.stdout)

  def run_sample(self, parameters:BenchmarkParameters):
    self.compile()
    os.chdir(self.benchmark_location)
    parameters.exec_for_each_param(self, f"./{self.benchmark_exec}")
    os.chdir(self.curr_dir)