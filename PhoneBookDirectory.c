#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// Structure to represent a contact
struct Contact {
    char firstName[50];
    char lastName[50];
    char phone[15];
    struct Contact* prev;
    struct Contact* next;
};

// Function to create a new contact
struct Contact* createContact(char firstName[], char lastName[], char phone[]) {
    struct Contact* newContact = (struct Contact*)malloc(sizeof(struct Contact));
    strcpy(newContact->firstName, firstName);
    strcpy(newContact->lastName, lastName);
    strcpy(newContact->phone, phone);
    newContact->prev = NULL;
    newContact->next = NULL;
    return newContact;
}

// Global pointers for the head and tail of the linked list
struct Contact* head = NULL;
struct Contact* tail = NULL;

// SQLite database pointer
sqlite3* db;

// Function to open the SQLite database
void openDatabase() {
    int rc = sqlite3_open("phone_directory.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");

        // Create the contacts table if it doesn't exist
        const char* createTableSQL = "CREATE TABLE IF NOT EXISTS Contacts ("
                                     "FirstName TEXT, "
                                     "LastName TEXT, "
                                     "Phone TEXT);";
        rc = sqlite3_exec(db, createTableSQL, 0, 0, 0);

        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(0);
        } else {
            fprintf(stdout, "Table created successfully\n");
        }
    }
}

// Function to close the SQLite database
void closeDatabase() {
    sqlite3_close(db);
}

// Function to insert a contact into the database
void insertIntoDatabase(char firstName[], char lastName[], char phone[]) {
    char insertSQL[150];
    sprintf(insertSQL, "INSERT INTO Contacts (FirstName, LastName, Phone) VALUES ('%s', '%s', '%s');",
            firstName, lastName, phone);

    int rc = sqlite3_exec(db, insertSQL, 0, 0, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stdout, "Contact inserted successfully into the database\n");
    }
}

// Function to insert a contact at the beginning of the list
void insertAtStart(char firstName[], char lastName[], char phone[]) {
    struct Contact* newContact = createContact(firstName, lastName, phone);

    if (head == NULL) {
        head = newContact;
        tail = newContact;
    } else {
        newContact->next = head;
        head->prev = newContact;
        head = newContact;
    }

    // Insert the contact into the database
    insertIntoDatabase(firstName, lastName, phone);
}

// Function to insert a contact at the end of the list
void insertAtEnd(char firstName[], char lastName[], char phone[]) {
    struct Contact* newContact = createContact(firstName, lastName, phone);

    if (tail == NULL) {
        head = newContact;
        tail = newContact;
    } else {
        newContact->prev = tail;
        tail->next = newContact;
        tail = newContact;
    }

    // Insert the contact into the database
    insertIntoDatabase(firstName, lastName, phone);
}

// Function to insert a contact at a specified position
void insertAtPosition(char firstName[], char lastName[], char phone[], int position) {
    if (position <= 0) {
        printf("Invalid position\n");
        return;
    }
    if (position == 1) {
        insertAtStart(firstName, lastName, phone);
    } else {
        struct Contact* newContact = createContact(firstName, lastName, phone);
        struct Contact* current = head;
        int i;
        for (i = 1; i < position - 1 && current != NULL; i++) {
            current = current->next;
        }
        if (current == NULL) {
            printf("Invalid position\n");
            return;
        }
        newContact->prev = current;
        newContact->next = current->next;
        if (current->next != NULL) {
            current->next->prev = newContact;
        }
        current->next = newContact;
    }

    // Insert the contact into the database
    insertIntoDatabase(firstName, lastName, phone);
}

// Function to display contacts from the start
void displayContacts() {
    struct Contact* current = head;
    while (current != NULL) {
        printf("First Name: %s, Last Name: %s, Phone: %s\n", current->firstName, current->lastName, current->phone);
        current = current->next;
    }

    // Display contacts from the SQLite database
    displayContactsFromDatabase();
}

// Function to delete a contact from any position
void deleteContact(int position) {
    if (head == NULL) {
        printf("List is empty\n");
        return;
    }
    if (position <= 0) {
        printf("Invalid position\n");
        return;
    }
    if (position == 1) {
        struct Contact* temp = head;
        head = head->next;
        if (head != NULL) {
            head->prev = NULL;
        }
        free(temp);
    } else {
        struct Contact* current = head;
        int i;
        for (i = 1; i < position && current != NULL; i++) {
            current = current->next;
        }
        if (current == NULL) {
            printf("Invalid position\n");
            return;
        }
        if (current == tail) {
            tail = tail->prev;
            tail->next = NULL;
        } else {
            current->prev->next = current->next;
            current->next->prev = current->prev;
        }
        free(current);
    }

    // Delete the contact from the database
    // You need to implement this part based on your existing deletion logic
    // Example: deleteFromDatabase(firstName, lastName);
}

