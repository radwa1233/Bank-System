#include <iostream>
#include "clsLoginScreen.h"

int main()

{
   
    while (clsLoginScreen::ShowLoginScreen())
    {
        clsLoginScreen::ShowLoginScreen();
    }

    return 0;
    
}