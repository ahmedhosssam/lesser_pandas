/*
 * Copyright (c) 2025 Ahmed Hossam <https://github.com/ahmedhosssam>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LESSER_PANDAS
#define LESSER_PANDAS

#include <filesystem> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <iomanip>
#include <algorithm>
#include <type_traits>
using namespace std;

/**
 * @brief Represents a single column in a DataFrame with associated operations.
 * 
 * The Column class stores data as strings internally but can handle different data types
 * (int, float, string) through automatic type detection and conversion. It provides
 * statistical operations, filtering, and data manipulation methods.
 */
class Column {
public:
    string name; // column's name
    vector<string> data; // column's data
    string dtype = "string"; // data type
    friend std::ostream& operator<<(std::ostream& os, const Column& col);

    /**
     * @brief Prints the column data with optional row limiting and tail functionality.
     * 
     * @param rows_cnt Number of rows to print (0 = print all rows)
     * @param is_tail If true, prints the last N rows instead of first N rows
     */
    void print(int rows_cnt = 0, bool is_tail = false) const {
        if (data.size() == 0) {
            return;
        }

        if (rows_cnt == 0) {
            rows_cnt = data.size()-1;
        }

        cout << name << endl;

        for(size_t i = 0; i < name.length(); i++) {
            cout << '-';
        }

        cout << endl;

        if (is_tail) {
            for(size_t idx = data.size()-rows_cnt; idx < data.size(); idx++) {
                cout << data[idx] << endl;
            }
        } else {
            for(int idx = 0; idx < rows_cnt; idx++) {
                cout << data[idx] << endl;
            }
        }

        cout << "\nPrinted: " << rows_cnt << " rows\n";
    }

    /**
     * @brief Displays the first N rows of the column.
     * 
     * @param rows_cnt Number of rows to display from the beginning (default: 5)
     */
    void head(int rows_cnt = 5) const {
        print(rows_cnt);
    }

    /**
     * @brief Displays the last N rows of the column.
     * 
     * @param rows_cnt Number of rows to display from the end (default: 5)
     */
    void tail(int rows_cnt = 5) const {
        print(rows_cnt, true);
    }

    /**
     * @brief Calculates the arithmetic mean of numeric column data.
     * 
     * @return The mean value as a double
     * @throws invalid_argument If the column dtype is not "int" or "float"
     * @note Empty/missing values are excluded from the calculation
     */
    double mean() const {
        if (dtype == "int" || dtype == "float") {
            double sum = 0;
            double col_size = static_cast<double> (data.size());
            for(const string& element : data) {
                if (element.length() == 0) {
                    // missing element
                    col_size--;
                    continue;
                }
                double num = stod(element);
                sum += num;
            }
            return sum / col_size;
        }

        throw invalid_argument("Invalid type: Column::mean() expects `dtype` to be int or float");
    }

    /**
     * @brief Calculates the sum of the column data.
     * 
     * @return The sum value as a double
     * @throws invalid_argument If the column dtype is not "int" or "float"
     * @note Empty/missing values are excluded from the calculation
     */
    double sum() const {
        if (dtype == "int" || dtype == "float") {
            double sum = 0;
            for(const string& element : data) {
                if (element.length() == 0) {
                    // missing element
                    continue;
                }
                double num = stod(element);
                sum += num;
            }
            return sum;
        }

        throw invalid_argument("Invalid type: Column::mean() expects `dtype` to be int or float");
    }
    
    /**
     * @brief Returns a sorted copy of the column data in ascending order.
     * 
     * @return Vector of strings containing sorted numeric values
     * @throws invalid_argument If the column dtype is "string"
     * @note The original column data remains unchanged
     */
    vector<string> sorted() const {
        if (dtype == "string") {
            throw invalid_argument("Invalid type: Column::Sorted() expects `dtype` to be int or float");
        }
        vector<string> result = data;

        std::sort(result.begin(), result.end(), [](string &s1, string &s2) {
            return stod(s1) < stod(s2);
        });
        
        return result;
    }

    /**
     * @brief Finds the minimum value in the column.
     * 
     * @return The minimum value as a double
     * @throws invalid_argument If the column dtype is "string"
     */
    double min() {
        if (dtype == "string") {
            throw invalid_argument("Invalid type: Column::min() expects `dtype` to be int or float");
        }
        vector<string> sorted_col = sorted();
        double mn = stod(sorted_col[0]);

        return mn;
    }

