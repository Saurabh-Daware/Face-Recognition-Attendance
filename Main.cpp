// Face-Recognition-Attendance.cpp : This file contains the 'main' function. Program execution begins and ends there.

//include files
#include <iostream>
#include "Functions.h"


int main()
{
	while (1) {
		int choice;
		system("cls");
		std::cout << "1. Recognise Face\n";
		std::cout << "2. Add Face\n";
		std::cout << "3. Exit\n";
		std::cout << "Choose One: ";
		std::cin >> choice;
		switch (choice)
		{
		case 1:
			FaceRecognition();
			break;
		case 2:
			addFace();
			eigenFaceTrainer();
			break;
		case 3:
			std::cout << "Exiting ...";
			return 0;
		default:
			return 0;
		}
	}
	//system("pause");
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
