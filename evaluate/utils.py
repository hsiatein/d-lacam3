import subprocess
import re

def extract_total_cost(log:str):
    match = re.search(r'sum_of_costs:\s*(\d+)\s*\(lb=(\d+),\s*ub=([\d.]+)\)', log)
    sum_of_costs = float(match.group(1))
    lb = float(match.group(2))
    ub = sum_of_costs/lb
    return sum_of_costs,lb,ub

def visualize(map:str):
    visualize = [
        'mapf-visualizer',
        f'assets/{map}.map',
        f'./build/result.txt'
    ]
    result_visualize = subprocess.run(visualize, capture_output=True, text=True, check=True)
