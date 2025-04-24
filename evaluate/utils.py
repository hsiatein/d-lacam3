import subprocess
import re
import matplotlib.pyplot as plt

def extract_total_cost(log:str):
    match = re.search(r'sum_of_costs:\s*(\d+)\s*\(lb=(\d+),\s*ub=([\d.]+)\)', log)
    sum_of_costs = float(match.group(1))
    lb = float(match.group(2))
    ub = 0
    if lb!=0:
        ub = sum_of_costs/lb
    return sum_of_costs,lb,ub

def visualize(map:str):
    visualize = [
        'mapf-visualizer',
        f'assets/{map}.map',
        f'./build/result.txt'
    ]
    result_visualize = subprocess.run(visualize, capture_output=True, text=True, check=True)
    
def plot_ub(ubs,title,save_fig=False,show_fig=False):
    x = list(range(len(ubs)))
    # 绘制柱状图
    plt.bar(x, ubs)
    # 添加标题和标签
    plt.title(title)
    plt.xlabel('tie breaker')
    plt.ylabel('ub')
    plt.ylim(bottom=1)
    for i in range(len(ubs)):
        plt.text(i, ubs[i]+0.01, str(round(ubs[i],4)), ha='center')
    if save_fig:
        plt.savefig(f'evaluate/fig4/{title}.png')
    # 显示图形
    if show_fig:
        plt.show()
    plt.clf()

