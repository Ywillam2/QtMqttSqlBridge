# QtMqttSqlBridge
A lightweight Qt 6 C++ library that acts as a bridge between MQTT messaging and MySQL databases, enabling automatic, secure persistence of raw MQTT JSON payloads into user-defined MySQL tables.

---

## ✨ Core Features
- **Minimalist Design**: No complex configuration files, no automatic JSON parsing, no dynamic table mapping. Full control of database schema is left to the user.
- **Direct JSON Storage**: Leverages MySQL's native `JSON` data type to store complete MQTT message payloads, preserving all raw data.
- **Thread-Safe & Secure**: Implements parameterized SQL queries to prevent SQL injection, with thread-safe database access for stable IoT data logging.
- **One-Click Table Creation**: Provides a helper tool to quickly generate a standard table structure for users who prefer a ready-to-use template.
- **Modular Architecture**: Encapsulated MySQL operation library (`dbManager`) for easy reuse and extension.

---

## 📁 Project Structure
```
QtMqttSqlBridge/
├── dbManager/          # Encapsulated MySQL operation shared library (core)
├── Test/               # Standalone test module for core library functionality
├── TestWidget/         # Qt Widgets-based MQTT testing UI (WIP)
├── CMakeLists.txt      # Root CMake build configuration
└── .gitignore          # Git ignore rules for Qt/CMake projects
```

---

## 📝 Module Description
### 1. `dbManager` (Core Library)
The core encapsulated MySQL operation library, implemented with **Singleton Pattern** and **PIMPL (Private Implementation) Idiom** for clean, maintainable code. It provides:
- Secure connection management for MySQL databases
- Thread-safe SQL query execution
- Helper functions for table creation and data insertion
- Full support for MySQL's native JSON data type

### 2. `Test`
A standalone console test project to verify the core functionality of the `dbManager` library, independent of the UI module.

### 3. `TestWidget` (Work In Progress)
A Qt Widgets-based graphical user interface for MQTT message testing and monitoring.
> **Note**: This module is **not fully implemented yet**. It currently serves as a UI framework for future MQTT subscription, message visualization, and database writing features, which will be supplemented in subsequent updates.

---

## 🛠️ Environment Requirements
- **Qt 6.10.1+** (with Qt SQL and Qt MQTT modules)
- **CMake 3.16+**
- **MySQL 5.7.8+** (supports native JSON data type)
- **C++17** compliant compiler (MinGW / MSVC)

---

## 🚀 Build & Run
1.  Clone the repository:
    ```bash
    git clone https://github.com/Ywillam2/QtMqttSqlBridge.git
    cd QtMqttSqlBridge
    ```
2.  Create a build directory and run CMake:
    ```bash
    mkdir build && cd build
    cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="path/to/Qt/6.10.3/mingw_64"
    ```
3.  Build the project:
    ```bash
    cmake --build .
    ```
4.  Run the test application or integrate the `dbManager` library into your own project.

---

## 📌 Usage
1.  **Prepare Database**: Create a MySQL database and a table with at least two required fields:
    - A `VARCHAR` field to store the MQTT topic
    - A `JSON` field to store the raw message payload
    (Or use the built-in one-click table creation tool to generate a standard table)
2.  **Configure Connection**: Set up your MySQL database credentials and MQTT broker address in the application.
3.  **Subscribe & Write**: The application will automatically subscribe to the specified MQTT topic, receive messages, and write the topic + full JSON payload directly to your target table.
4.  **Query Data**: Use MySQL's native JSON functions (e.g., `JSON_EXTRACT`, `JSON_CONTAINS`) to query and process the stored data as needed.

---

## 📄 License
This project is open-source under the **MIT License**. See the [LICENSE](sslocal://flow/file_open?url=LICENSE&flow_extra=eyJsaW5rX3R5cGUiOiJjb2RlX2ludGVycHJldGVyIn0=) file for details.

---

## 📌 Future Plans
- Complete the `TestWidget` UI module with full MQTT subscription, real-time message display, and one-click database writing features
- Add support for multiple MQTT topic subscriptions
- Implement data visualization and export functions for stored MQTT data
- Optimize performance for high-frequency IoT data logging scenarios

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](sslocal://flow/file_open?url=https%3A%2F%2Fgithub.com%2FYwillam2%2FQtMqttSqlBridge%2Fissues&flow_extra=eyJsaW5rX3R5cGUiOiJjb2RlX2ludGVycHJldGVyIn0=) if you want to contribute.

---

### 💡 Final Note
This project follows the principle of "user autonomy": the program only guarantees the correct writing of raw MQTT data, and all table structure optimization, index creation, and data processing are the responsibility of the user.

---
