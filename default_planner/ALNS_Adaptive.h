#ifndef ALNS_ADAPTIVE_H
#define ALNS_ADAPTIVE_H

#include <vector>
#include <random>
#include <numeric>
#include <stdexcept>
#include <iostream>

namespace DefaultPlanner {

    // ALNS 轮盘赌自适应机制
    // 根据运行过程中的破坏和修复效果，动态调整三种正交算子的选中概率
    class ALNSAdaptiveMechanim {
    public:
        enum DestroyHeuristicType {
            AGENT_BASED = 0, 
            MAP_BASED = 1,   
            RANDOM = 2       
        };

        static const int NUM_HEURISTICS = 3;

        ALNSAdaptiveMechanim(double reaction_factor = 0.1, double min_weight = 0.05) 
            : r_factor(reaction_factor), min_w(min_weight) {
            
            weights.resize(NUM_HEURISTICS, 1.0);
            probabilities.resize(NUM_HEURISTICS, 1.0 / NUM_HEURISTICS);
            scores.resize(NUM_HEURISTICS, 0.0);
            usage_counts.resize(NUM_HEURISTICS, 0);

            std::random_device rd;
            rng.seed(rd());
        }

        // 轮盘赌选择下一个破坏算子
        DestroyHeuristicType selectHeuristic() {
            std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());
            int selected = distribution(rng);
            return static_cast<DestroyHeuristicType>(selected);
        }

        // 迭代结束更新得分
        void updateScore(DestroyHeuristicType type, double score_increment) {
            scores[type] += score_increment;
            usage_counts[type]++;
        }

        // 定期更新权重与轮盘概率
        void updateWeightsAndProbabilities() {
            double total_prob = 0.0;
            
            for (int i = 0; i < NUM_HEURISTICS; ++i) {
                if (usage_counts[i] > 0) {
                    double average_score = scores[i] / usage_counts[i];
                    weights[i] = (1.0 - r_factor) * weights[i] + r_factor * average_score;
                }
                
                if (weights[i] < min_w) {
                    weights[i] = min_w;
                }
                
                total_prob += weights[i];
            }

            for (int i = 0; i < NUM_HEURISTICS; ++i) {
                probabilities[i] = weights[i] / total_prob;
                scores[i] = 0.0;
                usage_counts[i] = 0;
            }
        }

        void printStatus() const {
            std::cout << "--- ALNS Probabilities ---\n";
            std::cout << "Agent-Based: " << probabilities[AGENT_BASED] << "\n";
            std::cout << "Map-Based: " << probabilities[MAP_BASED] << "\n";
            std::cout << "Random: " << probabilities[RANDOM] << "\n";
            std::cout << "--------------------------\n";
        }

    private:
        std::vector<double> weights;
        std::vector<double> probabilities;  
        std::vector<double> scores;         
        std::vector<int> usage_counts;      

        double r_factor; 
        double min_w;    

        std::mt19937 rng;
    };

}

#endif // ALNS_ADAPTIVE_H
