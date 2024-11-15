#include <iostream>
using namespace std;
#include <sqlite3.h>

int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

int main() {
    sqlite3* db;
    char* errorMessage = 0;
    int exit;

    // Open (or create) the database
    exit = sqlite3_open("example.db", &db);
    if (exit) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return 1;
    } else {
        cout << "Opened database successfully!" << endl;
    }

    // SQL command to create a table
    string sql = "CREATE TABLE IF NOT EXISTS PERSON("
                      "ID INT PRIMARY KEY NOT NULL, "
                      "NAME TEXT NOT NULL, "
                      "AGE INT NOT NULL);";

    // Execute SQL command
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
    if (exit != SQLITE_OK) {
        cerr << "Error creating table: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "Table created successfully!" << endl;
    }

    // SQL command to insert data
    sql = "INSERT INTO PERSON (ID, NAME, AGE) VALUES (1, 'John', 25);"
          "INSERT INTO PERSON (ID, NAME, AGE) VALUES (2, 'Alice', 30);";

    // Execute SQL command to insert data
    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
    if (exit != SQLITE_OK) {
        cerr << "Error inserting data: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "Records inserted successfully!" << endl;
    }

    // SQL command to fetch data
    sql = "SELECT * FROM PERSON;";

    // Execute SQL command to fetch data
    exit = sqlite3_exec(db, sql.c_str(), callback, 0, &errorMessage);
    if (exit != SQLITE_OK) {
        cerr << "Error fetching data: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }

    // Close the database connection
    sqlite3_close(db);
    return 0;
}
