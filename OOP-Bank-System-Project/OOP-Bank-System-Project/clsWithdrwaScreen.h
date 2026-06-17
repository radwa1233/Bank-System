#pragma once
#include<iostream>
#include"clsInputValidate.h"
#include"clsBankClient.h"
#include"clsScreen.h"
using namespace std;
class clsWithdraw :protected clsScreen
{
	static string  _ReadAccountNumber()
	{

		string AccountNumber;
		AccountNumber = clsInputValidate::ReadString();
		return AccountNumber;
	}
	static double _ReadWithdrawAmount()
	{
		cout << "Enter  the Deposit Amount";
		double DepositAmount;
		DepositAmount = clsInputValidate::ReadDblNumber();
		return DepositAmount;
	}
	static void _PrintClientData(clsBankClient Client)
	{
		cout << "\nClient Card:";
		cout << "\n___________________";
		cout << "\nFirstName   : " << Client.FirstName;
		cout << "\nLastName    : " << Client.LastName;
		cout << "\nFull Name   : " << Client.FullName();
		cout << "\nEmail       : " << Client.Email;
		cout << "\nPhone       : " << Client.Phone;
		cout << "\nAcc. Number : " << Client.AccountNumber();
		cout << "\nPassword    : " << Client.PinCode;
		cout << "\nBalance     : " << Client.AccountBalance;
		cout << "\n___________________\n";


	}
public:
	static void Withdraw()
	{
		_DrawScreenHeader("\t\Withdraw Screen ");
		cout << " Enter The Account Number\n";
		string AccountNumber = _ReadAccountNumber();
		while (!(clsBankClient::IsClientExist(AccountNumber)))
		{
			cout << "Account Number is Unvalid , Please Try Again \n";
			AccountNumber = _ReadAccountNumber();
		}
		clsBankClient Client = clsBankClient::GetClient(AccountNumber);
		_PrintClientData(Client);
		double Withdraw = _ReadWithdrawAmount();
		while (Withdraw > Client.AccountBalance)
		{
			cout << "this Opretor cant be done ";
			Withdraw = _ReadWithdrawAmount();
		}
		cout << "\nAre you sure you want to perform this transaction?  [y/n]";
		char Answer = 'n';
		cin >> Answer;

		if (Answer == 'Y' || Answer == 'y')
		{
			Client.AccountBalance -= Withdraw;
			cout << "\nAmount Withdraw Successfully.\n";
			cout << "\nNew Balance Is: " << Client.AccountBalance;
			Client.Save();

		}
		else
		{
			cout << "\nOperation was cancelled.\n";
		}
		

	
	}
};
