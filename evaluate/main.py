import subprocess
import utils
import matplotlib.pyplot as plt
import numpy as np
import random

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
         13:["w_woundedcoast",1000],
         14:["Berlin_1_256",1000],
         15:["brc202d",1000],
         16:["den312d",400],
         17:["maze-32-32-4",100],
         18:["maze-128-128-2",300],
         19:["orz900d",1000],
         20:["warehouse-20-40-10-2-1",1000],
         21:["warehouse-20-40-10-2-2",1000]}

Candidates=[8,10,11,12,13,14,15,16,17,18,19,20,21]
#Candidates=[12]
TieBreakers=['0','1','2','3','4']



TieBreakers=['0','4']
selected=10
num=460
seeds_num=10
average_ubs=np.array([0 for _ in range(len(TieBreakers))])
fail={'0':0,'4':0}
for _ in range(seeds_num):
    seed=random.randint(0,100000000)
    print(mapDict[selected][0])
    print(num)
    print(seed)
    costs=[]
    lbs=[]
    ubs=[]
    for tiebreaker in TieBreakers:
        # 构建命令
        compute = [
            'build/main',
            '-i', f'assets/{mapDict[selected][0]}.scen',
            '-m', f'assets/{mapDict[selected][0]}.map',
            '-N', str(num),
            '-v', '3',
            '-t', '5',
            '--no-star',
            '--tie-breaker',tiebreaker,
            '-s',str(seed),
        ]
        try:
            # 运行命令
            result_compute = subprocess.run(compute, capture_output=True, text=True, check=True)
            # 打印输出
            print("标准输出:")
            print(result_compute.stdout)
            print("标准错误:")
            print(result_compute.stderr)
            cost,lb,ub=utils.extract_total_cost(result_compute.stdout)
            costs.append(cost)
            lbs.append(lb)
            ubs.append(ub)
            if ub<1:
                fail[tiebreaker]=fail[tiebreaker]+1
        except subprocess.CalledProcessError as e:
            print(f"命令执行失败: {e}")
            print(f"返回码: {e.returncode}")
            print(f"标准错误: {e.stderr}")
            break
    if len(ubs)==0:
        break
    average_ubs=average_ubs+np.array(ubs)
    title=f'{mapDict[selected][0]} , num = {num} , seed = {seed}'
    utils.plot_ub(ubs,title)
average_ubs[0]=average_ubs[0]/(seeds_num-fail["0"])
average_ubs[1]=average_ubs[1]/(seeds_num-fail["4"])
utils.plot_ub(average_ubs,f'{mapDict[selected][0]} , num = {num} , seed = average({seeds_num})',save_fig=True)


# for selected in Candidates:
#     print(mapDict[selected][0])
#     print(mapDict[selected][1])
#     costs=[]
#     lbs=[]
#     ubs=[]
#     for tiebreaker in TieBreakers:
#         # 构建命令
#         compute = [
#             'build/main',
#             '-i', f'assets/{mapDict[selected][0]}.scen',
#             '-m', f'assets/{mapDict[selected][0]}.map',
#             '-N', str(mapDict[selected][1]),
#             '-v', '3',
#             '-t', '5',
#             '--no-star',
#             '--tie-breaker',tiebreaker,
#         ]
#         try:
#             # 运行命令
#             result_compute = subprocess.run(compute, capture_output=True, text=True, check=True)
#             # 打印输出
#             print("标准输出:")
#             print(result_compute.stdout)
#             print("标准错误:")
#             print(result_compute.stderr)
#             cost,lb,ub=utils.extract_total_cost(result_compute.stdout)
#             costs.append(cost)
#             lbs.append(lb)
#             ubs.append(ub)
#         except subprocess.CalledProcessError as e:
#             print(f"命令执行失败: {e}")
#             print(f"返回码: {e.returncode}")
#             print(f"标准错误: {e.stderr}")
#     x = list(range(len(ubs)))
#     # 绘制柱状图
#     plt.bar(x, ubs)
#     # 添加标题和标签
#     title=f'{mapDict[selected][0]} , {mapDict[selected][1]}'
#     plt.title(title)
#     plt.xlabel('tie breaker')
#     plt.ylabel('ub')
#     plt.ylim(bottom=1)
#     for i in range(len(ubs)):
#         plt.text(i, ubs[i]+0.01, str(round(ubs[i],4)), ha='center')
#     plt.savefig(f'evaluate/fig/{title}.png')
#     plt.clf()
#     # 显示图形
#     #plt.show()
    
# selected=10
# nums=[50*i for i in range(1,11)]    
# for num in nums:
#     print(mapDict[selected][0])
#     print(num)
#     costs=[]
#     lbs=[]
#     ubs=[]
#     for tiebreaker in TieBreakers:
#         # 构建命令
#         compute = [
#             'build/main',
#             '-i', f'assets/{mapDict[selected][0]}.scen',
#             '-m', f'assets/{mapDict[selected][0]}.map',
#             '-N', str(num),
#             '-v', '3',
#             '-t', '5',
#             '--no-star',
#             '--tie-breaker',tiebreaker,
#         ]
#         try:
#             # 运行命令
#             result_compute = subprocess.run(compute, capture_output=True, text=True, check=True)
#             # 打印输出
#             print("标准输出:")
#             print(result_compute.stdout)
#             print("标准错误:")
#             print(result_compute.stderr)
#             cost,lb,ub=utils.extract_total_cost(result_compute.stdout)
#             costs.append(cost)
#             lbs.append(lb)
#             ubs.append(ub)
#         except subprocess.CalledProcessError as e:
#             print(f"命令执行失败: {e}")
#             print(f"返回码: {e.returncode}")
#             print(f"标准错误: {e.stderr}")
#             break
#     if len(ubs)==0:
#         break
#     x = list(range(len(ubs)))
#     # 绘制柱状图
#     plt.bar(x, ubs)
#     # 添加标题和标签
#     title=f'{mapDict[selected][0]} , {num}'
#     plt.title(title)
#     plt.xlabel('tie breaker')
#     plt.ylabel('ub')
#     plt.ylim(bottom=1)
#     for i in range(len(ubs)):
#         plt.text(i, ubs[i]+0.01, str(round(ubs[i],4)), ha='center')
#     plt.savefig(f'evaluate/fig2/{title}.png')
#     plt.clf()
#     # 显示图形
#     #plt.show()