import subprocess

mapDict={0:["corners",4],
         1:["corners2",5],
         2:["loop-chain",7],
         3:["connector",6],
         4:["string",5],
         5:["tree",3],
         6:["tree2",17],
         7:["tunnel",4],
         8:["lak303d",1000],
         9:["random-32-32-10-2",485],
         10:["random-32-32-10",460],
         11:["maze-128-128-1",350],
         12:["room-64-64-8",1000],
         13:["w_woundedcoast",1000]}

selected=12

# 构建命令
compute = [
    'build/main',
    '-i', f'assets/{mapDict[selected][0]}.scen',
    '-m', f'assets/{mapDict[selected][0]}.map',
    '-N', str(mapDict[selected][1]),
    '-v', '3',
    '--no-star'
]
visualize = [
    'mapf-visualizer',
    f'assets/{mapDict[selected][0]}.map',
    f'./build/result.txt'
]

try:
    # 运行命令
    result_compute = subprocess.run(compute, capture_output=True, text=True, check=True)

    # 打印输出
    print("标准输出:")
    print(result_compute.stdout)
    print("标准错误:")
    print(result_compute.stderr)
    result_visualize = subprocess.run(visualize, capture_output=True, text=True, check=True)
    
    
except subprocess.CalledProcessError as e:
    print(f"命令执行失败: {e}")
    print(f"返回码: {e.returncode}")
    print(f"标准错误: {e.stderr}")