    /**
     * @brief Finds the maximum value in the column.
     * 
     * @return The maximum value as a double
     * @throws invalid_argument If the column dtype is "string"
     */
    double max() {
        if (dtype == "string") {
            throw invalid_argument("Invalid type: Column::max() expects `dtype` to be int or float");
        }
        vector<string> sorted_col = sorted();
        double mx = stod(sorted_col[sorted_col.size()-1]);

        return mx;
    }

    /**
     * @brief Fills missing (empty) values in the column with a specified value.
     * 
     * @tparam T The type of the fill value (int, double, or string)
     * @param x The value to use for filling missing entries
     * @note For numeric types, the value is converted to string representation
     */
    template <typename T>
    void fillna(T x) {
        for (string &element : data) {
            if (element.length() == 0) {
                // missing element
                if (is_same<T, int>::value || is_same<T, double>::value) {
                    if (dtype == "int") {
                        x = static_cast<int>(x);
                    }
                    element = to_string(x);
                } else {
                    element = x;
                }
            }
        }
    }

    /**
     * @brief Equality comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements equal the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator==(const double& key) const {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) == key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    } 

    /**
     * @brief Inequality comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements are not equal to the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator!=(const double& key) {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) != key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    }

    /**
     * @brief Less-than comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements are less than the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator<(const double& key) {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) < key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    }

    /**
     * @brief Greater-than comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements are greater than the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator>(const double& key) {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) > key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    }

    /**
     * @brief Less-than-or-equal comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements are less than or equal to the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator<=(const double& key) {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) <= key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    }

    /**
     * @brief Greater-than-or-equal comparison operator for numeric columns.
     * 
     * @param key The numeric value to compare against
     * @return Vector of boolean values indicating which elements are greater than or equal to the key
     * @throws runtime_error If the column dtype is "string"
     */
    vector<bool> operator>=(const double& key) {
        if (dtype == "string") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            try {
                if (data[i].empty()) {
                    mask[i] = false; 
                } else {
                    mask[i] = (stod(data[i]) >= key); 
                }
            } catch (...) {
                mask[i] = false; 
            }
        }
        return mask;
    }

    /**
     * @brief Equality comparison operator for string columns.
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements equal the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator==(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] == key);
        }
        return mask;
    }

    /**
     * @brief Inequality comparison operator for string columns.
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements are not equal to the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator!=(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] != key);
        }
        return mask;
    }

    /**
     * @brief Less-than comparison operator for string columns (lexicographic order).
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements are lexicographically less than the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator<(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] < key);
        }
        return mask;
    }

    /**
     * @brief Greater-than comparison operator for string columns (lexicographic order).
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements are lexicographically greater than the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator>(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] > key);
        }
        return mask;
    }

    /**
     * @brief Less-than-or-equal comparison operator for string columns (lexicographic order).
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements are lexicographically less than or equal to the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator<=(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] <= key);
        }
        return mask;
    }

    /**
     * @brief Greater-than-or-equal comparison operator for string columns (lexicographic order).
     * 
     * @param key The string value to compare against
     * @return Vector of boolean values indicating which elements are lexicographically greater than or equal to the key
     * @throws runtime_error If the column dtype is "float" or "int"
     */
    vector<bool> operator>=(const string& key) {
        if (dtype == "float" || dtype == "int") {
           throw runtime_error("Error: Invalid comparison");
        }

        vector<bool> mask(data.size());
        for (size_t i = 0; i < data.size(); i++) {
            mask[i] = (data[i] >= key);
        }
        return mask;
    }
};

/**
 * @brief Checks if a string represents a valid integer.
 * 
 * @param s The string to test
 * @return True if the string can be converted to an integer, false otherwise
 */
bool is_integer(const string& s) {
    try {
        size_t pos;
        stoi(s, &pos);
        return pos == s.length();
    } catch (...) {
        return false;
    }
}

/**
 * @brief Checks if a string represents a valid floating-point number.
 * 
 * @param s The string to test
 * @return True if the string can be converted to a double, false otherwise
 */
bool is_float(const string& s) {
    try {
        size_t pos;
        stod(s, &pos);
        return pos == s.length();
    } catch (...) {
        return false;
    }
}

