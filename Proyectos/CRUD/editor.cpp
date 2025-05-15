#include <iostream>
#include <string>
#include <sqlite3.h>
#include <cstring>
#include <vector>
#include <iomanip>

using namespace std;

// Colores ANSI
const string COLOR_VERDE = "\033[32m";
const string COLOR_ROJO = "\033[31m";
const string COLOR_AMARILLO = "\033[33m";
const string COLOR_AZUL = "\033[34m";
const string COLOR_MAGENTA = "\033[35m";
const string COLOR_CYAN = "\033[36m";
const string COLOR_RESET = "\033[0m";

sqlite3* db = nullptr;

void limpiar_pantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void ejecutar_sql(const string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error en SQL: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

int callback_mostrar(void* data, int argc, char** argv, char** azColName) {
    auto* firstRow = static_cast<bool*>(data);
    vector<int> columnWidths(argc, 0);

    if (*firstRow) {
        for (int i = 0; i < argc; i++) {
            columnWidths[i] = strlen(azColName[i]);
        }
        *firstRow = false;
    }

    for (int i = 0; i < argc; i++) {
        int length = strlen(argv[i] ? argv[i] : "NULL");
        if (columnWidths[i] < length) {
            columnWidths[i] = length;
        }
    }

    if (*firstRow) {
        for (int i = 0; i < argc; i++) {
            cout << "| " << setw(columnWidths[i]) << azColName[i] << " ";
        }
        cout << "|" << endl;

        for (int i = 0; i < argc; i++) {
            cout << "+-" << setfill('-') << setw(columnWidths[i]) << "" << "-";
        }
        cout << "+" << setfill(' ') << endl;
    }

    for (int i = 0; i < argc; i++) {
        cout << "| " << setw(columnWidths[i]) << (argv[i] ? argv[i] : "NULL") << " ";
    }
    cout << "|" << endl;

    return 0;
}

void listar_tablas() {
    string sql = "SELECT name FROM sqlite_master WHERE type='table';";
    cout << COLOR_VERDE << "--- LISTA DE TABLAS ---" << COLOR_RESET << endl;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), [](void* NotUsed, int argc, char** argv, char** azColName) -> int {
        for (int i = 0; i < argc; i++) {
            cout << "- " << argv[i] << endl;
        }
        return 0;
    }, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error mostrando tablas: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

void crear_tabla_personalizada() {
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    int numColumnas;
    cout << COLOR_AZUL << "Número de columnas: " << COLOR_RESET;
    cin >> numColumnas;
    cin.ignore(); // Limpiar el buffer

    vector<string> columnas;
    for (int i = 0; i < numColumnas; ++i) {
        string columna;
        cout << COLOR_AZUL << "Nombre de la columna " << i + 1 << ": " << COLOR_RESET;
        getline(cin, columna);
        cout << COLOR_AZUL << "Tipo de la columna " << i + 1 << " (ej: TEXT, INTEGER): " << COLOR_RESET;
        string tipo;
        getline(cin, tipo);
        columnas.push_back(columna + " " + tipo);
    }

    string sql = "CREATE TABLE IF NOT EXISTS " + nombreTabla + " (id INTEGER PRIMARY KEY AUTOINCREMENT, " + columnas[0];
    for (size_t i = 1; i < columnas.size(); ++i) {
        sql += ", " + columnas[i];
    }
    sql += ");";

    ejecutar_sql(sql);
    cout << COLOR_VERDE << "Tabla creada correctamente." << COLOR_RESET << endl;
}

void insertar_registro() {
    listar_tablas();
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    string sql = "PRAGMA table_info(" + nombreTabla + ");";
    vector<string> columnas;
    sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        vector<string>* cols = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            if (strcmp(azColName[i], "name") == 0 && strcmp(argv[i], "id") != 0) {
                cols->push_back(argv[i]);
            }
        }
        return 0;
    }, &columnas, nullptr);

    vector<string> valores;
    for (const auto& columna : columnas) {
        string valor;
        cout << COLOR_AZUL << "Valor para " << columna << ": " << COLOR_RESET;
        getline(cin, valor);
        valores.push_back(valor);
    }

    sql = "INSERT INTO " + nombreTabla + " (" + columnas[0];
    for (size_t i = 1; i < columnas.size(); ++i) {
        sql += ", " + columnas[i];
    }
    sql += ") VALUES ('" + valores[0] + "'";
    for (size_t i = 1; i < valores.size(); ++i) {
        sql += ", '" + valores[i] + "'";
    }
    sql += ");";

    ejecutar_sql(sql);
    cout << COLOR_VERDE << "Registro insertado correctamente." << COLOR_RESET << endl;
}

void buscar_registros() {
    listar_tablas();
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    string sql = "PRAGMA table_info(" + nombreTabla + ");";
    vector<string> columnas;
    sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        vector<string>* cols = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            if (strcmp(azColName[i], "name") == 0) {
                cols->push_back(argv[i]);
            }
        }
        return 0;
    }, &columnas, nullptr);

    string whereClause;
    cout << COLOR_AZUL << "Criterio de búsqueda (ej: nombre='Juan'): " << COLOR_RESET;
    getline(cin, whereClause);

    sql = "SELECT * FROM " + nombreTabla;
    if (!whereClause.empty()) {
        sql += " WHERE " + whereClause;
    }
    sql += ";";

    cout << COLOR_VERDE << "--- RESULTADOS DE LA BÚSQUEDA ---" << COLOR_RESET << endl;
    bool firstRow = true;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        return callback_mostrar(data, argc, argv, azColName);
    }, &firstRow, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error mostrando registros: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

