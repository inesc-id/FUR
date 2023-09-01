#!/bin/env python3

import os
import subprocess

class RunnableBench:
  def run_benchmark(self, exec):
    pass

class BenchmarkParameters:
  def __init__(self, parameter_names:list(str)):
    self.parameter_names = parameter_names

  def set_params(self, name:str, params:list):
    if name not in self.parameter_names:
      raise Exception("Parameter not found")
    self.parameter_names[name] = params

  def aux_exec(self, idx_name, exec_fn:RunnableBench, args):
    if idx_name >= len(self.parameter_names):
      exec_fn.run_benchmark(args)
    name = self.parameter_names[idx_name]
    for p in self.parameter_names[name]:
      self.aux_exec(idx_name+1, exec_fn, args + f" {name} {p} ")

  def exec_for_each_param(self, exec_fn:RunnableBench, args):
    self.aux_exec(0, exec_fn, args)

class CollectData(RunnableBench):
  def __init__(self, benchmark_location, benchmark_exec, build_script, backend_name, sample_output_folder = "data"):
    self.benchmark_location = benchmark_location
    self.benchmark_exec = benchmark_exec
    self.build_script = build_script
    self.backend_name = backend_name
    self.sample_output_folder = sample_output_folder
    os.system(f"mkdir -fp {sample_output_folder}")

  def compile(self):
    os.chdir(self.benchmark_location)
    out = subprocess.run(
      f"./{self.build_script} {self.backend_name}".split(),
      stdout=subprocess.PIPE,
      text=True)
    print(out.stdout)
    print(out.stderr)
    print(f"return code == {out.returncode}")

  def run_benchmark(self, exec):
    out = subprocess.run(f"{exec}".split(), stdout=subprocess.PIPE, text=True)
    with open(f"{self.sample_output_folder}/s1") as f:
      f.write(out.stdout)

  def run_sample(self, parameters:BenchmarkParameters):
    self.compile()
    parameters.exec_for_each_param(self, f"./{self.benchmark_exec}")
    
if __name__ == "__main__":
  params = BenchmarkParameters(["-u", "-d", "-i", ""])
  params.set_params("-u", [10, 90])
  params.set_params("-d", [5000])
  params.set_params("-i", [32768])
  params.set_params("-n", [1, 2, 4, 8, 12, 16])

  for s in range(5):
    data = CollectData(
        "~/power8tm-pisces/benchmarks/datastructures", # benchmark_location
        "hashmap/hashmap",
        "build-datastructures.sh",
        "pisces",
        f"data-pisces-s{s}"
      )
    data.run_sample()
