/* Амелина Полина Павловна, 2 курс, 91 группа, лаба 1
№5: Система двусторонних дорог такова, что для любой пары городов можно указать
соединяющий их путь. Найдите такой город, сумма расстояний от которого до остальных
городов минимальна. */

//предупреждаю, что я конечно все перевела, но у меня машина с английской локалью, поэтому сама я проверить ничего не могу

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

bool belongs(std::string s, char c)
{
	if (s.find(c) != std::string::npos)
		return true;
	return false;
}

void enterAgainMessage(bool newline)
{
	std::cout << "Неверный ввод. Попробуйте еще раз: ";
	if (newline)
		std::cout << std::endl;
}

char inputChar(std::string validChars) //строка с разрешенными значениями
{
	std::string check;
	std::getline(std::cin, check);
	while (check.length() != 1 || !belongs(validChars, check[0]))
	{
		enterAgainMessage(false);
		std::getline(std::cin, check);
	}
	return check[0];
}



bool isValidFilename(std::string filename)
{
	for (char c : "<>:\"/\\|?*")
		if (belongs(filename, c))
		{
			enterAgainMessage(true);
			return false;
		}
	return true;
}

void inputFilename(std::string &filename)
{
	do
		std::getline(std::cin, filename);
	while (!isValidFilename(filename));
}



bool ifFileOpened(std::fstream &file)
{
	if (!file.is_open())
	{
		std::cout << "Файл с таким названием не существует/недоступен. Введите еще раз:" << std::endl;
		return false;
	}
	return true;
}

bool ifCreateOrRewriteFile(std::string filename)
{
	if (std::filesystem::exists(filename))
	{
		std::cout << "Файл уже существует. Перезаписать его? Y/N" << std::endl;
		char rewriteChoice = inputChar("yYnN");
		if (rewriteChoice == 'n' || rewriteChoice == 'N')
		{
			std::cout << "Введите другое название:" << std::endl;
			return false;
		}
	}
	return true;
}

void openFile(std::fstream &file, bool forInput) //открываем ли мы файл для ввода данных или вывода
{
	std::cout << std::endl << "Введите название файла:" << std::endl;
	std::string filename;
	if (forInput)
	{
		do
		{
			inputFilename(filename);
			file.open(filename, std::ios::in);
		}
		while (!ifFileOpened(file));
	}
	else
	{
		do
			inputFilename(filename);
		while (!ifCreateOrRewriteFile(filename));
		file.open(filename, std::ios::out);
	}
}



bool findNumberFile(int &number, std::fstream &file)
{
	while (!file.eof() && !isdigit(file.get()));
	if (!file.eof())
	{
		file.unget();
		number = file.get() - '0';
		while (!file.eof() && isdigit(file.peek()))
			number = number * 10 + (file.get() - '0');
		return true;
	}
	return false;
}

bool inputDataFile(int &cities, int ***roads) //возвращает true, если все нужные данные были считаны
{
	std::fstream file;
	openFile(file, true);
	if (findNumberFile(cities, file) && cities > 1)  //если городов < 2, то и дорог нет
	{
		//почистила
		//считаем количество дорог по формуле 1 + 2 + ... + (cities - 1)
		int roadsAmount = 0;
		*roads = new int *[cities - 1];
		for (int i = 0; i < cities - 1; i++)
		{
			(*roads)[i] = new int[cities - 1 - i];
			roadsAmount += i + 1;
		}
		for (int i = 1; i <= roadsAmount; i++)
		{
			int city1, city2, roadLength;                             //индексы не могут быть больше количества городов, а длина дороги не может быть = 0
			if ((findNumberFile(city1, file) && city1 <= cities) && (findNumberFile(city2, file) && city2 <= cities) && (findNumberFile(roadLength, file) && roadLength))
				if (city2 > city1)
					(*roads)[city1 - 1][cities - city2] = roadLength;
				else
					(*roads)[city2 - 1][cities - city1] = roadLength;
			else
			{
				std::cout << std::endl << "Файл содержит некорректную информацию. Обработка данных не может быть завершена." << std::endl;
				file.close();
				return false;
			}
		}
	}
	file.close();
	return true;
}



bool isNumberKeyboard(std::string number)
{
	for (int i = 0; i < number.length(); i++)
		if (!isdigit(number[i]))
		{
			enterAgainMessage(false);
			return false;
		}
	return true;
}

void inputNumberKeyboard(int &number)
{
	std::string check;
	do
		std::getline(std::cin, check);
	while (!isNumberKeyboard(check));
	number = std::stoi(check);
}

void inputRowKeyboard(int cities, int ***roads, int i) //новая функция
{
	for (int j = i + 1; j <= cities; j++)
	{
		std::cout << i << " и " << j << ": ";
		int roadLength;
		inputNumberKeyboard(roadLength);
		while (!roadLength)
		{
			std::cout << "Длина дороги не может быть равна 0. Введите еще раз: ";
			inputNumberKeyboard(roadLength);
		}
		(*roads)[i - 1][cities - j] = roadLength;
	}
}

void inputDataKeyboard(int &cities, int ***roads)
{
	std::cout << std::endl << "Введите количество городов: ";
	inputNumberKeyboard(cities);
	if (cities > 1)
	{
		*roads = new int *[cities - 1];
		for (int i = 0; i < cities - 1; i++)
			(*roads)[i] = new int[cities - 1 - i];
		std::cout << "Введите длину дороги между городами... ";
		for (int i = 1; i < cities; i++)
			inputRowKeyboard(cities, roads, i);
	}
}



