#include <iostream>
#include <unordered_map>
#include <unordered_map>
#include <vector>
#include <random>
#include <algorithm>

// draw prportionally for this distrubtion and return 0, 1, 2
int get_move(std::unordered_map<int, double> &regret_sum) {
  // first, we need to convert normalize
  std::vector<int> keys;
  std::vector<double> values;
  double sum = 0.0;

  for(const auto& [key, value]: regret_sum) {

    // check if the value is negative, in which cause just multiply by -1
    keys.push_back(key);
    
    int new_val = (value > 0) ? value : 0.0;
    values.push_back(new_val);
    sum += new_val;
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::discrete_distribution<> d(values.begin(), values.end());

  if(sum == 0.0) {
    std::uniform_int_distribution<> uniform_dist(0, keys.size() - 1);
    return keys[uniform_dist(gen)];
  } 

  int index = d(gen);
  return keys[index];
}

// this is non optimal, but we will fix later
void accumulate_regrets(const std::unordered_map<int, double> &regret_sum, std::unordered_map<int, double> &strategy_sum) {
  
  // first, grab the sum from the regret sum values 
  double sum = 0.0;

  for(const auto&[key, value]: regret_sum) {
    double val = (value > 0) ? value : 0.0;

    sum += val;
  }
  
  // if all are 0, then just add a third to each 
  if (sum == 0.0) {
    for(const auto&[key, value]:regret_sum) {
      strategy_sum[key] += 1.0 / 3.0;
    }

    return;
  }

  // now we can loop through and add the normalized val based on the correct key 
  for(const auto&[key, value]:regret_sum) { 
    double val = (value > 0) ? value : 0.0;
    strategy_sum[key] += val/ sum; 
  } 
}

void update_regret_sum(int hero_move, int villain_move, std::unordered_map<int, double> &regret_sum, const int payoff_matrix[3][3]) {
  // recall that regret = what we could have gotten MINUS what we got  

  int what_we_got = payoff_matrix[hero_move][villain_move];

  for(int i = 0; i < 3; i++) {
    int what_we_could_have = payoff_matrix[i][villain_move];
    int val_to_add = what_we_could_have - what_we_got;
    regret_sum[i] += val_to_add; 
  }
}

int main() {
  
  // so row = our move, col = villain move, and indexing by this gives us the payout we expect in rock, paper, scissors 
  int payoff_matrix[3][3] = {{0 , -1, 1}, 
                            {1, 0, -1}, 
                            {-1, 1, 0}};

   
  // see how it goes with this number 
  int MAX_ITERS {1000000};


  // we also need to define our strategy sum and regret sum dicts as unordered_maps
  // for both the hero and the villain
  std::unordered_map<int, double> h_regret_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};
  std::unordered_map<int, double> h_strategy_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};
  
  std::unordered_map<int, double> v_regret_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};
  std::unordered_map<int, double> v_strategy_sum = { {0, 0.0}, {1, 0.0} , {2, 0.0}};


  for(int i = 0; i < MAX_ITERS; i++) {
    int hero_move = get_move(h_regret_sum);
    int villain_move = get_move(v_regret_sum);


    accumulate_regrets(h_regret_sum, h_strategy_sum);
    accumulate_regrets(v_regret_sum, v_strategy_sum);

    // now we need to calculate regrets for both players
    int hero_regret = payoff_matrix[hero_move][villain_move];
    int villain_regret = payoff_matrix[villain_move][hero_move];
    
    // now just update regret sums 
    update_regret_sum(hero_move, villain_move, h_regret_sum, payoff_matrix); 
    update_regret_sum(villain_move, hero_move, v_regret_sum, payoff_matrix); 
  } 
  
  std::cout << "Final Strat:\n";

  for(const auto&[key, value]: h_strategy_sum) {
    std::cout << "Key: " << key << " Val: " << value / MAX_ITERS << "\n";
  }
  
}
