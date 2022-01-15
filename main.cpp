#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string repeater(char, int = 30);
void menu();

int main()
{
    menu();
    return 0;
}

string repeater(char c, int n)
{
    string x;
    for (int i = 0; i < n; i++)
    {
        x += c;
    }
    return x; 
}

void menu()
{
    string command;
    cout << "(!" << repeater('-') << "MENU" << repeater('-') << "!)" << endl;
    cout << "(1)  Log In" << endl;
    cout << "(2)  Sign Up (For Students)" << endl;
    cout << "(3)  exit the program" << endl;
    cout << "Please enter a number from above: ";
    cin >> command;
    if (command == "1") {
        // login();

    }
    if (command == "2") {
        // signup();

    }
    if (command == "3") {
        // exit
        exit(0);
    } else {
        // restart
        cout << "Please try again" << endl;
        menu();
    }
}
