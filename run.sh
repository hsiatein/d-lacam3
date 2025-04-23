#map="corners"&&N=4
#map="corners2"&&N=5
#map=loop-chain&&N=7
#map=connector&&N=6
#map=string&&N=5
#map=tree&&N=3
#map=tree2&&N=17
#map=tunnel&&N=4
#map=lak303d&&N=1000
#map=random-32-32-10&&N=485
#map=maze-128-128-1&&N=350
map=room-64-64-8&&N=1000

build/main -i assets/$map.scen -m assets/$map.map -N $N -v 3 --no-star
mapf-visualizer assets/$map.map ./build/result.txt