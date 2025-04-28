#map="corners"&&N=4
#map="corners2"&&N=5
map=loop-chain&&N=7
#map=connector&&N=6
#map=string&&N=5
#map=tree&&N=3
#map=tree2&&N=17
#map=tunnel&&N=4
#map=lak303d&&N=1000
#map=random-32-32-10-2&&N=485
#map=random-32-32-10&&N=460
#map=maze-128-128-1&&N=400
#map=room-64-64-8&&N=1000
#map=w_woundedcoast&&N=1000
#map=warehouse-20-40-10-2-2&&N=1000

# seed0=20
# for ((seed = $seed0; seed < $seed0 + 10; seed++)); do
#     echo $seed
#     build/main -i assets/maze-128-128-1.scen -m assets/maze-128-128-1.map -N 350 -v 3 --tie-breaker 0 -s $seed -t 5 --no-star --check-feasibility
# done

#build/main -i assets/maze-128-128-1.scen -m assets/maze-128-128-1.map -N 350 -v 3 --tie-breaker 0 -s 24 -t 5 --no-star --check-feasibility
build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --tie-breaker 0 -s 2 -t 10 -o log/result.txt --no-scatter --check-feasibility --no-star
mapf-visualizer assets/$map.map log/result.txt