import subprocess
import re
import matplotlib.pyplot as plt
import pandas as pd

def extract_total_cost(log:str):
    match = re.search(r'sum_of_costs:\s*(\d+)\s*\(lb=(\d+),\s*ub=([\d.]+)\)', log)
    sum_of_costs = float(match.group(1))
    lb = float(match.group(2))
    ub = 0
    if lb!=0:
        ub = sum_of_costs/lb
    return sum_of_costs,lb,ub

def extract_runtime(log:str):
    regex=""
    if("failed to solve" in log):
        regex = r"elapsed:\s*(\d+)ms\s*failed\sto\ssolve"
    else:
        regex = r"elapsed:\s*(\d+)ms\s*found\sinitial\ssolution"
    match = re.search(regex, log)
    runtime = float(match.group(1))
    return runtime

def extract_runtime(log:str):
    match = re.search(r"elapsed:\s*(\d+)ms\s*found\sinitial\ssolution", log)
    runtime = float(match.group(1))
    return runtime


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
        plt.savefig(f'evaluate/fig5/{title}.png')
    # 显示图形
    if show_fig:
        plt.show()
    plt.clf()

def check_and_add_instance(df, instance_name):
    # 检查是否存在给定的 instance
    if instance_name not in df['instance'].values:
        # 创建一个新的行，所有值初始化为 0
        new_row = {col: 0 for col in df.columns}
        new_row['instance'] = instance_name  # 设置 instance 列的值
        
        # 使用 pd.concat 添加新行
        df = pd.concat([df, pd.DataFrame([new_row])], ignore_index=True)
    return df

def df_add_num(df, instance_name, col_name, num):
    # 检查列是否存在
    if col_name in df.columns:
        # 找到对应的行并增加数值
        df.loc[df['instance'] == instance_name, col_name] += num
    else:
        print(f"Column '{col_name}' does not exist.")
    return df

def df_set_num(df, instance_name, col_name, num):
    # 检查列是否存在
    if col_name in df.columns:
        # 找到对应的行并增加数值
        df.loc[df['instance'] == instance_name, col_name] = num
    else:
        print(f"Column '{col_name}' does not exist.")
    return df