#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include"clsScreen.h"
#include"clsString.h"
#include<iomanip>
using namespace std;
class TransferLogScreen:protected clsScreen
{
    static string _ParseLoginLine(string Line, string Separator = "#//#")
    {
        vector<string> sLoginData = clsString::Split(Line, Separator);
        string Data = "";

        if (sLoginData.size() >= 6)
        {
            Data += sLoginData[0] + " ";
            Data += sLoginData[1] + " ";
            Data += sLoginData[2] + " ";
            Data += sLoginData[4] + " ";
            Data += sLoginData[5] + " ";
            Data += sLoginData[6];
          
        }

        return Data;
    }

    static vector<string> _LoadRegisterDataFromFile()
    {
        vector<string> vLoginData;
        fstream MyFile("TransferLog.txt", ios::in);

        if (MyFile.is_open())
        {
            string Line;
            while (getline(MyFile, Line))
            {
                vLoginData.push_back(Line);
               
            }
               MyFile.close();
            return vLoginData;
        }

    }
public:
    static void showLoginRegisterScreen()
    {
       /* if (!CheckAccessRights(clsUser::enPermissions::pLoginRegister))
        {
            return;
        }*/
        _DrawScreenHeader("\t Transfer Log Screen");

        vector<string> data = _LoadRegisterDataFromFile();
        cout << setw(8) << left << "" << "\n\t__________________________________________________________________________________________________________\n" << endl;

        cout << setw(8) << left << "" << "| " << left << setw(20) << "Date & Time";
        cout << "| " << left << setw(10) << "Acc.1";
        cout << "| " << left << setw(10) << "Acc.2";
        cout << "| " << left << setw(10) << "Amount";
        cout << "| " << left << setw(12) << "Balance.1";
        cout << "| " << left << setw(12) << "Balance.2";
        cout << "| " << left << setw(12) << "User" << endl;

        cout << setw(8) << left << "" << "\n\t__________________________________________________________________________________________________________\n" << endl;

        for (string line : data)
        {
            vector<string> parts = clsString::Split(line, " | ");

            if (parts.size() == 7)
            {
                string DateTime = parts[0];
                string FromAcc = parts[1].substr(6);
                string ToAcc = parts[2].substr(4);
                string Amount = parts[3].substr(8);
                string Balance1 = parts[4].substr(10);
                string Balance2 = parts[5].substr(10);
                string User = parts[6].substr(6);

                cout << setw(8) << left << "" << "| " << left << setw(20) << DateTime;
                cout << "| " << left << setw(10) << FromAcc;
                cout << "| " << left << setw(10) << ToAcc;
                cout << "| " << left << setw(10) << Amount;
                cout << "| " << left << setw(12) << Balance1;
                cout << "| " << left << setw(12) << Balance2;
                cout << "| " << left << setw(12) << User << endl;
            }
        }


        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;
    }

};

