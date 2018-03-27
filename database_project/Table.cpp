//
//  Table.cpp
//  Project 3
//
//  Created by Jordan Katz on 3/11/18.
//  Copyright © 2018 Jordan Katz. All rights reserved.
//

#include <algorithm> // remove_if, find
#include <getopt.h> // getopt_long
#include <iostream> // cin, cout

#include "Table.h"


///////////////////////////////////////////////////////////////////////////
/////////////////////// Helper Function Definitions ///////////////////////
///////////////////////////////////////////////////////////////////////////

// FUNCTION: Parses the command line
bool get_options(int argc, char** argv) {
    bool is_quite_mode = false;
    
    static option long_options[] = {
        {"quiet", no_argument, nullptr, 'q'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr,  0}
    };
    
    int choice = 0;
    while ((choice = getopt_long(argc, argv, "qh", long_options, nullptr)) != -1) {
        switch (choice) {
            case 'q':
                is_quite_mode = true;
                break;
                
            case 'h':
                std::cout << "Help message: type ''-q'' or ''--quiet'' to enable quite mode";
                exit(0);
                
            default:
                throw "unrecognized option";
        }
    } // while
    
    return is_quite_mode;
}

// FUNCTION: prints error message
void print_error1(const string &tablename_in) {
    std::cout << "Error: Cannot create already existing table " << tablename_in << '\n';
}

// FUNCTION: prints error message
void print_error2(const string &tablename_in) {
    std::cout << "Error: " << tablename_in << " does not name a table in the database" << '\n';
}

// FUNCTION: prints error message
void print_error3(const string &tablename_in, const string &colname_in) {
    std::cout << "Error: " << colname_in << " does not name a column in " << tablename_in << '\n';
}