/**
 * @brief A DataFrame class for handling tabular data similar to pandas DataFrame.
 * 
 * The DataFrame class provides functionality to load CSV files, manipulate data,
 * perform filtering operations, and save results. It automatically detects column
 * data types and provides various data analysis methods.
 */
class DataFrame {
private:
    map<string, Column> col_data;
    vector<vector<string>> row_data; // used for printing
    string file_dir;
public:
    vector<string> columns;

    /**
     * @brief Default constructor for creating an empty DataFrame.
     */
    DataFrame() = default;
    
    /**
     * @brief Copy constructor for creating a DataFrame from another DataFrame.
     * 
     * @param other The DataFrame to copy from
     */
    DataFrame(const DataFrame& other) 
    : col_data(other.col_data), 
      row_data(other.row_data), 
      file_dir(other.file_dir), 
      columns(other.columns) {}

    /**
     * @brief Constructor that loads data from a CSV file.
     * 
     * @param new_file_dir Path to the CSV file to load
     * @throws runtime_error If the file cannot be found or opened
     * @note Automatically detects column data types (int, float, or string)
     * @note Handles missing values in CSV files
     */
    DataFrame(string new_file_dir) {
        file_dir = new_file_dir;
        ifstream file(file_dir);

        if (!file) {
            throw runtime_error("Error: File not found!");
        }

        string line;

        vector<Column> temp_data;

        int idx = 0;
        while(getline(file, line)) {
            char delim = ',';
            stringstream ss(line);
            string element;
            vector<string> temp_row;

            if (idx == 0) {
                idx = 1;
                while(getline(ss, element, delim)) {
                    Column col;
                    col.name = element;
                    columns.push_back(element);

                    temp_data.push_back(col);
                    temp_row.push_back(element);
                }
                row_data.push_back(temp_row);
                continue;
            }

            size_t jdx = 0;
            while(getline(ss, element, delim)) {
                temp_data[jdx].data.push_back(element);
                temp_row.push_back(element);
                jdx++;
            }

            if (temp_row.size() < row_data[0].size()) {
                // there is a missing element in that row (in the last column)
                temp_row.push_back("");
                temp_data[jdx].data.push_back("");
            }

            row_data.push_back(temp_row);
        }

        for (auto& col : temp_data) {
            bool all_int = true;
            bool all_float = true;
            for (const string& element : col.data) {
                if (element.length() == 0) {
                    // missing element
                    continue;
                }
                if (!is_integer(element)) {
                    all_int = false;
                }
                if (!is_float(element)) {
                    all_float = false;
                }
                if (!all_int && !all_float) {
                    break;
                }
            }
            if (all_int) {
                col.dtype = "int";
            } else if (all_float) {
                col.dtype = "float";
            } else {
                col.dtype = "string";
            }
        }

        for(Column col : temp_data) {
            col_data[col.name] = col;
        }
    }

