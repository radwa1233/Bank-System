#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "clsScreen.h"
#include "clsString.h"
using namespace std;

class clsRegisterLoginScreen : protected clsScreen
{
    static vector<string> _LoadRegisterDataFromFile()
    {
        vector<string> vLoginData;
        fstream MyFile("LoginRegister.txt", ios::in);

        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                vLoginData.push_back(Line);
            }

            MyFile.close();
        }

        return vLoginData;
    }

public:
    static void ShowLoginRegisterScreen()
    {
        if (!CheckAccessRights(clsUser::enPermissions::pLoginRegister))
        {
            return;
        }

        _DrawScreenHeader("\tLogin Register Screen");

        vector<string> data = _LoadRegisterDataFromFile();

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;

        cout << setw(8) << left << "" << "| " << left << setw(20) << "Date & Time";
        cout << "| " << left << setw(13) << "User Name";
        cout << "| " << left << setw(12) << "Password";
        cout << "| " << left << setw(20) << "Permissions";

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;

        for (string line : data)
        {
            vector<string> parts = clsString::Split(line, "#//#");

            if (parts.size() < 4)
                continue; 

            string dateTime = parts[0];
            string username = parts[1];
            string password = parts[2];
            string permission = parts[3];

            cout << setw(8) << left << "" << "| " << setw(20) << left << dateTime;
            cout << "| " << setw(13) << left << username;
            cout << "| " << setw(12) << left << password;
            cout << "| " << setw(20) << left << permission << endl;
        }

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;
    }
};