// Function to search for a contact by first name
void searchContact(char firstName[]) {
    struct Contact* current = head;
    while (current != NULL) {
        if (strcmp(current->firstName, firstName) == 0) {
            printf("First Name: %s, Last Name: %s, Phone: %s\n", current->firstName, current->lastName, current->phone);
            return;
        }
        current = current->next;
    }
    printf("Contact not found\n");

    // Search for the contact in the database
    searchContactInDatabase(firstName);
}

// Function to sort the contacts in alphabetical order by first name
void sortContacts() {
    struct Contact* current = head;
    struct Contact* temp;
    int swapped;

    if (head == NULL) {
        return;
    }

    do {
        swapped = 0;
        current = head;

        while (current->next != NULL) {
            if (strcmp(current->firstName, current->next->firstName) > 0) {
                // Swap the contacts
                temp = createContact(current->firstName, current->lastName, current->phone);
                strcpy(current->firstName, current->next->firstName);
                strcpy(current->lastName, current->next->lastName);
                strcpy(current->phone, current->next->phone);
                strcpy(current->next->firstName, temp->firstName);
                strcpy(current->next->lastName, temp->lastName);
                strcpy(current->next->phone, temp->phone);
                free(temp);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);

    // Sort contacts in the database
    sortContactsInDatabase();
}

// Function to display contacts from the SQLite database
void displayContactsFromDatabase() {
    // Implement this function based on your database structure
    // Fetch contacts from the database and display them
}

// Function to search for a contact in the SQLite database
void searchContactInDatabase(char firstName[]) {
    // Implement this function based on your database structure
    // Search for the contact by first name in the database
}

// Function to delete a contact from the SQLite database
void deleteFromDatabase(char firstName[], char lastName[]) {
    // Implement this function based on your database structure
    // Delete the contact from the database using the given first name and last name
}

// Function to sort contacts in the SQLite database
void sortContactsInDatabase() {
    // Implement this function based on your database structure
    // Retrieve contacts from the database, perform sorting, and update the database
}

int main() {
    int choice;
    char firstName[50];
    char lastName[50];
    char phone[15];
    int position;

    // Initialize SQLite database
    openDatabase();

    while (1) {
        printf("\nPhone Directory Menu:\n");
        printf("1. Create a contact\n");
        printf("2. Insert a contact at the beginning\n");
        printf("3. Insert a contact at the end\n");
        printf("4. Insert a contact at a specific position\n");
        printf("5. Display contacts\n");
        printf("6. Delete a contact\n");
        printf("7. Search for a contact\n");
        printf("8. Sort contacts\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter First Name: ");
                scanf("%s", firstName);
                printf("Enter Last Name: ");
                scanf("%s", lastName);
                printf("Enter Phone: ");
                scanf("%s", phone);
                insertAtEnd(firstName, lastName, phone);
                break;
            case 2:
                printf("Enter First Name: ");
                scanf("%s", firstName);
                printf("Enter Last Name: ");
                scanf("%s", lastName);
                printf("Enter Phone: ");
                scanf("%s", phone);
                insertAtStart(firstName, lastName, phone);
                break;
            case 3:
                printf("Enter First Name: ");
                scanf("%s", firstName);
                printf("Enter Last Name: ");
                scanf("%s", lastName);
                printf("Enter Phone: ");
                scanf("%s", phone);
                insertAtEnd(firstName, lastName, phone);
                break;
            case 4:
                printf("Enter First Name: ");
                scanf("%s", firstName);
                printf("Enter Last Name: ");
                scanf("%s", lastName);
                printf("Enter Phone: ");
                scanf("%s", phone);
                printf("Enter position: ");
                scanf("%d", &position);
                insertAtPosition(firstName, lastName, phone, position);
                break;
            case 5:
                displayContacts();
                break;
            case 6:
                printf("Enter position to delete: ");
                scanf("%d", &position);
                deleteContact(position);
                break;
            case 7:
                printf("Enter First Name to search: ");
                scanf("%s", firstName);
                searchContact(firstName);
                break;
            case 8:
                sortContacts();
                break;
            case 9:
                // Free memory and exit
                closeDatabase();
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }

    // Close SQLite database before exiting
    closeDatabase();

    return 0;
}