    /**
     * @brief Prints the DataFrame with various formatting options.
     * 
     * @param rows_cnt Number of rows to print (0 = print all rows)
     * @param is_tail If 1, prints from the end; if 0, prints from the beginning
     * @param cols Vector of column names to print (empty = print all columns)
     * @throws std::out_of_range If any specified column is not found
     */
    void print(int rows_cnt = 0, int is_tail = 0, vector<string> cols = {}) const {
        if (rows_cnt == 0) {
            rows_cnt = row_data.size();
        }

        if (cols.size() == 0) {
            cols = columns;
        }

        vector<vector<string>> print_row_data;
        vector<string> col_name_row;
        size_t sz = 0;
        for(string col : cols) {
            bool found = false;
            for (auto it = col_data.rbegin(); it != col_data.rend(); ++it) {
                if (col == it->second.name) {
                    col_name_row.push_back(it->second.name);
                    sz = it->second.data.size();
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw std::out_of_range("Column not found!");
            }
        }
        print_row_data.push_back(col_name_row);

        for(size_t idx = 0; idx < sz; idx++) {
            vector<string> new_row;
            for(string col : cols) {
                for (auto it = col_data.rbegin(); it != col_data.rend(); ++it) {
                    if (col == it->second.name) {
                        new_row.push_back(it->second.data[idx]);
                        break;
                    }
                }
            }
            print_row_data.push_back(new_row);
        }

        size_t idx = 0;
        for(auto& row : print_row_data) {
            cout << std::left;
            if (is_tail && idx < static_cast<size_t>(row_data.size()-rows_cnt)) {
                if (idx == 0) {
                    for(auto& element : row) {
                        cout << setw(20) << element;
                    }
                    cout << endl;
                }
                idx++;
                continue;
            }
            for(auto& element : row) {
                cout << setw(20) << element;
            }
            cout << endl;
            idx++;
            if (idx == static_cast<size_t>(rows_cnt + 1) && !is_tail) {
                break;
            }

            if (idx == row_data.size() && is_tail) {
                break;
            }
        }
        cout << "\nPrinted: " << print_row_data.size() << " rows\n";
    }

    /**
     * @brief Displays the first N rows of the DataFrame.
     * 
     * @param rows_cnt Number of rows to display from the beginning (default: 5)
     */
    void head(int rows_cnt = 5) const {
        print(rows_cnt);
    };

    /**
     * @brief Displays the last N rows of the DataFrame.
     * 
     * @param rows_cnt Number of rows to display from the end (default: 5)
     */
    void tail(int rows_cnt = 5) const {
        print(rows_cnt, 1);
    };

    /**
     * @brief Renames columns in the DataFrame.
     * 
     * @param vec Vector of pairs containing (old_name, new_name) mappings
     * @throws std::out_of_range If any old column name is not found
     */
    void rename(vector<pair<string, string>> vec) {
        for(pair<string, string> col_pair : vec) {
            string old_col_name = col_pair.first;
            string new_col_name = col_pair.second;

            auto it = col_data.find(old_col_name);
            if (it != col_data.end()) {
                col_data[new_col_name] = it->second;
                col_data[new_col_name].name = new_col_name;
                col_data[new_col_name].dtype = col_data[new_col_name].dtype;
                col_data.erase(old_col_name);

                for(string &col : columns) {
                    if (col == old_col_name) {
                        col = new_col_name;
                    }
                }
                continue;
            }

            throw std::out_of_range("Column not found!");
        }
    }

    /**
     * @brief Fills all missing values in the DataFrame with a specified value.
     * 
     * @tparam T The type of the fill value (int, double, or string)
     * @param x The value to use for filling missing entries
     * @note Applies to all columns in the DataFrame
     */
    template <typename T>
    void fillna(T x) {
        for (auto it = col_data.begin(); it != col_data.end(); ++it) {
            for(string &element : it->second.data) {
                if (element.length() == 0) {
                    if (is_same<T, int>::value || is_same<T, double>::value) {
                        element = to_string(x);
                    } else {
                        element = x;
                    }
                }
            }
        }
    }

    /**
     * @brief Removes rows from the DataFrame where the specified column has missing values.
     * 
     * @param col The name of the column to check for missing values
     * @note Removes entire rows across all columns when the specified column has empty values
     */
    void dropna(string col) {
        vector<string> &data = col_data[col].data;
        vector<int> removed_idx;

        for(size_t idx = 0; idx < data.size(); idx++) {
            if (data[idx].length() == 0) {
                removed_idx.push_back(idx);
            }
        }

        int jdx = 0;
        for(size_t idx : removed_idx) {
            for (auto it = col_data.begin(); it != col_data.end(); ++it) {
                it->second.data.erase(it->second.data.begin() + idx - jdx);
            }
            jdx++;
        }
    }

    /**
     * @brief Saves the DataFrame to a CSV file with customizable options.
     *
     * This function allows saving the DataFrame to a specified file with various options:
     * - Custom separator for columns.
     * - Option to include or exclude column headers.
     * - Option to include or exclude row indices.
     * - Replace missing values with a custom string.
     * - Save only specific columns if specified.
     *
     * @param output_file The path to the output file where the DataFrame will be saved.
     * @param index Whether to include row indices in the output file (default: true).
     * @param sep The separator to use between columns (default: ",").
     * @param header Whether to include column headers in the output file (default: true).
     * @param na_rep The string to replace missing values (default: "").
     * @param selected_columns A vector of column names to save. If empty, all columns are saved (default: {}).
     * @throws std::runtime_error If the file cannot be opened for writing.
     * @throws std::out_of_range If any of the specified columns in `selected_columns` do not exist.
     */
    void save_to_csv(
        const string& output_file,
        bool index = true,
        const string& sep = ",",
        bool header = true,
        const string& na_rep = "",
        const vector <string>& selected_columns = {}
    ) const {
        std::filesystem::path file_path(output_file);
 
        // Create directories if they don't exist
        if (!file_path.parent_path().empty()) {
           std::filesystem::create_directories(file_path.parent_path());
        }
        ofstream file(output_file);
 
        if (!file) {
           throw runtime_error("Error: Unable to open file for writing!");
        }
 
        // Determine which columns to save
        vector < string > columns_to_save;
        if (selected_columns.empty()) {
           columns_to_save = columns;
        } else {
           // Validate selected columns
           for (const string & col_name: selected_columns) {
              if (col_data.find(col_name) == col_data.end()) {
                 throw std::out_of_range("Column not found: " + col_name);
              }
              columns_to_save.push_back(col_name);
           }
        }
 
        // handle header option
        if (header) {
           // index option
           if (index) {
              file << "index" << sep;
           }
           for (size_t i = 0; i < columns_to_save.size(); ++i) {
              file << columns_to_save[i];
              if (i < columns_to_save.size() - 1) {
                 file << sep;
              }
           }
           file << endl;
        }
 
        // Determine the number of rows
        size_t num_rows = 0;
        if (!columns_to_save.empty()) {
           const string & first_col = columns_to_save[0];
           num_rows = col_data.at(first_col).data.size();
         }
 
        // Write row data
        for (size_t idx = 0; idx < num_rows; ++idx) {
           if (index) {
              file << idx << sep;
           }
 
           for (size_t j = 0; j < columns_to_save.size(); ++j) {
              const string & col_name = columns_to_save[j];
              const auto & col = col_data.at(col_name);
 
              // Replace missing values with `na_rep` string
              string value = col.data[idx].empty() ? na_rep : col.data[idx];
              file << value;
 
              if (j < columns_to_save.size() - 1) {
                 file << sep;
              }
           }
           file << endl;
        }
 
        file.close();
        cout << "Data saved successfully to " << output_file << " with separator '" << sep << "'." << endl;
     }

    friend std::ostream& operator<<(std::ostream& os, const DataFrame& df);

    /**
     * @brief Filters the DataFrame using a boolean mask.
     * 
     * @param mask Vector of boolean values indicating which rows to include
     * @return Reference to a new filtered DataFrame
     * @throws std::out_of_range If the mask size doesn't match the number of data rows
     * @note Creates a new DataFrame with only the rows where mask is true
     * @note The header row is always preserved
     */
    DataFrame& operator[](const vector<bool> & mask) {
        size_t data_rows = row_data.size() - 1; // Exclude header
        if (mask.size() != data_rows) {
            throw std::out_of_range("Mask size does not match data rows!");
        }

        DataFrame *filtered_df = new DataFrame(*this);

        // Preserve the header row
        filtered_df->row_data.push_back(row_data[0]);

        // Filter data rows 
        for (size_t i = 0; i < data_rows; ++i) {
            if (mask[i]) {
                filtered_df->row_data.push_back(row_data[i + 1]); // Skip header
            }
        }

        // Rebuild columns from filtered data
        for (const auto& col_name : columns) {
            Column filtered_col;
            filtered_col.name = col_name;
            filtered_col.dtype = col_data.at(col_name).dtype;

            // Copy data based on mask (original data indices)
            for (size_t i = 0; i < data_rows; ++i) {
                if (mask[i]) {
                    filtered_col.data.push_back(col_data.at(col_name).data[i]);
                }
            }
            filtered_df->col_data[col_name] = filtered_col;
        }

        return *filtered_df;
    }

    /**
     * @brief Accesses a single column by name.
     * 
     * @param key The name of the column to access
     * @return Reference to the Column object
     * @throws std::out_of_range If the column name is not found
     */
    Column& operator[](const string& key) {
        auto it = col_data.find(key);
        if (it != col_data.end()) {
            return it->second;
        }
        throw std::out_of_range("Column not found!");
    }

    /**
     * @brief Displays specific columns of the DataFrame.
     * 
     * @param keys Vector of column names to display
     * @return A dummy Column object (for interface compatibility)
     * @note This method prints the specified columns and returns a dummy Column
     */
    Column operator[](const vector<string>& keys) {
        print(0, 0, keys);

        Column dummy;
        return dummy;
    }
};

ostream& operator<<(std::ostream& os, const DataFrame& df) {
    df.print();
    return os;
}

ostream& operator<<(std::ostream& os, const Column& col) {
    col.print();
    return os;
}

#endif
