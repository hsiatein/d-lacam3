#map="corners"&&N=4
#map="corners2"&&N=5
#map=loop-chain&&N=7
#map=connector&&N=6
#map=string&&N=5
#map=tree&&N=3
#map=tree2&&N=17
#map=tunnel&&N=4
#map=lak303d&&N=1000
#map=random-32-32-10-2&&N=485
#map=random-32-32-10&&N=460
map=maze-128-128-1&&N=400
#map=room-64-64-8&&N=1000
#map=w_woundedcoast&&N=1000
#map=warehouse-20-40-10-2-2&&N=10000
#map=den312d&&N=1000

# seed0=210
# for ((seed = $seed0; seed < $seed0 + 1; seed++)); do
#     echo $seed
#     #build/main -i assets/maze-128-128-1.scen -m assets/maze-128-128-1.map -N 350 -v 3 --tie-breaker 0 -s $seed -t 5 --no-star --check-feasibility --runtime-log-verbose 3 --cut-constraint
#     echo "cut"
#     build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --tie-breaker 0 -s $seed -t 10 -o log/result.txt --no-scatter #--no-star --cut-constraint
#     echo "don't cut"
#     build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --tie-breaker 0 -s $seed -t 10 -o log/result.txt --no-scatter #--no-star
# done

#build/main -i assets/maze-128-128-1.scen -m assets/maze-128-128-1.map -N 450 -v 3 --tie-breaker 4 -s 0 -t 10 --cut-constraint #--no-star --check-feasibility --runtime-log-verbose 3
seed=17 #72804709
build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --tie-breaker 0 -s $seed -t 5 -o log/result.txt --no-scatter --cut-constraint --no-star
build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --tie-breaker 0 -s $seed -t 5 -o log/result.txt --no-scatter --no-star
#mapf-visualizer assets/$map.map log/result.txt