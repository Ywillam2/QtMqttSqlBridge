# QtMqttSqlBridge
A lightweight Qt 6 C++ library that acts as a bridge between MQTT messaging and MySQL databases, enabling automatic, secure persistence of raw MQTT JSON payloads into user-defined MySQL tables.

---

## ✨ Core Features
- **Minimalist Design**: No complex configuration files, no automatic JSON parsing, no dynamic table mapping. Full control of database schema is left to the user.
- **Direct JSON Storage**: Leverages MySQL's native `JSON` data type to store complete MQTT message payloads, preserving all raw data.
- **Thread-Safe & Secure**: Implements parameterized SQL queries to prevent SQL injection, with thread-safe database access for stable IoT data logging.
- **One-Click Table Creation**: Provides a helper tool to quickly generate a standard table structure for users who prefer a ready-to-use template.
- **Modular Architecture**: Encapsulated MySQL operation library (`dbManager`) for easy reuse and extension.
- **Complete GUI Tool**: Full-featured Qt Widgets UI for MQTT testing, monitoring, and database writing.

---

## 📁 Project Structure

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

### 3. `TestWidget` (Completed ✅)
A fully developed **Qt Widgets-based graphical user interface** for MQTT message testing, monitoring, subscription management, and database writing.

**Features included:**
- MQTT broker connection configuration
- Topic subscription & publishing
- Real-time message display & logging
- MySQL database connection configuration
- One-click automatic data insertion from MQTT messages to database
- Complete, ready-to-use testing workflow

---

## 🛠️ Environment Requirements
- **Qt 6.10.1+** (with Qt SQL and Qt MQTT modules)
- **CMake 3.16+**
- **MySQL 5.7.8+** (supports native JSON data type)
- **C++17** compliant compiler (MinGW / MSVC)

---

## 🚀 Build & Run
1. Clone the repository:
    ```bash
    git clone https://github.com/Ywillam2/QtMqttSqlBridge.git
    cd QtMqttSqlBridge

2. Create a build directory and run CMake:

```
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="path/to/Qt/6.10.3/mingw_64"
```



3. Build the project:

   ```
cmake --build .
   ```

   

4. Run the **TestWidget** GUI or the console test module.

   

------

## 📌 Usage

1. **Prepare Database**: Create a MySQL database and a table with at least two required fields:

   

   - A `VARCHAR` field to store the MQTT topic

   - A `JSON` field to store the raw message payload

     (Or use the built-in one-click table creation tool to generate a standard table)
     
     (Or use the built-in one-click table creation tool to generate a standard table)

   

2. **Configure Connections**: Set up your MySQL database credentials and MQTT broker address in the application.

   

3. **Subscribe & Monitor**: Subscribe to MQTT topics and view real-time messages.

   

4. **Automatic Data Writing**: The application automatically stores received MQTT messages into the MySQL database.

   

5. **Query Data**: Use MySQL's native JSON functions (e.g., `JSON_EXTRACT`, `JSON_CONTAINS`) to query and process the stored data as needed.

   

------

## 📄 License

This project is open-source under the **MIT License**. See the LICENSE file for details.

------

## 📌 Future Plans

- Add support for multiple MQTT topic subscriptions
- Implement data visualization and export functions
- Support batch message processing
- Optimize high-frequency IoT data logging performance
- Add configuration import/export

------

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the issues page if you want to contribute.

------

### 💡 Final Note

This project follows the principle of "user autonomy": the program only guarantees the correct writing of raw MQTT data, and all table structure optimization, index creation, and data processing are the responsibility of the user.