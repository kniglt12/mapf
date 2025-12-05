#ifndef RHCR_WINDOW_H
#define RHCR_WINDOW_H

#include "Types.h"
#include "SharedEnv.h"
#include <vector>
#include <unordered_map>
#include <iostream>

namespace DefaultPlanner {

    // 滚动视界消解机制 (RHCR)
    // 将长远期规划截断至特定窗口(Window)中，仅在窗口内严格消解冲突
    class RHCR_Framework {
    public:
        int window_size; // w，严格无冲突时间窗口
        int horizon;     // h，重规划周期

        std::vector<int> last_replan_timestep;

        RHCR_Framework(int w = 10, int h = 5) : window_size(w), horizon(h) {}

        void init(int num_agents) {
            last_replan_timestep.assign(num_agents, -horizon); 
        }

        // 检查是否需要加入重规划批次
        bool requireReplan(int agent_id, int current_timestep) {
            if (current_timestep - last_replan_timestep[agent_id] >= horizon) {
                return true;
            }
            return false;
        }

        // 为批次生成窗口约束
        void buildWindowConstraints(int current_timestep, const std::vector<int>& replan_agents, 
                                    std::vector<std::vector<int>>& dynamic_obstacles) {
            // TODO: integrate with obstacle tables
            // for (int id : unreplanned_agents) {
            //     for (int t = 0; t < window_size; ++t) {
            //         int loc = getAgentFutureLocation(id, current_timestep + t);
            //         dynamic_obstacles[t].push_back(loc);
            //     }
            // }
        }

        void markReplanned(int agent_id, int current_timestep) {
            last_replan_timestep[agent_id] = current_timestep;
        }

        // 死锁避免机制，发生局部死锁时扩大窗口
        void adjustWindowForDeadlock(int deadlock_agent_id) {
            // std::cout << "[RHCR] Deadlock for agent " << deadlock_agent_id << ", increasing window.\n";
            window_size += 5; 
        }

        void step(int current_timestep, int num_agents) {
            std::vector<int> replan_batch;

            for (int i = 0; i < num_agents; ++i) {
                if (requireReplan(i, current_timestep)) {
                    replan_batch.push_back(i);
                }
            }

            if (!replan_batch.empty()) {
                std::vector<std::vector<int>> window_constraints;
                buildWindowConstraints(current_timestep, replan_batch, window_constraints);

                for (int id : replan_batch) {
                    markReplanned(id, current_timestep);
                }
            }
        }
    };

}

#endif // RHCR_WINDOW_H
