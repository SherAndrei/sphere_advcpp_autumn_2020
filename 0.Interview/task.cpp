#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <cctype>

// Для простой работы после обработки команды
// используем enum
enum class COMMAND {
    UNKNOWN = -1,
    LOWER_CASE,
    UPPER_CASE,
    REPLACE
};

// стандартный запрос пользователя
struct Query  {
    int line = 0;
    // по умолчанию стоят нули,
    // так как есть команды, не затрагивающая
    // эти чары
    char A = 0;
    char B = 0;
};

// Чтобы не работать с командой пользователя дважды
// функция обработки возвращает пару из
// енума и самого запроса
std::pair<COMMAND, Query> parse(const std::string& str_command) {
    // Создаем строковый поток для простой обработки
    std::istringstream iss(str_command);
    char c;
    // не пропускаем пробелы, так как пользователь
    // может захотеть поменять пробелы на что-то другое
    while (iss >> std::noskipws >> c) {
        // проверяем начинается ли с цифры
        // если нет, то команда неивзестна
        if (isdigit(c)) {
            // Записываем все первые цифры
            //  в число N
            std::string N(1, c);
            while (isdigit(iss.peek()))
                N += iss.get();

            // проверяем на двоеточие
            // если его нет, то команда неивзестна
            if (iss.peek() != ':')
                break;
            iss.ignore(1);

            // Дальше работаем с остатком команды
            if (iss.peek() == 'u') {
                c = iss.get();
                if (iss.peek() == EOF)
                    return { COMMAND::LOWER_CASE, { stoi(N) } };
                else
                    break;
            } else if (iss.peek() == 'U') {
                c = iss.get();
                if (iss.peek() == EOF)
                    return { COMMAND::UPPER_CASE, { stoi(N) } };
                else
                    break;
            } else if (iss.peek() == 'R') {
                c = iss.get();
                char A = iss.get();
                char B = iss.get();
                if (A != EOF && B != EOF && iss.peek() == EOF)
                    return { COMMAND::REPLACE , { stoi(N), A, B } };
                else
                    break;
            } else {
                 break;
            }
        } else {
            break;
        }
    }
    // Возвращаем неивзестный
    return { COMMAND::UNKNOWN, {}};
}

std::string find_field(std::ifstream& input, int N)  {
    std::string field = {};
    // чтобы работать с нулевым полем
    // и чтобы программа заходила в цикл\/ стоит единица
    for (int counter = 0; counter != N + 1; counter++) {
        if (input.good()) {
            // если еще не достигли EOF считываем
            getline(input, field, '\t');
        } else {
            field.clear();
            // когда достигли EOF, очищаем строчку
            // от последнего getline и возвращаем пустоту
            std::cout << "No such field!";
            break;
        }
    }
    // Если достигли EOF, то его надо сбросить
    input.clear();
    // Также сбросить курсор в файле
    input.seekg(0);
    return field;
}

int main() {
    // Считываем название файла со строки,
    // учитывая пробелы и другие символы в названии
    std::cout << "Please enter filename:\n";
    std::string filename;
    std::getline(std::cin, filename);
    std::ifstream input(filename);
    // Если не получилось открыть возвращаем код ошибки
    if (!input.good()) {
        std::cout << "Cannot open file with such name!" << std::endl;
        return -1;
    }

    // Обрабатываем команды пользователя.
    // Так как неизвестно сколько изначально команд,
    // обрабатываем пока пользователь не закончит ввод
    std::cout << "Please enter command:\n";
    std::string str_command;
    while (std::getline(std::cin, str_command)) {
        // Получаем нашу пару и обрабатываем ее
        auto pair = parse(str_command);
        COMMAND command = pair.first;
        Query   request = pair.second;
        switch (command) {
        case COMMAND::LOWER_CASE: {
            int N = request.line;
            std::string field = find_field(input, N);
            // В найденом поле меняем все,
            // что возможно поменять
            for (size_t i = 0; i < field.size(); ++i)
                field[i] = tolower(field[i]);
            // Если поле не найдено, то в цикл не зайдет
            // здесь напечатается перевод строки
            std::cout << field << std::endl;
            break;
        }
        case COMMAND::UPPER_CASE: {
            // Аналогично LOWER_CASE
            int N = request.line;
            std::string field = find_field(input, N);

            for (size_t i = 0; i < field.size(); ++i)
                field[i] = toupper(field[i]);
            std::cout << field << std::endl;
            break;
        }
        case COMMAND::REPLACE: {
            // Аналогично LOWER_CASE
            int N = request.line;
            char lhs = request.A;
            char rhs = request.B;
            std::string field = find_field(input, N);
            // Если какой-то символ совпадает с lhs,
            // меняем его на rhs
            for (size_t i = 0; i < field.size(); ++i) {
                if (field[i] == lhs)
                    field[i] = rhs;
            }
            std::cout << field << std::endl;
            break;
        }
        case COMMAND::UNKNOWN:
            // Если пользователь указал неправильную команду
            std::cout << "Unknown command!" << std::endl;
            break;
        default:
            break;
        }
    }
}
