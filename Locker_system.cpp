#include <iostream>
#include <cctype>
#include <fstream>
#include <conio.h>
#include <string.h>
#include <cstdlib>
#include <ctime>
using namespace std;

#define LOCK 30	//Maximum locker.		
#define MAX_SELECTION 5 //We have currently 5 main functions only.
#define MAX_RESIDENT 200 //Maximum resident records
#define KEY 0xFACA //secret key for cryptography.

typedef struct
{
	char res_name[50];
	int  res_unit_no;
	char res_password[15];
	char res_contact[12];
	int  lock_id;
	char res_otp[6];

} USER_DATA;   //Details of the registered resident.

typedef struct
{
	int	 id;
	char lock_otp[6];
	bool lock_status;

} LOCKER;      //Lockers status and OTP.

int read_file(USER_DATA* residents, LOCKER* locker);   //Read resident and locker data.
int user_registeration(USER_DATA* residents, int size);   //For new user to register.
void user_login(USER_DATA* residents, int size);    //For existing user to check otp and own account details.
void parcel_collect(USER_DATA* residents, int size, LOCKER* locker, int lock);   //To match otp and collect parcel.
void parcel_deposit(USER_DATA* residents, int size, LOCKER* locker, int lock);   //For delivery man to load parcel.
void encrypt_decrypt(char* password, char types);
void write_file(USER_DATA* residents, int size, LOCKER* locker, int lock);

int main()
{
	char function;
	do {
		USER_DATA residents[MAX_RESIDENT] = { 0 };
		LOCKER locker[LOCK] = { 0 };
		int current_resident = 0;
		current_resident = read_file(residents, locker);

		cout << "\n ***************************************************************\n";
		cout << " *                                                             *\n";
		cout << " *                           WELCOME!                          *\n";
		cout << " *                                                             *\n";
		cout << " *  1.New User Registration                                    *\n";
		cout << " *  2.Login                                                    *\n";
		cout << " *  3.Collect Parcel                                           *\n";
		cout << " *  4.Deposit Parcel (Courier use only)                        *\n";
		cout << " *  5.Exit					               *\n";
		cout << " *                                                             *\n";
		cout << " *                                                             *\n";
		cout << " ***************************************************************\n";

		cout << "\n Please enter the number for your choice: ";
		cin >> function;

		switch (function) {
		case '1': {		system("CLS");		current_resident = user_registeration(residents, current_resident);	break; }
		case '2': {		system("CLS");		user_login(residents, current_resident);	break; }
		case '3': {		system("CLS");		parcel_collect(residents, current_resident, locker, LOCK);	break; }
		case '4': {		system("CLS");		parcel_deposit(residents, current_resident, locker, LOCK); 	break; }
		case '5': {		break;											  }
		default: {
			cout << "\n ERROR: Input only valid between " << 1 << " and " << MAX_SELECTION << endl << endl;
			system("pause");
			break;
		}
		}
		system("CLS");

	} while (function != '5');

	cout << "\n ***************************************************************\n";
	cout << " *                                                             *\n";
	cout << " *                                                             *\n";
	cout << " *                                                             *\n";
	cout << " *            THANK YOU FOR USING OUR SERVICE!                 *\n";
	cout << " *                 HOPE TO SEE YOU AGAIN!                      *\n";
	cout << " *        					               *\n";
	cout << " *                                                             *\n";
	cout << " *                                                             *\n";
	cout << " ***************************************************************\n";

	return 0;
}

