#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool isBlockBound(int clone_pos, int width, string direction)
{   //определение края карты
	return (((clone_pos == (width - 1)) && (direction == "RIGHT")) ||
            ((clone_pos == 0) && (direction == "LEFT")));
}	

bool isBlockExit(int clone_pos, int exit_pos, string direction)
{   //определение расположения выхода относительно расположения основного персонажа
	return (((clone_pos > exit_pos) && (direction == "RIGHT")) ||
			((clone_pos < exit_pos) && (direction == "LEFT")));
}

bool isBlockElevator(int *ElevatorPos, int clone_pos, int clone_floor, string direction)
{   //определение расположения лифта относительно основного персонажа
    return (((clone_pos > ElevatorPos[clone_floor]) && (direction == "RIGHT")) ||
		    ((clone_pos < ElevatorPos[clone_floor]) && (direction == "LEFT")));
}						 

int main()
{
    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // ignore (always zero)
    int nb_elevators; // number of elevators
		
    cin >> nb_floors >> width >> nb_rounds >> exit_floor >> exit_pos >> nb_total_clones >> nb_additional_elevators >> nb_elevators; cin.ignore();
    	
	int* elevatorsPosArray = new int[nb_elevators]; //хранение координат расположения лифтов
	
	for (size_t i = 0; i < nb_elevators; i++) {
        int elevator_floor; // floor on which this elevator is found
        int elevator_pos; // position of the elevator on its floor
        cin >> elevator_floor >> elevator_pos; cin.ignore();
		//уровень используется в качестве индекса для хранения позиции лифта
		elevatorsPosArray[elevator_floor] = elevator_pos;
    }
	bool isBlocked = false;
    // game loop
    while (1) 
	{
        isBlocked = false;
        int clone_floor; // floor of the leading clone
        int clone_pos; // position of the leading clone on its floor
        string direction; // direction of the leading clone: LEFT or RIGHT
        cin >> clone_floor >> clone_pos >> direction; cin.ignore();

		if(clone_floor != -1)
        {
            if(isBlockBound(clone_pos, width, direction)) //проверка на достижение границы карты
            {
                cout << "BLOCK" << endl;
            }
            else
            {   //проверка, если на этаже выход
                isBlocked = ((clone_floor == exit_floor) && isBlockExit(clone_pos, exit_pos, direction));
                if(isBlocked) { cout << "BLOCK" << endl; }
                else if(!isBlocked && (nb_elevators != 0) && (clone_floor != exit_floor))
                {   //определение положения лифта
                    if (isBlockElevator(elevatorsPosArray, clone_pos, clone_floor, direction)) 
                    { cout << "BLOCK" << endl; }
                    else { cout << "WAIT" << endl; }
                }
                else {cout << "WAIT" << endl;}
            }    
		}
        else {cout << "WAIT" << endl;}
	}
    delete [] elevatorsPosArray;
}