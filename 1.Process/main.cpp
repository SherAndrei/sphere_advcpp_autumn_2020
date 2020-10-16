#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <thread>
#include <string>   
#include <vector>   
#include "process.h"

using namespace std;
 
#define LOG(x) cout << #x << ": " << (x) << endl


int main()
{
	size_t size;

	{
		//read date and logs it into stdout
		Process proc("/usr/bin/date");
		string str(512, '\0');
		try{
			size = proc.read(str.data(), str.length());
		} catch (std::runtime_error& ex) {
			LOG(ex.what());
		}
		LOG(str);
		LOG(size);
	}
	{
		//creates a file "./tests/somename.txt"
		Process proc("./tests/1test");
		string str = "somename.txt\n";
		try{
			size = proc.write(str.data(), str.length());
			
			std::string file_cond(20, '\0'); 
			proc.read(file_cond.data(), file_cond.length());
			LOG(file_cond);
			
			} catch (std::runtime_error& ex) {
			LOG(ex.what());
		}
	}

	{
		//writesExact to a file with file name "somename" str
		Process proc("./tests/2test");
		try{
			std::string file_cond(8, '\0'); 					 
			proc.read(file_cond.data(), file_cond.length());	// читаем состояние файла
			LOG(file_cond);

			std::string str = "Studying C++\n at Sphere!\n"; 	// строчку которую мы отправим
			
			proc.writeExact(str.data(), str.length());			// все хорошо, пишется str.length()
			proc.closeStdin();									// дескриптор становится невалидным

			::sleep(1); // даем дочерернему процессу успеть записать слова в файл

			proc.read(file_cond.data(), file_cond.length()); // считываем последнее слово
			LOG(file_cond);										// выводим на экран					
		} catch (std::runtime_error& ex) {
			LOG(ex.what());
		}
	}
	{
		// readExact слова из файла "./tests/somename.txt"
		Process proc("./tests/3test");
		proc.close();
		proc.open("./tests/3test");
		try{
			std::string file_cond(8, '\0'); 					 
			proc.read(file_cond.data(), file_cond.length());	// читаем состояние файла
			LOG(file_cond);

			std::string str(50, '\0');
			proc.readExact(str.data(), 20);
			LOG(str);	
		} catch (std::runtime_error& ex) {
			LOG(ex.what());
		}
	}

    return 0;
}