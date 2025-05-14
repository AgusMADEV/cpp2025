#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

// Estructura de configuración FTP
struct FTPConfig {
    std::string hostname;
    int port;
    std::string username;
    std::string password;
    std::vector<std::string> folders;
};

// Estructura de configuración de base de datos
struct DBConfig {
    std::string host;
    std::string port;
    std::string user;
    std::string password;
    std::vector<std::string> databases;
};

// Carpetas excluidas de la copia
std::vector<std::string> EXCLUDE_FOLDERS = {".git", "myphp", "node_modules", "evo-luciona.es"};

// Cargar configuración FTP desde archivo
FTPConfig load_ftp_config(const std::string& config_file) {
    FTPConfig config;
    std::ifstream file(config_file);
    std::string line;

    while (std::getline(file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            if (key == "hostname") config.hostname = value;
            else if (key == "port") config.port = std::stoi(value);
            else if (key == "username") config.username = value;
            else if (key == "password") config.password = value;
            else if (key == "folders") {
                std::stringstream ss(value);
                std::string folder;
                while (std::getline(ss, folder, ',')) {
                    config.folders.push_back(folder);
                }
            }
        }
    }

    return config;
}

// Cargar configuración de base de datos
DBConfig load_db_config(const std::string& config_file) {
    DBConfig config;
    std::ifstream file(config_file);
    std::string line;

    while (std::getline(file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            if (key == "host") config.host = value;
            else if (key == "port") config.port = value;
            else if (key == "user") config.user = value;
            else if (key == "password") config.password = value;
            else if (key == "databases") {
                std::stringstream ss(value);
                std::string db;
                while (std::getline(ss, db, ',')) {
                    config.databases.push_back(db);
                }
            }
        }
    }

    return config;
}

// Contar archivos en directorio (excluyendo carpetas)
int count_files_in_dir(const std::string& local_dir) {
    int count = 0;
    for (const auto& entry : fs::recursive_directory_iterator(local_dir)) {
        if (entry.is_regular_file()) {
            bool excluded = false;
            for (const auto& exclude_folder : EXCLUDE_FOLDERS) {
                if (entry.path().string().find(exclude_folder) != std::string::npos) {
                    excluded = true;
                    break;
                }
            }
            if (!excluded) {
                count++;
            }
        }
    }
    return count;
}

// Dump de base de datos MySQL
void dump_mysql_databases(const DBConfig& db_config, const std::string& output_dir) {
    for (const auto& db : db_config.databases) {
        std::string command = "mysqldump --host=" + db_config.host +
                              " --port=" + db_config.port +
                              " --user=" + db_config.user +
                              " --password=" + db_config.password +
                              " --databases " + db +
                              " --events --routines --triggers" +
                              " --result-file=" + output_dir + "/" + db + ".sql";
        system(command.c_str());
    }
}

// Mostrar barra de progreso en consola
void show_progress(int total, int current, const std::chrono::duration<double>& elapsed_time) {
    double percentage = (static_cast<double>(current) / total) * 100;
    int bar_width = 50;
    int pos = static_cast<int>(bar_width * percentage / 100);

    std::string progress_bar = "[" + std::string(pos, '#') + std::string(bar_width - pos, '-') + "]";
    double remaining_time = (elapsed_time.count() / current) * (total - current);
    int minutes = static_cast<int>(remaining_time) / 60;
    int seconds = static_cast<int>(remaining_time) % 60;

    std::cout << "\033[2J\033[1;1H"; // Limpiar consola
    std::cout << "\033[38;5;79mCopia de seguridad en progreso...\033[0m\n";
    std::cout << "\033[92mArchivos procesados: " << current << " / " << total << "\033[0m\n";
    std::cout << "\033[94mProgreso: " << progress_bar << " " << (int)percentage << "%\033[0m\n";
    std::cout << "\033[96mTiempo transcurrido: " << (int)elapsed_time.count() << "s\033[0m\n";
    std::cout << "\033[93mTiempo restante estimado: " << minutes << "m " << seconds << "s\033[0m\n";
}

// Simular transferencia de archivos vía SFTP
void transfer_files_via_sftp(const std::string& hostname, const std::string& username, const std::string& password, const std::vector<std::string>& local_folders, const std::string& remote_path) {
    int total_files = 0;
    for (const auto& folder : local_folders) {
        total_files += count_files_in_dir(folder);
    }

    int current_file = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (const auto& local_folder : local_folders) {
        for (const auto& entry : fs::recursive_directory_iterator(local_folder)) {
            if (entry.is_regular_file()) {
                bool excluded = false;
                for (const auto& exclude_folder : EXCLUDE_FOLDERS) {
                    if (entry.path().string().find(exclude_folder) != std::string::npos) {
                        excluded = true;
                        break;
                    }
                }
                if (excluded) continue;

                // Aquí puedes usar código real para enviar el archivo vía SFTP
                std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulación

                current_file++;
                auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
                show_progress(total_files, current_file, std::chrono::duration_cast<std::chrono::seconds>(elapsed_time));
            }
        }
    }
}

int main() {
    // Cargar configuraciones
    FTPConfig ftp_config = load_ftp_config("ftp_config.txt");
    DBConfig db_config = load_db_config("db_config.txt");

    // Mostrar cantidad de archivos a copiar
    int total_files = 0;
    for (const auto& folder : ftp_config.folders) {
        total_files += count_files_in_dir(folder);
    }
    std::cout << "Total de archivos a copiar: " << total_files << "\n";

    // Dump de bases de datos
    dump_mysql_databases(db_config, "mysql");

    // Transferencia de archivos
    transfer_files_via_sftp(ftp_config.hostname, ftp_config.username, ftp_config.password, ftp_config.folders, "backup");

    std::cout << "\n\033[92m✅ Backup completado correctamente.\033[0m\n";
    return 0;
}