void listar_registros() {
    listar_tablas();
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    string sql = "SELECT COUNT(*) FROM " + nombreTabla + ";";
    int numFilas = 0;
    sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        *static_cast<int*>(data) = atoi(argv[0]);
        return 0;
    }, &numFilas, nullptr);

    cout << COLOR_VERDE << "--- ESTADÍSTICAS DE LA TABLA ---" << COLOR_RESET << endl;
    cout << "Número de filas: " << numFilas << endl;

    sql = "SELECT * FROM " + nombreTabla + ";";
    cout << COLOR_VERDE << "--- LISTA DE REGISTROS ---" << COLOR_RESET << endl;
    bool firstRow = true;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        return callback_mostrar(data, argc, argv, azColName);
    }, &firstRow, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error mostrando registros: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    }
}

void actualizar_registro() {
    listar_tablas();
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    string id;
    cout << COLOR_AZUL << "ID del registro a actualizar: " << COLOR_RESET;
    getline(cin, id);

    string sql = "PRAGMA table_info(" + nombreTabla + ");";
    vector<string> columnas;
    sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        vector<string>* cols = static_cast<vector<string>*>(data);
        for (int i = 0; i < argc; i++) {
            if (strcmp(azColName[i], "name") == 0) {
                cols->push_back(argv[i]);
            }
        }
        return 0;
    }, &columnas, nullptr);

    vector<string> valores;
    for (const auto& columna : columnas) {
        if (columna != "id") {
            string valor;
            cout << COLOR_AZUL << "Nuevo valor para " << columna << " (dejar vacío para no cambiar): " << COLOR_RESET;
            getline(cin, valor);
            if (!valor.empty()) {
                valores.push_back(columna + "='" + valor + "'");
            }
        }
    }

    if (!valores.empty()) {
        sql = "UPDATE " + nombreTabla + " SET " + valores[0];
        for (size_t i = 1; i < valores.size(); ++i) {
            sql += ", " + valores[i];
        }
        sql += " WHERE id=" + id + ";";
        ejecutar_sql(sql);
        cout << COLOR_VERDE << "Registro actualizado correctamente." << COLOR_RESET << endl;
    } else {
        cout << COLOR_AMARILLO << "No se realizaron cambios." << COLOR_RESET << endl;
    }
}

void borrar_registro() {
    listar_tablas();
    string nombreTabla;
    cout << COLOR_AZUL << "Nombre de la tabla: " << COLOR_RESET;
    getline(cin, nombreTabla);

    string id;
    cout << COLOR_AZUL << "ID del registro a borrar: " << COLOR_RESET;
    getline(cin, id);

    string sql = "DELETE FROM " + nombreTabla + " WHERE id=" + id + ";";
    ejecutar_sql(sql);
    cout << COLOR_VERDE << "Registro borrado correctamente." << COLOR_RESET << endl;
}

void ejecutar_sql_directo() {
    string sql;
    cout << COLOR_AZUL << "Escribe tu declaración SQL: " << COLOR_RESET;
    getline(cin, sql);

    char* errMsg = nullptr;
    bool firstRow = true;
    int rc = sqlite3_exec(db, sql.c_str(), [](void* data, int argc, char** argv, char** azColName) -> int {
        return callback_mostrar(data, argc, argv, azColName);
    }, &firstRow, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << COLOR_ROJO << "Error en SQL: " << errMsg << COLOR_RESET << endl;
        sqlite3_free(errMsg);
    } else {
        cout << COLOR_VERDE << "SQL ejecutado correctamente." << COLOR_RESET << endl;
    }
}

void mostrar_menu() {
    cout << COLOR_MAGENTA << "\n--- MENU EDITOR DE SQLITE ---" << COLOR_RESET << endl;
    cout << "1. Crear tabla" << endl;
    cout << "2. Insertar registro" << endl;
    cout << "3. Buscar registros" << endl;
    cout << "4. Listar registros" << endl;
    cout << "5. Actualizar registro" << endl;
    cout << "6. Borrar registro" << endl;
    cout << "7. Ejecutar SQL" << endl;
    cout << "8. Salir" << endl;
    cout << COLOR_AZUL << "Elige una opción: " << COLOR_RESET;
}

int main(int argc, char* argv[]) {
    string dbName = "clientes.db"; // Valor por defecto

    // Procesar argumentos de línea de comandos
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-db") == 0 && i + 1 < argc) {
            dbName = argv[i + 1];
            ++i; // Saltar el siguiente argumento
        }
    }

    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc) {
        cerr << COLOR_ROJO << "No se pudo abrir la base de datos: " << dbName << COLOR_RESET << endl;
        return 1;
    }

    int opcion;
    string input;

    do {
        limpiar_pantalla();
        mostrar_menu();
        getline(cin, input);
        try {
            opcion = stoi(input);
        } catch (...) {
            opcion = 0;
        }

        switch (opcion) {
            case 1: crear_tabla_personalizada(); break;
            case 2: insertar_registro(); break;
            case 3: buscar_registros(); break;
            case 4: listar_registros(); break;
            case 5: actualizar_registro(); break;
            case 6: borrar_registro(); break;
            case 7: ejecutar_sql_directo(); break;
            case 8: cout << COLOR_VERDE << "Saliendo del programa..." << COLOR_RESET << endl; break;
            default: cout << COLOR_ROJO << "Opción no válida, intenta de nuevo." << COLOR_RESET << endl;
        }
        cout << COLOR_AZUL << "Presiona Enter para continuar..." << COLOR_RESET;
        cin.ignore();
        cin.get();
    } while (opcion != 8);

    sqlite3_close(db);
    return 0;
}
