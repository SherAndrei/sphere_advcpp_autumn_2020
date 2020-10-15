#include <fstream>
#include <iostream>
#include <string>

int main()
{
	std::ifstream file("./tests/somename.txt");
	
	if(!file.good()) { //пишем состояние файла
	 	std::cout << "Mistake" << std::endl;
		return -1;
	} else {
		std::cout << "Success" << std::endl;
	}

	std::string data;
	while(file >> data)
		std::cout << data << std::endl; 

	file.close();
}