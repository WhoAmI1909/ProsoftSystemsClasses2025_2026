#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

int FindInputName(string *input_name, string name, int n)
// Определение индекса сигнала по его имени
{
	int nameIdx = -1;
	for(size_t i = 0; i < n; i++)
	{
		if(input_name[i]==name)
		{
			nameIdx = i;
			return nameIdx;
		}
	}
	cout << "Wrong Signal Name. Check it." << endl;
	return -1;
}

int main()
{
    int n;
    cin >> n; cin.ignore();
    int m;
    cin >> m; cin.ignore();
	
	// Массив входных сигналов
	string *inputs = new string[n];
	// Массив имен входных сигналов
	string *input_name = new string[n];
	//Массив выходных сигналов
	string *outputs = new string[m];
	
	
    for (int i = 0; i < n; i++) {
        cin >> input_name[i] >> inputs[i]; cin.ignore();
    }
    int size = inputs[0].length();
	
	string output_name;
    string type;
    string input_name_1;
    string input_name_2;
	for (int i = 0; i < m; i++) {
        
        cin >> output_name >> type >> input_name_1 >> input_name_2; cin.ignore();
		
		// код обработки операции между входными сигналами
		cout << output_name << " ";
		
		int input1_id = FindInputName(input_name, input_name_1, n);
		int input2_id = FindInputName(input_name, input_name_2, n);
		
		string input_1 = inputs[input1_id];
		string input_2 = inputs[input2_id];
		
		for(size_t i = 0; i < size; i++)
		{
			string subs_1 = input_1.substr(i,1);
			string subs_2 = input_2.substr(i,1);
			
			bool signal_1 = subs_1 == "-";
			bool signal_2 = subs_2 == "-";
			
			if(type == "OR") { cout << ((signal_1 | signal_2) ? "-" : "_"); }
			else if(type == "AND") { cout << ((signal_1 & signal_2) ? "-" : "_"); }
			else if(type == "XOR") { cout << ((signal_1 ^ signal_2) ? "-" : "_"); }
			else if(type == "NAND") { cout << ((!(signal_1 & signal_2)) ? "-" : "_"); }
			else if(type == "NOR") { cout << ((!(signal_1 | signal_2)) ? "-" : "_"); }
			else if(type == "NXOR") { cout << ((!(signal_1 ^ signal_2)) ? "-" : "_"); }
		}
		cout << endl;
    }
	
	delete [] inputs;
	delete [] input_name;
	delete [] outputs;
}