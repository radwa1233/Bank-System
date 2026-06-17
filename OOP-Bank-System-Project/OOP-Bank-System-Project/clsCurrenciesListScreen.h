#pragma once
#include <iostream>
#include<iomanip>
#include "clsScreen.h"
#include"clsCurrency.h"
#include "clsInputValidate.h"
#include"clsMainScreen.h"
#include <fstream>


using namespace std;

class clsCurrenciesListScreen:protected clsScreen
{
	static vector <string >_LoadCurrenciesdataFromTheFile()
	{

      
            vector<string> vCurrenciesData;
            fstream MyFile("Currencies.txt", ios::in);

            if (MyFile.is_open())
            {
                string Line;
                while (getline(MyFile, Line))
                {
                    vCurrenciesData.push_back(Line);
                }

                MyFile.close();
            }

            return vCurrenciesData;
	}
public:
    static void ShowCurrenciesList()
    {
        vector <clsCurrency> vCurrencys = clsCurrency::GetCurrenciesList();

        string Title = "\t  Currencies List Screen";
        string SubTitle = "\t    (" + to_string(vCurrencys.size()) + ") Currency.";

        _DrawScreenHeader(Title, SubTitle);


        vector<string> data = _LoadCurrenciesdataFromTheFile();

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;

        cout << setw(8) << left << "" << "| " << left << setw(30) << "country ";
        cout << "| " << left << setw(8) << "Code";
        cout << "| " << left << setw(25) << "Name";
        cout << "| " << left << setw(10) << "Rate/(1$)";

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;


        for (string line : data)
        {
            vector<string> parts = clsString::Split(line, "#//#");

            if (parts.size() < 4)
                continue;

            string country = parts[0];
            string code = parts[1];
            string name = parts[2];
            string rate = parts[3];

            cout << setw(8) << left << "" << "| " << setw(30) << left << country;
            cout << "| " << setw(8) << left << code;
            cout << "| " << setw(25) << left << name;
            cout << "| " << setw(10) << left << rate << endl;
        }

        cout << setw(8) << left << "" << "\n\t_______________________________________________________";
        cout << "_________________________________________\n" << endl;
    }
    
};