int read_file(USER_DATA* residents, LOCKER* locker)
{
	int size = 0, lock = 0;
	ifstream in_file("resident_data.txt");

	if (!in_file)
		cout << "Error input file open (resident_data.txt)\n";
	else
	{
		in_file.getline(residents[size].res_name, 50);    //Get name from resident_data.txt
		while (in_file)
		{
			in_file.getline(residents[size].res_password, 15);
			encrypt_decrypt(residents[size].res_password, 'd');     //Decrypt the password after getting it from the file.

			in_file.getline(residents[size].res_contact, 12);
			in_file >> residents[size].res_unit_no;
			in_file >> residents[size].lock_id;
			in_file >> residents[size].res_otp;
			size++;

			if (in_file.peek() == '\n')
				in_file.ignore(256, '\n');

			in_file.getline(residents[size].res_name, 50);
		}
		in_file.close();
	}

	ifstream in_file2("locker.txt");

	if (!in_file2)
		cout << "Error input file open (locker.txt)\n";
	else
	{
		in_file2 >> locker[lock].id;
		while (in_file2) //Get data from locker.txt
		{
			string temp;
			in_file2 >> temp;
			if (temp == "true")
				locker[lock].lock_status = true;
			else
				locker[lock].lock_status = false;

			in_file2 >> locker[lock].lock_otp;
			lock++;

			if (in_file2.peek() == '\n')
				in_file2.ignore(256, '\n');

			in_file2 >> locker[lock].id;
		}
		in_file2.close();
	}

	return size;
}

int user_registeration(USER_DATA* residents, int size)
{
	int name_len, pass_len, cont_len, unit_no;
	char name[50], contact[12];
	int i = 0;
	char word = ' ';

	cout << "\n ********************** USER REGISTRATION **********************\n";
	while (getchar() != '\n');
	cout << "\n     Name: ";
	cin.getline(name, 50);
	name_len = strlen(name);   //Get the length of name.

	bool digit = true;
	do {
		cout << "\n     Contact number (without '-'): ";
		cin.getline(contact, 12);
		cont_len = strlen(contact);    //Get the length of contact.

		for (int z = 0; z < cont_len; z++) {    //Check if contact number input is valid.
			if (!isdigit(contact[z])) {
				digit = false;
				cout << "\n ERROR: Input only can only consist numerics (0 to 9)" << endl << endl;
				break;
			}
			else digit = true;
		}
	} while (!digit);

	bool same_user = false;     //Check if the user register before.
	for (int j = 0; j < size; j++) {
		if (strcmp(name, residents[j].res_name) == 0 || strcmp(contact, residents[j].res_contact) == 0)
		{
			cout << "\n This user had registered before.\n Please call our customer service for any enquiries.\n";
			same_user = true; break;
		}
		else
			same_user = false;
	}

	if (!same_user) {
		size++;
		strcpy_s(residents[size].res_name, name);
		for (int c = 0; c < 12; c++)
			residents[size].res_contact[c] = contact[c];
		do {
			cout << "\n     Unit number : ";
			cin >> residents[size].res_unit_no;

			if (residents[size].res_unit_no < 10 || residents[size].res_unit_no > 99)      //Unit number validation.
				cout << "\n ERROR: Invalid unit number (Unit no. between 10 to 99)\n\n";

		} while (residents[size].res_unit_no < 10 || residents[size].res_unit_no > 99);

		cout << "\n     Password : ";
		while (word != 13) {
			word = _getch();
			if (word != 13) {
				residents[size].res_password[i] = word;
				cout << "*";
				i++;
			}
		}
		pass_len = strlen(residents[size].res_password);

		residents[size].lock_id = 0;
		strcpy_s(residents[size].res_otp, "0");

		ofstream out_res;
		out_res.open("resident_data.txt", ios::app);
		if (!out_res)
			cout << " Errors opening resident files";
		else {      //Write new resident information into resident_data.txt
			out_res << "\n";
			out_res.write(residents[size].res_name, name_len) << "\n";
			encrypt_decrypt(residents[size].res_password, 'e');
			out_res.write(residents[size].res_password, pass_len) << "\n";
			out_res.write(residents[size].res_contact, cont_len) << "\n";
			out_res << residents[size].res_unit_no << "\n"
				<< residents[size].lock_id << "\n"
				<< residents[size].res_otp;

			encrypt_decrypt(residents[size].res_password, 'd');

			cout << "\n\n Registered successfully!\n";
		}
		out_res.close();
	}
	system("Pause");

	return size;
}

