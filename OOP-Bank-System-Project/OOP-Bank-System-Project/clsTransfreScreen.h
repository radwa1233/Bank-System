#include"clsScreen.h"

#include"Radwa.h"
class clsTransferScreen:protected clsScreen
{
    static void _PrintClient(clsBankClient Client)
    {
        cout << "\nClient Card:";
        cout << "\n___________________";
        cout << "\nFull Name   : " << Client.FullName();
        cout << "\nAcc. Number : " << Client.AccountNumber();
        cout << "\nBalance     : " << Client.AccountBalance;
        cout << "\n___________________\n";
    }

public:
    static void ShowTransferScreen()
    {
        _DrawScreenHeader("\t Transfer Screen ");
        cout << "Enter The First Account: ";
        string Acc1 = clsInputValidate::ReadString();
        clsBankClient client1 = clsBankClient::Find(Acc1);

        if (client1.IsEmpty())
        {
            cout << "\nAccount 1 not found.\n";
            return;
        }

        _PrintClient(client1);

        cout << "Enter The Second Account: ";
        string Acc2 = clsInputValidate::ReadString();
        clsBankClient client2 = clsBankClient::Find(Acc2);

        if (client2.IsEmpty())
        {
            cout << "\nAccount 2 not found.\n";
            return;
        }

        _PrintClient(client2);

        cout << "Enter Amount: ";
        double Amount;
        cin >> Amount;

        if (Amount <= 0)
        {
            cout << "\nAmount must be greater than zero.\n";
            return;
        }

        if (client1.AccountBalance < Amount)
        {
            cout << "\nInsufficient balance in Account 1.\n";
            return;
        }

        cout << "\nAre you sure you want to transfer this amount (y/n)? ";
        char Answer = 'n';
        cin >> Answer;

        if (Answer == 'y' || Answer == 'Y')
        {
            client1.AccountBalance -= Amount;
            client2.AccountBalance += Amount;


            cout << "\nTransfer Done Successfully.\n";
            cout << "\nNew Balances:\n";
            _PrintClient(client1);
            _PrintClient(client2);
            client1._RegisterTransferLog(Amount, client2, CurrentU.UserName);
            
            client1.Save();
            client2.Save();
        }
    }

};
