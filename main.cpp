#include <argparse/argparse.hpp>
#include <iostream>
#include <lacam.hpp>

int main(int argc, char *argv[])
{
  // arguments parser
  argparse::ArgumentParser program("lacam3", "0.1.0");
  program.add_argument("-m", "--map").help("map file").required();
  program.add_argument("-i", "--scen")
      .help("scenario file")
      .default_value(std::string(""));
  program.add_argument("-N", "--num").help("number of agents").required();
  program.add_argument("-s", "--seed")
      .help("seed")
      .default_value(std::string("0"));
  program.add_argument("-v", "--verbose")
      .help("verbose")
      .default_value(std::string("0"));
  program.add_argument("-t", "--time_limit_sec")
      .help("time limit sec")
      .default_value(std::string("3"));
  program.add_argument("-o", "--output")
      .help("output file")
      .default_value(std::string("./log/result.txt"));
  program.add_argument("-l", "--log_short")
      .default_value(false)
      .implicit_value(true);

  // solver parameters
  program.add_argument("--no-all")
      .help("turn off all options, i.e., vanilla LaCAM")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--no-star")
      .help("turn off the anytime part, i.e., usual LaCAM")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--random-insert-prob1")
      .help("probability of inserting the start node")
      .default_value(std::string("0.001"));
  program.add_argument("--random-insert-prob2")
      .help("probability of inserting a node after finding the goal")
      .default_value(std::string("0.01"));
  program.add_argument("--random-insert-init-node")
      .help("insert start node instead of random ones")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--no-swap")
      .help("turn off swap operation in PIBT")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--no-multi-thread")
      .help("turn off multi-threading")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--pibt-num")
      .help("used in Monte-Carlo configuration generation")
      .default_value(std::string("10"));
  program.add_argument("--no-scatter")
      .help("turn off SUO")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--scatter-margin")
      .help("allowing non-shortest paths in SUO")
      .default_value(std::string("10"));
  program.add_argument("--no-refiner")
      .help("turn off iterative refinement")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--refiner-num")
      .help("specify the number of refiners")
      .default_value(std::string("4"));
  program.add_argument("--recursive-rate")
      .help("specify the rate of the recursive call of LaCAM")
      .default_value(std::string("0.2"));
  program.add_argument("--recursive-time-limit")
      .help("time limit (sec) of the recursive call")
      .default_value(std::string("1"));
  program.add_argument("--checkpoints-duration")
      .help("for recording")
      .default_value(std::string("5"));

  // new solver parameters 
  program.add_argument("--tie-breaker")
      .help("pibt with new tie-breaker\n0.vanilla PIBT\n1.当前没有被占用优先\n2.下一步没有被占用优先\n3.当前与下一步都没有被占用优先\n4.当前与下一步都没有被占用，且与想要占用自身位置的智能体方向不同优先")
      .default_value(std::string("0"));
  program.add_argument("--check-feasibility")
      .help("check feasibility of solution")
      .default_value(false)
      .implicit_value(true);
  program.add_argument("--runtime-log")
      .help("runtime log filename")
      .default_value(std::string("./log/log.txt"));
  program.add_argument("--runtime-log-verbose")
      .help("verbose about runtime log")
      .default_value(std::string("3"));
  program.add_argument("--cut-constraint")
      .help("level of cut invalid constraint, 0表示不剪枝, 1表示只剪枝约束直接导致的冲突, 2表示在1基础上剪枝一步间接导致的冲突")
      .default_value(std::string("0"));
  program.add_argument("--undirect-edge")
      .help("when a hnode explored, build a undirect edge between from and to")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_known_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  // setup instance
  const auto verbose = std::stoi(program.get<std::string>("verbose"));
  const auto time_limit_sec =
      std::stoi(program.get<std::string>("time_limit_sec"));
  const auto scen_name = program.get<std::string>("scen");
  const auto seed = std::stoi(program.get<std::string>("seed"));
  const auto map_name = program.get<std::string>("map");
  const auto output_name = program.get<std::string>("output");
  const auto log_short = program.get<bool>("log_short");
  const auto N = std::stoi(program.get<std::string>("num"));
  const auto ins = scen_name.size() > 0 ? Instance(scen_name, map_name, N)
                                        : Instance(map_name, N, seed);
  if (!ins.is_valid(1)) return 1;

  // solver parameters
  const auto flg_no_all = program.get<bool>("no-all");
  Planner::FLG_SWAP = !program.get<bool>("no-swap") && !flg_no_all;
  Planner::FLG_STAR = !program.get<bool>("no-star") && !flg_no_all;
  Planner::FLG_MULTI_THREAD =
      !program.get<bool>("no-multi-thread") && !flg_no_all;
  Planner::PIBT_NUM =
      flg_no_all ? 1 : std::stoi(program.get<std::string>("pibt-num"));
  Planner::FLG_REFINER = !program.get<bool>("no-refiner") && !flg_no_all;
  Planner::REFINER_NUM = std::stoi(program.get<std::string>("refiner-num"));
  Planner::FLG_SCATTER = !program.get<bool>("no-scatter") && !flg_no_all;
  Planner::SCATTER_MARGIN =
      std::stoi(program.get<std::string>("scatter-margin"));
  Planner::RANDOM_INSERT_PROB1 =
      flg_no_all ? 0
                 : std::stof(program.get<std::string>("random-insert-prob1"));
  Planner::RANDOM_INSERT_PROB2 =
      flg_no_all ? 0
                 : std::stof(program.get<std::string>("random-insert-prob2"));
  Planner::FLG_RANDOM_INSERT_INIT_NODE =
      program.get<bool>("random-insert-init-node") && !flg_no_all;
  Planner::RECURSIVE_RATE =
      flg_no_all ? 0 : std::stof(program.get<std::string>("recursive-rate"));
  Planner::RECURSIVE_TIME_LIMIT =
      flg_no_all
          ? 0
          : std::stof(program.get<std::string>("recursive-time-limit")) * 1000;
  Planner::CHECKPOINTS_DURATION =
      std::stof(program.get<std::string>("checkpoints-duration")) * 1000;

  // parse new parameters
  PIBT::TIE_BREAKER=std::stoi(program.get<std::string>("tie-breaker"));
  const auto runtime_log_name = program.get<std::string>("runtime-log");
  runtime_log_verbose = std::stoi(program.get<std::string>("runtime-log-verbose"));
  const auto check_feasibility = program.get<bool>("check-feasibility");
  cut_constraint_level = std::stoi(program.get<std::string>("cut-constraint"));
  flg_undirect_edge=program.get<bool>("undirect-edge");


  runtime_log_stream.open(runtime_log_name, std::ios::out);

  // solve
  const auto deadline = Deadline(time_limit_sec * 1000);
  const auto solution = solve(ins, verbose - 1, &deadline, seed);
  const auto comp_time_ms = deadline.elapsed_ms();

  // failure
  if (solution.empty()) info(1, verbose, &deadline, "failed to solve");

  if(check_feasibility){
    info(3, verbose, &deadline, "start check feasibility");
    // check feasibility
    if (!is_feasible_solution(ins, solution, verbose)) {
    info(0, verbose, &deadline, "invalid solution");
    return 1;
    }
    info(3, verbose, &deadline, "complete check feasibility");
  }
  else info(3, verbose, &deadline, "don't check feasibility");
  // post processing
  print_stats(verbose, &deadline, ins, solution, comp_time_ms);
  make_log(ins, solution, output_name, comp_time_ms, map_name, seed, log_short);
  info(3, verbose, &deadline, "complete log");
  runtime_log_stream.close();
  return 0;
}
