#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

int SearchElevator(map<int, vector<int>> elevatorsMap, int clone_pos, int clone_floor) { 
    if(elevatorsMap.find(clone_floor) != elevatorsMap.end()) {
        vector<int> elevators = elevatorsMap[clone_floor];
        int idx_closest_elevator = 0;
        int distance_to_closest_elevator = 100;
        for(int i = 0; i < elevators.size(); i++) {
            if (abs(elevators[i] - clone_pos) < distance_to_closest_elevator) {
                distance_to_closest_elevator = abs(elevators[i] - clone_pos);
                idx_closest_elevator = i;
            }
        }
        return idx_closest_elevator;
    }
    else { return -1;}
}

void AnalyzeClonePos(int pos, int clone_pos, string direction) {
    if (((clone_pos > pos) &&(direction == "RIGHT")) ||
        ((clone_pos < pos) && (direction == "LEFT"))) {
            cout << "BLOCK" << endl;
        }
    else {cout << "WAIT" << endl;}
}

void AnalyzeElevatorPos(
    map<int, vector<int>> elevatorsMap, 
    int elevator_idx,
    int clone_pos,
    int clone_floor,
    string direction,
    int *nb_additional_elevators
) {
    if (elevator_idx == -1) {
        cout << "ELEVATOR" << endl;
        (*nb_additional_elevators)--;
    }
    else {
        AnalyzeClonePos(elevatorsMap[clone_floor][elevator_idx], clone_pos, direction);
    }
}

int main()
{
    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // number of additional elevators that you can build
    int nb_elevators; // number of elevators
    
    cin >> nb_floors >> width >> nb_rounds >> exit_floor >> exit_pos >> nb_total_clones >> nb_additional_elevators >> nb_elevators; cin.ignore();
    
    int *p_nb_additional_elevators = &nb_additional_elevators;
    map<int, vector<int>> elevatorsMap;
    
    for (int i = 0; i < nb_elevators; i++) {
        int elevator_floor; // floor on which this elevator is found
        int elevator_pos; // position of the elevator on its floor
        cin >> elevator_floor >> elevator_pos; cin.ignore();
        elevatorsMap[elevator_floor].push_back(elevator_pos);
    }
    bool isBlocked = false;
    // game loop
    while (1) {
        int clone_floor; // floor of the leading clone
        int clone_pos; // position of the leading clone on its floor
        string direction; // direction of the leading clone: LEFT or RIGHT
        cin >> clone_floor >> clone_pos >> direction; cin.ignore();
        if(clone_floor == exit_floor) {
            AnalyzeClonePos(exit_pos, clone_pos, direction);
        }
        else {
            int elevator_idx = SearchElevator(elevatorsMap, clone_pos, clone_floor);
            AnalyzeElevatorPos(elevatorsMap, elevator_idx, clone_pos, clone_floor, direction);
        }

        cout << "WAIT" << endl; // action: WAIT or BLOCK
    }
}