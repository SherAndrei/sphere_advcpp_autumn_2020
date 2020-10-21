#include <fstream>
#include <iostream>

int main()
{
	std::string filename;
	std::cin >> filename;
	std::ofstream file("./tests/" + filename);
	
	if(!file.good()) {
	 	std::cout << "Failure" << std::endl;
		return -1;
	}
	else {
		std::cout << "Success" << std::endl;
	}
	file.close();
}