// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

#define MAX_RECORDS 100

// Menu choices
#define EXPORT_TXT 1
#define UPDATE_RECORD 2
#define NEW_RECORD 3
#define DELETE_RECORD 4
#define DISPLAY_RECORDS 5
#define END_PROGRAM 6

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};                        // end structure clientData

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAllRecords(FILE *fPtr);
void clearInputBuffer(void);
void pauseConsole(void);

int main(int argc, char *argv[])
{
    (void)argc; // suppress unused parameter warning

    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    // fopen opens the file; exits if file cannot be opened
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        struct clientData blankClient = {0, "", "", 0.0};
        unsigned int i;

        // If file doesn't exist, try to create and initialize it
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL)
        {
            printf("%s: File could not be opened.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        // Initialize the file with MAX_RECORDS blank records
        for (i = 1; i <= MAX_RECORDS; ++i)
        {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        rewind(cfPtr);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != END_PROGRAM)
    {
        switch (choice)
        {
        case EXPORT_TXT:
            textFile(cfPtr);
            pauseConsole();
            break;
        case UPDATE_RECORD:
            updateRecord(cfPtr);
            pauseConsole();
            break;
        case NEW_RECORD:
            newRecord(cfPtr);
            pauseConsole();
            break;
        case DELETE_RECORD:
            deleteRecord(cfPtr);
            pauseConsole();
            break;
        case DISPLAY_RECORDS:
            displayAllRecords(cfPtr);
            pauseConsole();
            break;
        default:
            puts("Incorrect choice");
            pauseConsole();
            break;
        } // end switch
    }     // end while

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};

    // fopen opens the file; exits if file cannot be opened
    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
    } // end if
    else
    {
        rewind(readPtr); // sets pointer to beginning of file
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        // copy all records from random-access file into text file
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
        {
            // write single record to text file
            if (client.acctNum != 0)
            {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                        client.balance);
            } // end if
        }     // end while

        fclose(writePtr); // fclose closes the file
        puts("\nData successfully exported to accounts.txt.");
    }                     // end else
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    // create clientData with no information
    struct clientData client = {0, "", "", 0.0};

    // obtain number of account to update
    printf("Enter account to update ( 1 - %d ): ", MAX_RECORDS);
    if (scanf("%u", &account) != 1)
    {
        clearInputBuffer();
        puts("Invalid account number.");
        return;
    }

    if (account < 1 || account > MAX_RECORDS)
    {
        puts("Invalid account number.");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account does not exist
    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
    }
    else
    { // update record
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // request transaction amount from user
        printf("%s", "Enter charge ( + ) or payment ( - ): ");
        if (scanf("%lf", &transaction) != 1)
        {
            clearInputBuffer();
            puts("Invalid transaction amount.");
            return;
        }
        client.balance += transaction; // update record balance

        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

        // move file pointer to correct record in file
        // move back by 1 record length
        fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
        // write updated record over old record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        puts("\nAccount updated successfully.");
    } // end else
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};      // stores record read from file
    struct clientData blankClient = {0, "", "", 0.0}; // blank client
    unsigned int accountNum;                          // account number

    // obtain number of account to delete
    printf("Enter account number to delete ( 1 - %d ): ", MAX_RECORDS);
    if (scanf("%u", &accountNum) != 1)
    {
        clearInputBuffer();
        puts("Invalid account number.");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        puts("Invalid account number.");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if record does not exist
    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
    } // end if
    else
    { // delete record
        // move file pointer to correct record in file
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        // replace existing record with blank record
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        puts("\nAccount deleted successfully.");
    } // end else
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    // create clientData with default information
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    // obtain number of account to create
    printf("Enter new account number ( 1 - %d ): ", MAX_RECORDS);
    if (scanf("%u", &accountNum) != 1)
    {
        clearInputBuffer();
        puts("Invalid account number.");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_RECORDS)
    {
        puts("Invalid account number.");
        return;
    }

    // move file pointer to correct record in file
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    // read record from file
    fread(&client, sizeof(struct clientData), 1, fPtr);
    // display error if account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
    } // end if
    else
    { // create record
        // user enters last name, first name and balance
        printf("%s", "Enter lastname, firstname, balance\n? ");
        if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
        {
            clearInputBuffer();
            puts("Invalid data entered.");
            return;
        }

        client.acctNum = accountNum;
        // move file pointer to correct record in file
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        // insert record in file
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        puts("\nAccount created successfully.");
    } // end else
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice; // variable to store user's choice
    
    // Clear the console for a clean menu experience
    system("cls");

    // display available options
    printf("%s", "\nEnter your choice\n"
                 "1 - store a formatted text file of accounts called\n"
                 "    \"accounts.txt\" for printing\n"
                 "2 - update an account\n"
                 "3 - add a new account\n"
                 "4 - delete an account\n"
                 "5 - display all active accounts\n"
                 "6 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInputBuffer();
        return 0; // Return invalid choice
    }
    return menuChoice;
} // end function enterChoice

// clear input buffer to prevent infinite loops on invalid input
void clearInputBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // discard character
    }
}

// pause the console so the user can read messages before the screen clears
void pauseConsole(void)
{
    puts(""); // add a blank line for readability
    system("pause");
}

// display all active records in the console
void displayAllRecords(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    rewind(fPtr); // start from beginning
    printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
    puts("---------------------------------------------");

    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
    puts("---------------------------------------------");
}