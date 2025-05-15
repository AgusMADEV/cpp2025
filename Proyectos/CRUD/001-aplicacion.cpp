#include <iostream>
#include <string>
#include "sqlite3.h"

using namespace std;

// Colores ANSI
const string COLOR_VERDE = "\033[32m";
const string COLOR_ROJO = "\033[31m";
const string COLOR_AMARILLO = "\033[33m";
const string COLOR_RESET = "\033[0m";

sqlite3* db = nullptr;

void ejecutar_sql(const string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error en SQL: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

void crear_tabla() {
    string sql = "CREATE TABLE IF NOT EXISTS clientes ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "nombre TEXT NOT NULL, "
                 "apellido TEXT NOT NULL, "
                 "email TEXT NOT NULL, "
                 "telefono TEXT NOT NULL);";
    ejecutar_sql(sql);
}

void crear_cliente() {
    string nombre, apellido, email, telefono;
    cout << "Nombre: ";
    getline(cin, nombre);
    cout << "Apellido: ";
    getline(cin, apellido);
    cout << "Email: ";
    getline(cin, email);
    cout << "Teléfono: ";
    getline(cin, telefono);

    string sql = "INSERT INTO clientes (nombre, apellido, email, telefono) VALUES (?,?,?,?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nombre.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, apellido.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, telefono.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << COLOR_VERDE << "Cliente creado correctamente." << COLOR_RESET << endl;
        } else {
            cout << COLOR_ROJO << "Error al crear cliente." << COLOR_RESET << endl;
        }
    } else {
        cout << COLOR_ROJO << "Error en la preparación de la sentencia." << COLOR_RESET << endl;
    }
    sqlite3_finalize(stmt);
}

int callback_mostrar(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << COLOR_AMARILLO << azColName[i] << ": " << COLOR_RESET << argv[i] << "\t";
    }
    cout << endl;
    return 0;
}

void leer_clientes() {
    string sql = "SELECT * FROM clientes;";
    cout << COLOR_VERDE << "--- LISTA DE CLIENTES ---" << COLOR_RESET << endl;
    ejecutar_sql(sql);  // Sin callback no muestra
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback_mostrar, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error mostrando clientes: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

void actualizar_cliente() {
    string id;
    cout << "ID del cliente a actualizar: ";
    getline(cin, id);

    string nombre, apellido, email, telefono;
    cout << "Nuevo nombre: ";
    getline(cin, nombre);
    cout << "Nuevo apellido: ";
    getline(cin, apellido);
    cout << "Nuevo email: ";
    getline(cin, email);
    cout << "Nuevo teléfono: ";
    getline(cin, telefono);

    string sql = "UPDATE clientes SET nombre=?, apellido=?, email=?, telefono=? WHERE id=?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, nombre.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, apellido.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, telefono.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, stoi(id));

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << COLOR_VERDE << "Cliente actualizado correctamente." << COLOR_RESET << endl;
        } else {
            cout << COLOR_ROJO << "Error al actualizar cliente." << COLOR_RESET << endl;
        }
    } else {
        cout << COLOR_ROJO << "Error en la preparación de la sentencia." << COLOR_RESET << endl;
    }
    sqlite3_finalize(stmt);
}

void borrar_cliente() {
    string id;
    cout << "ID del cliente a borrar: ";
    getline(cin, id);

    string sql = "DELETE FROM clientes WHERE id=?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, stoi(id));

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << COLOR_VERDE << "Cliente borrado correctamente." << COLOR_RESET << endl;
        } else {
            cout << COLOR_ROJO << "Error al borrar cliente." << COLOR_RESET << endl;
        }
    } else {
        cout << COLOR_ROJO << "Error en la preparación de la sentencia." << COLOR_RESET << endl;
    }
    sqlite3_finalize(stmt);
}

void mostrar_menu() {
    cout << COLOR_AMARILLO << "\n--- MENU CRUD DE CLIENTES ---" << COLOR_RESET << endl;
    cout << "1. Crear cliente" << endl;
    cout << "2. Listar clientes" << endl;
    cout << "3. Actualizar cliente" << endl;
    cout << "4. Borrar cliente" << endl;
    cout << "5. Salir" << endl;
    cout << "Elige una opción: ";
}

int main() {
    int opcion;
    string input;

    int rc = sqlite3_open("clientes.db", &db);
    if (rc) {
        cerr << COLOR_ROJO << "No se pudo abrir la base de datos." << COLOR_RESET << endl;
        return 1;
    }

    crear_tabla();

    do {
        mostrar_menu();
        getline(cin, input);
        try {
            opcion = stoi(input);
        } catch (...) {
            opcion = 0;
        }

        switch (opcion) {
            case 1: crear_cliente(); break;
            case 2: leer_clientes(); break;
            case 3: actualizar_cliente(); break;
            case 4: borrar_cliente(); break;
            case 5: cout << COLOR_VERDE << "Saliendo del programa..." << COLOR_RESET << endl; break;
            default: cout << COLOR_ROJO << "Opción no válida, intenta de nuevo." << COLOR_RESET << endl;
        }
    } while (opcion != 5);

    sqlite3_close(db);
    return 0;
}