void user_login(USER_DATA* residents, int size)
{
	int  i;
	char name[50] = " ", password[15] = "", con_log = 'n';
	do {
		cout << "\n *************************** USER LOGIN ***************************\n";

		bool match = false;
		while (getchar() != '\n');
		cout << "\n     Enter User Name: ";
		cin.getline(name, 50);

		cout << "\n     Enter Password : ";
		int z = 0;
		char word = ' ';
		while (word != 13) {
			word = _getch();

			if (word != 13) {
				password[z] = word;
				cout << "*";
				z++;
			}
		}

		for (i = 0; i < size; i++)
		{
			if (strcmp(password, residents[i].res_password) == 0 && strcmp(name, residents[i].res_name) == 0)     //Username and password validation.
			{
				system("CLS");
				cout << "\n ********************** Successfully log in **********************\n\n";
				cout << "     Name :" << residents[i].res_name << endl;
				cout << "\n     Contact no :" << residents[i].res_contact << endl;
				cout << "\n     Resident Unit :" << residents[i].res_unit_no << endl;
				cout << "\n     Locker number :" << residents[i].lock_id << endl;
				cout << "\n     Otp :" << residents[i].res_otp << endl << endl;
				match = true;
				break;
			}
		}
		if (!match)
			cout << endl << "\n Incorrect unit number or password./ User doesn't exist.";

		cout << "\n Enter Y to login again(Any key to exit) : ";
		cin >> con_log;
		system("CLS");

	} while (con_log == 'y' || con_log == 'Y');

	return;
}

void parcel_collect(USER_DATA* residents, int size, LOCKER* locker, int lock)
{
	int lock_num, l;
	char otp[7], con_col = 'n';
	bool match = false, valid = true;
	cout << "\n ********************** PARCEL COLLECTION **********************\n";
	do {
		con_col = 'n';
		do {
			cout << "\n     Locker number (1-30):";
			cin >> lock_num;
			if (lock_num < 1 || lock_num > 30)         //Check if locker number input is correct.
				cout << "\n     Invalid locker number.Please re-enter.";
		} while (lock_num < 1 || lock_num > 30);

		if (locker[lock_num - 1].lock_status) {
			cout << "\n Locker are empty\n Enter Y to re-enter. (any key to exit): ";        //Check locker status.
			cin >> con_col;
			valid = false;
		}
		else
			valid = true;
	} while (con_col == 'y' || con_col == 'Y');

	if (valid)
	{
		lock_num -= 1;    //locker index number.
		do
		{
			con_col = 'n';
			cout << "\n     OTP:";
			cin >> otp;
			if (strcmp(locker[lock_num].lock_otp, otp) != 0)
			{
				cout << "\n OTP do not match.\n Enter Y to re-enter. (any key to exit):";
				cin >> con_col;
			}
			else
			{
				match = true;
				for (int i = 0; i < size; i++)
				{
					if (strcmp(residents[i].res_otp, otp) == 0)
					{
						residents[i].lock_id = 0;            //reset resident's otp
						strcpy_s(residents[i].res_otp, "0");
					}
				}
				locker[lock_num].lock_status = true;        //reset locker's status and otp
				strcpy_s(locker[lock_num].lock_otp, "0");
				
				write_file(residents, size, locker, LOCK);

				cout << "\n OTP matched.Please take your parcel.\n Thank you.\n";
				system("pause");
			}
		} while (con_col == 'y' || con_col == 'Y');
	}
}

