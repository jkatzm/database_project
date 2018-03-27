//
//  main.cpp
//  Project 3
//
//  Created by Jordan Katz on 3/11/18.
//  Copyright © 2018 Jordan Katz. All rights reserved.
//

#include <iostream> // cin, cout

#include "TableEntry.h"
#include "Table.h"

using namespace std;



int main(int argc, char** argv) {
    std::ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;
    cout << std::boolalpha;

    bool quiet_mode = get_options(argc, argv);
    
    unordered_map<string, Table*> database;
    
    // read in from file
    string command, rubbish;
    while (cin >> command) {
        cout << "% ";
        string tablename_in;
        
        switch (command[0]) {
            // Create
            case 'C': {
                int num_cols_in;
                cin >> tablename_in >> num_cols_in;
                
                if (database.find(tablename_in) != database.end()) {
                    print_error1(tablename_in);
                    getline(cin, rubbish);
                    break;
                }
                
                Table* to_create = new Table(tablename_in, num_cols_in);
                to_create->CREATE();
                database[tablename_in] = to_create;
                break;
            }
            
            // Remove
            case 'R': {
                cin >> tablename_in;
                
                if (database.find(tablename_in) == database.end()) {
                    print_error2(tablename_in);
                    break;
                }
                
                Table* to_remove = database[tablename_in];
                delete to_remove;
                to_remove = nullptr;
                database.erase(tablename_in);
                cout << "Table " << tablename_in << " deleted" << '\n';
                break;
            }
                
            // INSERT_INTO
            case 'I': {
                int num_added_rows;
                cin >> rubbish >> tablename_in >> num_added_rows >> rubbish;
                
                if (database.find(tablename_in) == database.end()) {
                    print_error2(tablename_in);
                    break;
                }
                
                database[tablename_in]->INSERT_INTO(num_added_rows);
                break;
            }
            
            // DELETE_FROM
            case 'D': {
                string colname_in, value_in;
                char op_in; // one of '=', '<', '>'
                cin >> rubbish >> tablename_in >> rubbish >> colname_in >> op_in >> value_in;
                
                if (database.find(tablename_in) == database.end()) {
                    print_error2(tablename_in);
                    break;
                }
                
                Table* current = database[tablename_in];
                
                if (current->colname_to_idx_map.find(colname_in) == current->colname_to_idx_map.end()) {
                    print_error3(tablename_in, colname_in);
                    break;
                }
                
                int col_idx = current->colname_to_idx_map[colname_in];
                
                current->DELETE_FROM(col_idx, op_in, value_in);
                
                break;
            }
            
            // GENERATE_INDEX
            case 'G': {
                string hash_or_bst, colname_in;
                cin >> rubbish >> tablename_in >> hash_or_bst >> rubbish >> rubbish >> colname_in;
                
                if (database.find(tablename_in) == database.end()) {
                    print_error2(tablename_in);
                    break;
                }
                
                Table* current = database[tablename_in];
                
                if (current->colname_to_idx_map.find(colname_in) == current->colname_to_idx_map.end()) {
                    print_error3(tablename_in, colname_in);
                    break;
                }
                
                int col_idx = current->colname_to_idx_map[colname_in];
                
                current->GENERATE_INDEX(hash_or_bst[0], col_idx);
                
                cout << "Created " << hash_or_bst << " index for table "
                    << tablename_in << " on column " << colname_in << '\n';
                
                break;
            }
                
            // PRINT
            case 'P': {
                bool should_return = false;
                int num_cols_to_print;
                cin >> rubbish >> tablename_in >> num_cols_to_print;
                
                if (database.find(tablename_in) == database.end()) {
                    print_error2(tablename_in);
                    getline(cin, rubbish);
                    break;
                }
                
                Table* current = database[tablename_in];
                vector<string> print_cols_in;
                print_cols_in.reserve(num_cols_to_print);
            
                // read in columns to print, making sure each is valid
                // as we read in, push back the column names into "print_cols_in"
                for (int i = 0; i < num_cols_to_print; ++i) {
                    string ith_col_name;
                    cin >> ith_col_name;
                    
                    if (current->colname_to_idx_map.find(ith_col_name) == current->colname_to_idx_map.end()) {
                        print_error3(tablename_in, ith_col_name);
                        getline(cin, rubbish);
                        should_return = true;
                        break;
                    }
                    
                    print_cols_in.push_back(ith_col_name);
                } // for
                
                if (should_return == true) {
                    break;
                }
                
                current->PRINT(print_cols_in, quiet_mode);
                
                break;
            }
                
            // Join
            case 'J': {
                string tablename1_in, tablename2_in, colname1_in, colname2_in;
                int num_cols_to_print, rows_joined = 0;
                
                cin >> tablename1_in >> rubbish >> tablename2_in >> rubbish
                    >> colname1_in >> rubbish >> colname2_in >> rubbish
                    >> rubbish >> num_cols_to_print;

                if (database.find(tablename1_in) == database.end()) {
                    print_error2(tablename1_in);
                    getline(cin, rubbish);
                    break;
                }
                
                if (database.find(tablename2_in) == database.end()) {
                    print_error2(tablename2_in);
                    getline(cin, rubbish);
                    break;
                }
                
                Table* table1 = database[tablename1_in];
                Table* table2 = database[tablename2_in];
                
                if (table1->colname_to_idx_map.find(colname1_in) == table1->colname_to_idx_map.end()) {
                    print_error3(tablename1_in, colname1_in);
                    getline(cin, rubbish);
                    break;
                }
                
                if (table2->colname_to_idx_map.find(colname2_in) == table2->colname_to_idx_map.end()) {
                    print_error3(tablename2_in, colname2_in);
                    getline(cin, rubbish);
                    break;
                }
                
                int col1_idx = table1->colname_to_idx_map[colname1_in];
                int col2_idx = table2->colname_to_idx_map[colname2_in];
                
                vector<pair<string, int>> join_colnames;
                join_colnames.reserve(num_cols_to_print);
                
                // read in the specified column names, checking to make sure they exist
                bool should_return = false;
                for (int i = 0; i < num_cols_to_print; ++i) {
                    string ith_col_name;
                    int one_or_two;
                    cin >> ith_col_name >> one_or_two;
                    
                    switch (one_or_two) {
                        case 1: {
                            if (table1->colname_to_idx_map.find(ith_col_name) == table1->colname_to_idx_map.end()) {
                                print_error3(tablename1_in, ith_col_name);
                                getline(cin, rubbish);
                                should_return = true;
                                break;
                            }
                            
                            pair<string, int> to_insert(ith_col_name, 1);
                            join_colnames.push_back(to_insert);
                            
                            break;
                        }
                        case 2: {
                            if (table2->colname_to_idx_map.find(ith_col_name) == table2->colname_to_idx_map.end()) {
                                print_error3(tablename2_in, ith_col_name);
                                getline(cin, rubbish);
                                should_return = true;
                                break;
                            }

                            pair<string, int> to_insert(ith_col_name, 2);
                            join_colnames.push_back(to_insert);
                            
                            break;
                        }
                    } // switch
                    
                    if (should_return == true) {
                        break; // break out of the for-loop
                    }
                } // for
                
                if (should_return == true) {
                    break; // break out of the "Join" case
                }
                
                // done reading in and checking input
                
                if (quiet_mode == false) { // print column names
                    for (auto &col_pair : join_colnames) { // for each of the specified columns
                        cout << col_pair.first << " ";
                    }
                    cout << '\n';
                }
                
                
                
                
                
                
                
                
                
                
                // 0. create a hash table for table2 that maps from values in col2_idx to row indices (in table2)
                unordered_map<TableEntry, vector<int>> temp_map;
                for (int r_idx = 0; r_idx < int(table2->data.size()); ++r_idx) {
                    TableEntry* key_val = &table2->data[r_idx][col2_idx];
                    temp_map[*key_val].push_back(r_idx);
                }
                
                // 1. iterate through table1's rows
                for (auto &t1_row : table1->data) {
                    
                    // 2. for each row's respective col1_idx value in table1, find matching col2_idx values if they exist
                    TableEntry* matching_value = &t1_row[col1_idx];
                    if (temp_map.find(*matching_value) != temp_map.end()) {
                        
                        // 3. for each match found, print the column values in the matching rows
                        for (auto t2_row_idx : temp_map[*matching_value]) {
                            rows_joined++;
                            
                            if (quiet_mode == false) {
                                for (auto &col_pair : join_colnames) { // for each of the specified column-table pairs
                                    // determine whether to look at col "1" or "2"...
                                    switch (col_pair.second) {
                                        case 1: {
                                            int spec_idx = table1->colname_to_idx_map[col_pair.first];
                                            cout << t1_row[spec_idx] << " ";
                                            break;
                                        }
                                        case 2: {
                                            int spec_idx = table2->colname_to_idx_map[col_pair.first];
                                            cout << table2->data[t2_row_idx][spec_idx] << " ";
                                            break;
                                        }
                                    } // switch
                                } // for (pairs in join_cols)
                                cout << '\n';
                            }
                        }
                    }
                    
                    /*
                    for (auto &table2_row : table2->data) { // for each key-value pair in temp_map
//                    for (auto &table2_row : table2->data) { // for each row in table2
                        
                        // if the values in each column match...
                        if (table1_row[col1_idx] == table2_row[col2_idx]) {
                            rows_joined++;
                            
                            if (quiet_mode == false) { // print out the values
                                for (auto &col_pair : join_colnames) { // for each of the specified columns
                                    // determine whether to look at col "1" or "2"...
                                    switch (col_pair.second) {
                                        case 1: {
                                            int col_idx = table1->colname_to_idx_map[col_pair.first];
                                            cout << table1_row[col_idx] << " ";
                                            break;
                                        }
                                        case 2: {
                                            int col_idx = table2->colname_to_idx_map[col_pair.first];
                                            cout << table2_row[col_idx] << " ";
                                            break;
                                        }
                                    } // switch
                                } // for (pairs in join_cols)
                                cout << '\n';
                            }

                        } // if
                    } // for (table2 rows)
                     */
                    
                    
                } // for (table1 rows)
                

                
                
                cout << "Printed " << rows_joined << " rows from joining "
                    << tablename1_in << " to " << tablename2_in << '\n';

                break;
            }
                
            // Quit
            case 'Q': {
                for (auto &it : database) {
                    delete it.second;
                    it.second = nullptr;
                }
                database.clear();
                
                cout << "Thanks for being silly!" << endl;
                return 0;
            }
            
            // Comment
            case '#': {
                getline(cin, rubbish);
                break;
            }
            
            default: {
                cout << "Error: unrecognized command\n";
                getline(cin, rubbish);
                break;
            }
        } // switch
    } // while
    
    return 0;
}
