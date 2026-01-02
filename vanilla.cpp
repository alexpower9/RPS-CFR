#include <iostream>
#include <unordered_map>
#include <vector>


std::unordered_map<int, double> regret_match(const std::unordered_map<int, double> &regrets) {
  std::unordered_map<int, double> regrets_matched;

  double sum = 0.0;
  for(const auto&[key, value]: regrets) {
    double val = (value > 0) ? value : 0.0;

    sum += val;
  }

  
  if (sum == 0.0) {
    for(const auto&[key, value]:regrets) {
      regrets_matched[key] += 1.0 / 3.0;
    }

    return regrets_matched;
  }


  for(const auto&[key, value]:regrets) {

    double val = (value > 0) ? value : 0.0;
    regrets_matched[key] += val / sum;
  }

  return regrets_matched;
  
} 

void accumulate_regrets(const std::unordered_map<int, double> &regret_sum, std::unordered_map<int, double> &strategy_sum) {  
  // first, grab the sum from the regret sum values 
  auto regrets_matched = regret_match(regret_sum);
  // now we can loop through and add the normalized val based on the correct key 
  for(const auto&[key, value]:regrets_matched) { 
    strategy_sum[key] += regrets_matched[key]; 
  } 
}

std::vector<double> get_ev_specific(const int payoff_matrix[3][3], std::unordered_map<int, double> &opponents_regret_sum) {
  // where the 0 index will be paper, 1 is rock, 2 is scissors
  std::vector<double> expected_values;

  auto regret_matched_sum = regret_match(opponents_regret_sum);

  for(int i = 0; i < 3; i++) {
    double ev = 0.0;
    for(int j = 0; j < 3; j++) {
      double regret = payoff_matrix[i][j];

      double opponents_regret = regret_matched_sum[j];

      ev += regret * opponents_regret;
    }  

    expected_values.push_back(ev);
  }

  return expected_values; 
}

double get_ev_current(const std::vector<double> &expected_values, std::unordered_map<int, double> &current_strategy) {
  double ev_current = 0.0;

  auto regret_matched_strategy = regret_match(current_strategy);

  for(int i = 0; i < 3; i++) {
    ev_current += expected_values[i] * current_strategy[i];
  }

  return ev_current; 
}

void update_regrets(double ev_current, const std::vector<double> &expected_values, std::unordered_map<int, double> &current_strategy) {
  for(int i = 0; i < 3; i++) {
    current_strategy[i] += expected_values[i] - ev_current; 
  } 
}

// this is just the vanilla CFR version as opposed to the mc variation
int main() {
  

  int payoff_matrix[3][3] = {{0 , -1, 1}, 
                            {1, 0, -1}, 
                            {-1, 1, 0}};

   
  // see how it goes with this number 
  int MAX_ITERS {10000};
  
  std::unordered_map<int, double> h_regret_sum = { {0, 1.0}, {1, 2.0} , {2, 3.0}};
  std::unordered_map<int, double> h_strategy_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};
  
  std::unordered_map<int, double> v_regret_sum = { {0, 3.0}, {1, 2.0} , {2, 1.0}};
  std::unordered_map<int, double> v_strategy_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};

  // instead of sampling one action, we see how we would do against the opponents strategy
  // std::unordered_map<int, double> test_sum = { {0, 0.4}, {1, 0.3} , {2, 0.3}};
  //
  // auto results = get_ev_specific(payoff_matrix, test_sum);

  for(int i = 0; i < MAX_ITERS; i++) {
    // so full process should be 
    // regret match, add to strat sum 
    // calculate evs specific and ev for current strategy 
    // calculate regrets 
    // update sum 

    
    accumulate_regrets(h_regret_sum, h_strategy_sum);
    accumulate_regrets(v_regret_sum, v_strategy_sum);

    auto heroes_evs = get_ev_specific(payoff_matrix, v_regret_sum);
    auto villains_evs = get_ev_specific(payoff_matrix, h_regret_sum);

    double heroes_current_ev = get_ev_current(heroes_evs, h_regret_sum);
    double villains_current_ev = get_ev_current(villains_evs, v_regret_sum);

    update_regrets(heroes_current_ev, heroes_evs, h_regret_sum);
    update_regrets(villains_current_ev, villains_evs, v_regret_sum);
     
  }

  for(const auto&[key, value]: regret_match(v_strategy_sum)) {
    std::cout << "value: " << value << "\n";
  }

}
