#include <fstream>
#include <iostream>

int main()
{
	std::string filename;
	std::cin >> filename;
	std::ofstream file("./tests/" + filename);
	
	if(!file.good())
	 	std::cout << "Mistake" << std::endl;
	else {
		std::cout << "Success" << std::endl;
	}
	file.close();
}