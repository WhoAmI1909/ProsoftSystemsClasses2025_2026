#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

int max_prize(int **matrix, int dimensions_size, int currRow, int currCol, int sum_increment) {
    // Если дошли до нижней строчки с выигрышем
    if (currRow == dimensions_size - 1) {
        return sum_increment * matrix[currRow][currCol];
    }
    sum_increment = sum_increment + matrix[currRow][currCol];
    
    // Рекурсивно идем в обе возможные клетки следующей строки
    int left_prize = max_prize(matrix, dimensions_size, currRow + 1, currCol, sum_increment);
    int right_prize = max_prize(matrix, dimensions_size, currRow + 1, currCol + 1, sum_increment);
    
    // Возвращаем максимальный результат
    return max(left_prize, right_prize);
}

int main()
{
    int max = 0;
    int height;
    cin >> height; cin.ignore();
    int **matrix = new int*[height + 1];
    for(int i = 0; i < height + 1; i++) {
        matrix[i] = new int[height + 1];
        for(int j = 0; j < height + 1; j++) {
            matrix[i][j] = 0;
        }
    }


    for (int i = 0; i < height; i++) {
        string increments;
        getline(cin, increments);
        for(int col = 0; col <= i; col++) {
            matrix[i][col] = increments[col] - '0';
        }
    }

    for (int i = 0; i < height + 1; i++) {
        cin >> matrix[height][i]; cin.ignore();
    }
    max = max_prize(matrix, height + 1, 0, 0, 0);

    cout << max << endl;

    for(int i = 0; i < height; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}