int numberLength(int number)
{
	int length = 1;
	while (number > 9)
	{
		length++;
		number /= 10;
	}
	return length;
}

void valuesPerCity(int &roadLengthSum, int &longestRoad, int **roads, int cities, int i)
{
	roadLengthSum = 0;
	longestRoad = 0;
	for (int j = 0; j < cities - 1 - i; j++)
	{
		roadLengthSum += roads[i][j];
		int temp = numberLength(roads[i][j]);
		longestRoad = std::max(temp, longestRoad);
	}
	for (int j = 0; j < i; j++)
	{
		roadLengthSum += roads[j][cities - 1 - i];
		int temp = numberLength(roads[j][cities - 1 - i]);
		longestRoad = std::max(temp, longestRoad);
	}
}

void printTableLine(int cities, int *longestRoads)
{
	std::cout << std::endl << std::setfill('-') << std::setw(numberLength(cities) + 3) << "-";
	for (int i = 0; i < cities; i++)
		std::cout << std::setw(longestRoads[i] + 3) << "-";
	std::cout << std::endl << std::setfill(' ');
}

void printTable(int cities, int **roads, int *longestRoads)
{
	std::cout << std::setw(numberLength(cities) + 3) << "|";               //пустое пространство над первым столбцом
	for (int i = 0; i < cities; i++)                                       //первый ряд - номера всех городов
		std::cout << std::setw(longestRoads[i] + 1) << i + 1 << " |";
	printTableLine(cities, longestRoads);                                  //линия, разделяющая ряды
	for (int i = 0; i < cities; i++)                                       //все остальные ряды
	{
		std::cout << std::setw(numberLength(cities) + 1) << i + 1 << " |"; //номер города
		for (int j = 0; j < i; j++)
			std::cout << std::setw(longestRoads[j] + 1) << roads[j][cities - 1 - i] << " |";
		std::cout << std::setw(longestRoads[i] + 3) << "|";                //не печатаем ничего для одного и того же города
		for (int j = cities - 2 - i; j >= 0; j--)
			std::cout << std::setw(longestRoads[cities - 1 - j] + 1) << roads[i][j] << " |";
		printTableLine(cities, longestRoads);                              //линия, разделяющая ряды
	}
}

void printAnswer(int cities, int ***roads, bool usingFile) //передаем решение пользователя - печатать значения в файл или нет
{
	std::fstream file;
	std::streambuf *origbuf = nullptr;
	if (usingFile)
	{
		openFile(file, false);
		origbuf = std::cout.rdbuf(file.rdbuf());
	}
	if (cities > 2)                               //если городов меньше, то нет смысла что-то считать
	{
		int *roadLengthSumPerCity = new int[cities], *longestRoads = new int[cities]; //longestRoads - массив самых длинных чисел среди всех длин дорог для каждого города
		for (int i = 0; i < cities; i++)
			valuesPerCity(roadLengthSumPerCity[i], longestRoads[i], *roads, cities, i);

		std::cout << "Таблица всех дорог:" << std::endl;
		printTable(cities, *roads, longestRoads); //печатаем таблицу всех значений для наглядности

		int minRoadLengthCity = 0;                //индекс города с минимальной суммой длин дорог
		for (int i = 1; i < cities; i++)
			if (roadLengthSumPerCity[minRoadLengthCity] > roadLengthSumPerCity[i])
				minRoadLengthCity = i;

		std::cout << std::endl << "Город с минимальной суммой расстояний до остальных городов: " << minRoadLengthCity + 1 << std::endl
			<< "Сама сумма: " << roadLengthSumPerCity[minRoadLengthCity];

		delete[] roadLengthSumPerCity;
		delete[] longestRoads;
	}
	else                                          //уникальные сообщения для ситуаций, где городов <= 2
	{
		if (!cities) std::cout << "Городов не существует.";
		if (cities == 1) std::cout << "Всего один город. Дорог не существует.";
		if (cities == 2) std::cout << "Всего два города. Единственная существующая дорога будет самой короткой: " << (*roads)[0][0];
	}
	if (usingFile)
	{
		file.close();
		std::cout.rdbuf(origbuf);
		std::cout << std::endl << "Печать успешно завершена.";
	}
}



//разбила main на 3 функции
char menu(bool &ifInput)
{
	if (ifInput)
	{
		std::cout << "Как вы хотите ввести данные?" << std::endl << "F: из файла" << std::endl << "K: с клавиатуры" << std::endl << "E: выйти" << std::endl;
		ifInput = false;
		return inputChar("fFkKeE");
	}
	else
	{
		std::cout << std::endl << "Куда вы хотите напечатать ответ?" << std::endl << "F: в файл" << std::endl << "S: на экран" << std::endl << "E: выйти" << std::endl;
		return inputChar("fFsSeE");
	}
}

bool menuFunc(char ioChoice, int &cities, int ***roads)
{
	switch (ioChoice)
	{
		case 'k':
		case 'K':
			inputDataKeyboard(cities, roads);
			return true;
		case 'f':
		case 'F':
			if (cities < 0)
				return inputDataFile(cities, roads);
			else
			{
				printAnswer(cities, roads, true);
				break;
			}
		case 's':
		case 'S':
			std::cout << std::endl;
			printAnswer(cities, roads, false);
	}
	std::cout << std::endl;
	return false;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	int cities = -1, **roads = nullptr; //cities - кол-во городов, roads - матрица дорог
	bool ifInput = true;
	while (menuFunc(menu(ifInput), cities, &roads));
	if (roads)
	{
		for (int i = 0; i < cities - 1; i++)
			delete[] roads[i];
		delete[] roads;
	}
	return 0;
}