void parcel_deposit(USER_DATA* residents, int size, LOCKER* locker, int lock)
{
	int unit_num, lock_num;
	char con_dep = 'n';

	cout << "\n ********************** PARCEL DEPOSITION **********************\n";
	do {
		cout << "\n   Please enter unit number : ";
		do {
			cin >> unit_num;
			if (unit_num < 10 || unit_num > 99)       //check if unit number input is within range.
				cout << "\n Invalid unit number.Please re-enter:";

		} while (unit_num < 10 || unit_num > 99);

		bool found = false;
		int unit_index = 0;
		for (int i = 0; i < size; i++)       //check if the resident had register.
		{
			if (unit_num == residents[i].res_unit_no)
			{
				found = true;
				unit_index = i;
			}
		}

		if (!found)
			cout << "\n ERROR: Owner of unit " << unit_num << " have not register an account.\n Enter Y to re-enter (any key to exit): ";
		else
		{
			if (residents[unit_index].lock_id != 0)
				cout << "\n One unit only allowed to deposit one parcel.\n Enter Y to re-enter (any key to exit): ";
			else {
				lock_num = 1;
				do {
					cout << "\n   Please enter locker number (1-30):";
					cin >> lock_num;

					if (lock_num < 1 || lock_num > 30)      //check if locker number input is within range.
						cout << "\n Invalid locker number.Please re-enter:";
					else if (!locker[lock_num - 1].lock_status)      //check locker status.
						cout << "\n Locker are full.\n Please choose another locker:";

				} while (!locker[lock_num - 1].lock_status || (lock_num < 1 || lock_num > 30));

				int l = 0;
				char otp[6];
				bool repeat = false;
				do
				{
					srand(time(NULL));

					for (int i = 0; i < 6; i++)
						otp[i] = rand() % 10 + '0';

					for (l = 0; l < LOCK; l++)   //Check if there is any repeated otp.
					{
						if (strcmp(otp, locker[l].lock_otp) == 0) {
							repeat = true;
							break;
						}
					}

				} while (repeat);

				for (int s = 0; s < size; s++)   //Check account that stored the same unit number then store otp and locker number.
				{
					if (unit_num == residents[s].res_unit_no)
					{
						residents[s].lock_id = lock_num;
						for (int z = 0; z < 6; z++)
							residents[s].res_otp[z] = otp[z];
					}
				}

				locker[lock_num - 1].lock_status = false;    //Store otp to corresponding locker.
				for (int z = 0; z < 6; z++)
					locker[lock_num - 1].lock_otp[z] = otp[z];

				write_file(residents, size, locker, LOCK);

				cout << "\n Parcel loaded successfully.\n Continue parcel deposition? (Y for YES/ any key to exit): ";
			}
		}
		cin >> con_dep;
	} while (con_dep == 'y' || con_dep == 'Y');
}

void encrypt_decrypt(char* password, char types) {

	int i;
	//encryption
	if (types == 'e') {
		for (i = 0; i < strlen(password); ++i)
		{
			password[i] = password[i] - KEY;
		}
	}
	//decryption
	else if (types == 'd') {
		for (i = 0; i < strlen(password); ++i)
		{
			password[i] = password[i] + KEY;
		}
	}
	else
		cout << "\n Error : invalid operation (encryption/decryption)" << endl;
}

void write_file(USER_DATA* residents, int size, LOCKER* locker, int lock)
{
	ofstream out_lock;
	ofstream out_res;
	out_lock.open("locker.txt", ios::out);
	out_res.open("resident_data.txt", ios::out);

	if (!out_lock)
		cout << " Errors opening locker files";
	if (!out_res)
		cout << " Errors opening resident files";

	int name_len, pass_len, cont_len;
	for (int i = 0; i < size; i++)
	{
		name_len = strlen(residents[i].res_name);
		pass_len = strlen(residents[i].res_password);
		cont_len = strlen(residents[i].res_contact);

		if (i != 0)
			out_res << "\n";
		out_res.write(residents[i].res_name, name_len) << "\n";
		encrypt_decrypt(residents[i].res_password, 'e');     //Encrypt the password before writing it to the file.
		out_res.write(residents[i].res_password, pass_len) << "\n";
		out_res.write(residents[i].res_contact, cont_len) << "\n";
		out_res << residents[i].res_unit_no << "\n"
			<< residents[i].lock_id << "\n"
			<< residents[i].res_otp;

		encrypt_decrypt(residents[i].res_password, 'd');     //Decrypt the password.
	}
	out_res.close();

	for (int j = 0; j < LOCK; j++)
	{
		if (j != 0)
			out_lock << "\n";

		string status;
		out_lock << locker[j].id << "\n";
		if (locker[j].lock_status == true)
			status = "true";
		else
			status = "false";
		out_lock << status << "\n";
		out_lock << locker[j].lock_otp;
	}
	out_lock.close();
}


