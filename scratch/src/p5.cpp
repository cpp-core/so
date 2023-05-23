// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;


class Node
{
    public:
        Node() {
            position = vector<double> {0.0, 0.0, 0.0};
            parent = NULL;
        }

        Node(vector<double> positionParam, Node* parentParam) {
            position = positionParam;
            parent = parentParam;
        }

        vector<double> position;
        Node* parent;  // Null if its the root
};


// Pepper plant inter-branches.
// Lenght between nodes 7-10 cm max.
// Grow speed between 4.5 and 5.8 mm/day -> 5.787037037037037e-5 mm/s
class MinorBranch
{
    public:
        MinorBranch() = default;

        MinorBranch(Node firstNodeParam) {
            firstNode = firstNodeParam;
            lastNode = Node(firstNodeParam.position, &firstNode);
        }

        void pushLastNode(vector<double> pos) {
            lastNode.position.at(0) += pos.at(0);
            lastNode.position.at(1) += pos.at(1);
            lastNode.position.at(2) += pos.at(2);
            updateLength();
        }

        Node firstNode = Node(vector<double> {0.0, 0.0, 0.0}, NULL);
        Node lastNode = Node(vector<double> {0.0, 0.0, 0.0}, &firstNode);

        bool isHead {true};
        double length {0.0}; // [m]
        double maxLength {setRandomMaxLength()}; // [m]
        double verticalGrowRatio {5.787037037037037e-5 / 1000}; // [m/s]

    private:
        double setRandomMaxLength() {
            // Providing a seed value
            srand((unsigned) time(NULL));
            // Return random number between [0.07, 0.10]
            return (7 + (rand() % 4)) / 100.0;
        }

        void updateLength() {
            vector<double> AB {
                lastNode.position.at(0) - firstNode.position.at(0),
                lastNode.position.at(1) - firstNode.position.at(1),
                lastNode.position.at(2) - firstNode.position.at(2)
            };
            length = sqrt(pow(AB.at(0), 2) + pow(AB.at(1), 2) + pow(AB.at(2), 2));
        }
};


class MayorBranch
{
    public:
        MayorBranch() = default;

        int order {0};        // Order of the branch 0 (main), 1 (1st separation), 2 (second separation)
        vector<MinorBranch> minorBranches {MinorBranch()};
};


class Plant
{
    public:
        Plant() = default;

        double age {0.0};
        int higherOrder {0};
        vector<MayorBranch> mayorBranches{MayorBranch()};
};


int main(int argc, char *argv[])
{
    
    // System time
    auto startTime = std::chrono::system_clock::now();
    auto currentTime = startTime;
    std::chrono::duration<double> elapsedTime;

    // Simulated time
    double SIMULATION_DURATION {60};     //[s]
    int SIMULATION_SPEED_FACTOR {100'000};
    double lastSimuInstant {0};
    double currentSimuInstant {0};
    
    // Seed
    Plant capsicum = Plant();

    bool endSimulation = false;
    
    while (capsicum.age < SIMULATION_DURATION && !endSimulation)
    {
        // Update system time
        currentTime = std::chrono::system_clock::now();
        elapsedTime = currentTime - startTime;
        capsicum.age = elapsedTime.count();

        // Update simulated time
        currentSimuInstant = double(int(capsicum.age * SIMULATION_SPEED_FACTOR)) / SIMULATION_SPEED_FACTOR;

        // Update plant attributes
        if (lastSimuInstant != currentSimuInstant) {
            
            // For each minor branch of each mayor branch
            for (size_t i = 0; i < capsicum.mayorBranches.size(); i++)
            {
                MayorBranch& mayB = capsicum.mayorBranches.at(i);

                for (size_t j = 0; j < mayB.minorBranches.size(); j++)
                {
                    MinorBranch& minB = mayB.minorBranches.at(j);

                    // Grow if did not reach its max length
                    if (minB.length < minB.maxLength) {
                        minB.pushLastNode(vector<double>{0.0, minB.verticalGrowRatio, 0.0});

                    // Add new minor branch to current mayor branch
                    } else if (mayB.order == capsicum.higherOrder && minB.isHead) {
                            // endSimulation = true;
                            mayB.minorBranches.push_back(MinorBranch(minB.lastNode)); // <------ HERE crashes
                            minB.isHead = false;
                    }
                }
            }

        }

        // Done after generating 3 branches
        if (capsicum.mayorBranches.at(0).minorBranches.size() == 3) {
            break;
        }

        lastSimuInstant = currentSimuInstant;

    }

    return 0;
}