// INPUT: a char repesnting a type
// FUNCTION: creates a new "TableEntry" pointer initialized to specfied value
// OUTPUT: the created pointer
TableEntry* create_temp_entry(char type_in, const string val_in) {
    switch (type_in) {
        case 'b':
            if (val_in == "true") {return new TableEntry(true);}
            else if (val_in == "false") {return new TableEntry(false);}
        case 'i':
            return new TableEntry(stoi(val_in));
        case 's':
            return new TableEntry(val_in);
        case 'd':
            return new TableEntry(stod(val_in));
        default:
            return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////
/////////////////// "Table" Member Function Definitions ///////////////////
///////////////////////////////////////////////////////////////////////////

void Table::CREATE() {
    col_types.reserve(num_cols);
    
    for (int i = 0; i < num_cols; ++i) {
        string coltype_i;
        cin >> coltype_i;
        col_types.push_back(coltype_i[0]); // only care about first char
    }
    
    cout << "New table " << name << " with column(s) ";
    for (int i = 0; i < num_cols; ++i) {
        string colname_i;
        cin >> colname_i;
        cout << colname_i << " ";
        colname_to_idx_map[colname_i] = i;
    }
    cout << "created" << '\n';
    
    return;
}





void Table::INSERT_INTO(const int num_rows_to_add) {
    const int num_prior_rows = int(data.size());
    data.resize(num_prior_rows + num_rows_to_add);

    for (int r = 0; r < num_rows_to_add; ++r) { // for each row to be added
        data[r + num_prior_rows].reserve(this->num_cols);
        
        for (int c = 0; c < this->num_cols; ++c) { // for each the spots in the row
            char t = col_types[c];
            switch (t) {
                case 's': {
                    string val;
                    cin >> val;
                    data[r + num_prior_rows].emplace_back(TableEntry(val));
                    break;
                }
                case 'b': {
                    bool val;
                    cin >> val;
                    data[r + num_prior_rows].emplace_back(TableEntry(val));
                    break;
                }
                case 'i': {
                    int val;
                    cin >> val;
                    data[r + num_prior_rows].emplace_back(TableEntry(val));
                    break;
                }
                case 'd': {
                    double val;
                    cin >> val;
                    data[r + num_prior_rows].emplace_back(TableEntry(val));
                    break;
                }
                default:
                    break;
            } // switch
        } // for
        
        // check to see if we should update our indexes
        if (this->indexed_col != -1) {
            TableEntry* to_update = &data[r + num_prior_rows][indexed_col];
            int key_to_insert = r + num_prior_rows; // the row index
            if (this->bst_exists) {
                bst[*to_update].push_back(key_to_insert);
            }
            else if (this->hash_exists) {
                hash[*to_update].push_back(key_to_insert);
            }
        }
    } // for
    
    cout << "Added " << num_rows_to_add << " rows to " << name << " from position "
        << num_prior_rows << " to " << num_prior_rows + num_rows_to_add - 1 << '\n';
    
    return;
} // INSERT_INTO






void Table::DELETE_FROM(const int col_idx, const char op, const string value) {
    const int num_rows_before = int(data.size());
    const TableEntry* entry = create_temp_entry(col_types[col_idx], value);
    
    // delete all of the specified rows
    auto new_end = std::remove_if(data.begin(), data.end(), comp(*entry, col_idx, op));
    const int num_rows_after = int(std::distance(data.begin(), new_end));
    data.resize(num_rows_after);

    cout << "Deleted " << num_rows_before - num_rows_after << " rows from " << name << '\n';

    if (bst_exists) { GENERATE_INDEX('b', indexed_col); }
    else if (hash_exists) { GENERATE_INDEX('h', indexed_col); }
    
    delete entry; // clear up dynamic memory
    entry = nullptr;
    return;
} // DELETE_FROM






void Table::GENERATE_INDEX(const char h_or_b, const int col_idx) {
    const int num_rows = int(data.size());
    this->indexed_col = col_idx;

    if (this->hash_exists) {
        hash.clear();
        hash_exists = false;
    }
    if (this->bst_exists) {
        bst.clear();
        bst_exists = false;
    }

    switch (h_or_b) {
        case 'b': {
            for (int r = 0; r < num_rows; ++r) {
                TableEntry* key_val = &data[r][col_idx];
                bst[*key_val].push_back(r);
            }
            bst_exists = true;
            break;
        }
        case 'h': {
            for (int r = 0; r < num_rows; ++r) {
                TableEntry* key_val = &data[r][col_idx];
                hash[*key_val].push_back(r);
            }
            hash_exists = true;
            break;
        }
    }
    
    return;
} // GENERATE_INDEX





void Table::PRINT(const vector<string> &print_cols, const bool is_quiet_mode) {
    string all_or_where, colname_in, value_in;
    int rows_printed = 0;
    char op_in = '?';
    
    cin >> all_or_where; // equal to either "WHERE" or "ALL"
    
    if (all_or_where == "WHERE") {
        cin >> colname_in >> op_in >> value_in;
        if (colname_to_idx_map.find(colname_in) == colname_to_idx_map.end()) {
            print_error3(this->name, colname_in);
            return;
        }
    }
    // done reading in and checking input for errors
    
    // print column names
    if (is_quiet_mode == false) {
        for (auto &c : print_cols) {
            cout << c << " ";
        }
        cout << '\n';
    }
    
    if (all_or_where == "ALL") { // print every row
        rows_printed = int(data.size());
        
        if (is_quiet_mode == false) {
            for (auto &r : data) { // loop through every row
                for (auto &c : print_cols) { // loop through the specified columns
                    int col_idx = colname_to_idx_map[c];
                    cout << r[col_idx] << " ";
                }
                cout << '\n';
            }
        }
    }
    
    else { // only print certain rows
        int col_in_idx = colname_to_idx_map[colname_in];
        
        char entry_type = col_types[col_in_idx];
        TableEntry* entry = create_temp_entry(entry_type, value_in);
        
        if (this->bst_exists && this->indexed_col == col_in_idx) {
            for (auto &it : bst) { // for each of the it=<key, index> pairs in bst
                // if we should print the rows indexed by it.second, do so
                // (whether or not we should print a row depends on "op_in")
                
                bool print_current_rows = false;
                switch (op_in) {
                    case '=':
                        if (it.first == *entry) { print_current_rows = true; }
                        break;
                    case '<':
                        if (it.first < *entry) { print_current_rows = true; }
                        break;
                    case '>':
                        if (it.first > *entry) { print_current_rows = true; }
                        break;
                    default:
                        break;
                }
                
                if (print_current_rows == true) {
                    for (auto &r : it.second) { // loop through all of the row_idx's
                        rows_printed++;
                        
                        if (is_quiet_mode == false) {
                            for (auto &c : print_cols) { // loop through the specified columns
                                int col_idx = colname_to_idx_map[c];
                                cout << data[r][col_idx] << " ";
                            }
                            cout << '\n';
                        }
                        
                    } // for
                } // if
            } // for
        } // if (bst exists)
        
        else {
            for (auto &r : data) { // loop through every row
                // if we should print the current row, then...
                if (comp(*entry, col_in_idx, op_in).operator()(r)) {
                    rows_printed++;
                    
                    if (is_quiet_mode == false) {
                        for (auto &c : print_cols) { // loop through the specified columns
                            int col_idx = colname_to_idx_map[c];
                            cout << r[col_idx] << " ";
                        }
                        cout << '\n';
                    }

                } // if
            } // for
        } // else (bst doesn't exist)
        
        delete entry; // free up dynamic memory
        entry = nullptr;
    } // else
    
    cout << "Printed " << rows_printed << " matching rows" << " from " << name << '\n';
    
    return;
} // PRINT







