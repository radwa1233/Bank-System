#pragma once
#include <iostream>
#include<iomanip>
#include "clsScreen.h"
#include"clsCurrency.h"
#include "clsInputValidate.h"
#include"clsMainScreen.h"
#include"clsCurrenciesListScreen.h"
#include"clsFindCurrencyScreen.h"
#include"clsUpateCurrencyScreen.h"
#include"clsCurrencyExchangeScreen.h"
using namespace std;


class clsCurrencyMainScreen :protected clsScreen
{
    enum enCurrencyMainMenueOptions {
        eCurrenciesList = 1, eFindCurrency = 2, eUpdateCurrency = 3,
        eExchangeCurrency = 4, eBackToMainMenue = 5
    };

    static short _ReadCurrencyMainMenueOption()
    {
        cout << setw(37) << left << "" << "Choose what do you want to do? [1 to 5]? ";
        short Choice = clsInputValidate::ReadshortNumberBetween(1, 5, "Enter Number between 1 to 5 ? ");
        return Choice;
    }

    static  void _GoBackToCurrencyMainMenue()
    {
        cout << setw(37) << left << "" << "\n\tPress any key to go back to Currency Menu...\n";

        system("pause>0");
        ShowCurrencyMenue();
    }
    static  void _GoBackToMainMenue()
    {
        cout << setw(37) << left << "" << "\n\tPress any key to go back to Main Menu...\n";

        system("pause>0");
        
    }
    static void _ShowAllCurrenciesScreen()
    {
        
        clsCurrenciesListScreen::ShowCurrenciesList();

    }

    static void _ShowUpdateCurrencyScreen()
    {
        clsFindCurrencyScreen::ShowFindCurrencyScreen();
    }

    static void _ShowFindCurrenyScreen()
    {
        clsUpdateCurrencyRateScreen::ShowUpdateCurrencyRateScreen();
    }
    static void _ShowCurrenyExchangeScreen()
    {
        clsCurrencyExchangeMainScreen::ShowCurrenciesMenue();
    }


    static void _PerfromMainMenueOption(enCurrencyMainMenueOptions MainMenueOption)
    {
        switch (MainMenueOption)
        {
        case enCurrencyMainMenueOptions::eCurrenciesList:
        {
            system("cls");
            _ShowAllCurrenciesScreen();
            _GoBackToCurrencyMainMenue();
            break;
        }
        case enCurrencyMainMenueOptions::eFindCurrency:
            system("cls");
            _ShowUpdateCurrencyScreen();
            _GoBackToCurrencyMainMenue();
            break;

        case enCurrencyMainMenueOptions::eUpdateCurrency:
            system("cls");
            _ShowFindCurrenyScreen();
            _GoBackToCurrencyMainMenue();
            break;

        case enCurrencyMainMenueOptions::eExchangeCurrency:
            system("cls");
            _ShowCurrenyExchangeScreen();
            break;
        case enCurrencyMainMenueOptions::eBackToMainMenue:
            system("cls");
            _GoBackToMainMenue();
            break;

        
        }

    }



public:


    static void ShowCurrencyMenue()
    {

        system("cls");
        _DrawScreenHeader("\t\tMain Screen");

        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t\t\tMain Menu\n";
        cout << setw(37) << left << "" << "===========================================\n";
        cout << setw(37) << left << "" << "\t[1] Show Currencies List.\n";
        cout << setw(37) << left << "" << "\t[2] Find Currency.\n";
        cout << setw(37) << left << "" << "\t[3] Update Currency.\n";
        cout << setw(37) << left << "" << "\t[4] Exchange Currency.\n";
        cout << setw(37) << left << "" << "\t[5] Back to the Main Menu.\n";
        cout << setw(37) << left << "" << "===========================================\n";

        _PerfromMainMenueOption((enCurrencyMainMenueOptions)_ReadCurrencyMainMenueOption());
    }

};

