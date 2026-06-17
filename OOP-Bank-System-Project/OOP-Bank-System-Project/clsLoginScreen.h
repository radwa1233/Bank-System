#pragma once
#include <iostream>
#include "clsScreen.h"
#include"clsMainScreen.h"


class clsLoginScreen :protected clsScreen
{
private:

    static  bool _Login()
    {
        bool LoginFaild = false;
          short counter = 1;
        string Username, Password;

        do
        {
            
            if (LoginFaild)
            {
                
                cout << "\Invalid  Username/Password!\n\n";
                cout << "You Have " << 3 - counter << " Trial(s) to Login\n";
               counter++;
            }

            cout << "Enter Username? ";
            cin >> Username;

            cout << "Enter Password? ";
            cin >> Password;

            CurrentU = clsUser::Find(Username, Password);

            LoginFaild = CurrentU.IsEmpty();
            if (counter == 3)
            {
                cout << "\nYou Are Locked After 3 Falid Trails\n";
                return false;
            }
        } while (LoginFaild);

        CurrentU.RegisterLogIn();
        clsMainScreen::ShowMainMenue();

    }

public:


    static bool ShowLoginScreen()
    {
        system("cls");
        _DrawScreenHeader("\t  Login Screen");
         return _Login();

    }


};

