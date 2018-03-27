//
//  Table.h
//  Project 3
//
//  Created by Jordan Katz on 3/11/18.
//  Copyright © 2018 Jordan Katz. All rights reserved.
//

#ifndef Table_h
#define Table_h

#include <vector>
#include <unordered_map>
#include <map>

#include "TableEntry.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////
// "Table" CLASS
///////////////////////////////////////////////////////////////////////////
class Table {
public:
    // MEMBER FUNCTIONS
    //////////////////////////////////////////////////////////////////
    Table(const string &name_in, const int &num_cols_in) :
        name(name_in), num_cols(num_cols_in), indexed_col(-1), bst_exists(false), hash_exists(false) {}
    
    // INPUT:
    // FUNCTION:
    // OUTPUT:
    void CREATE();
    
    // INPUT:
    // FUNCTION:
    // OUTPUT:
    void INSERT_INTO(const int num_rows_to_add);
    
    // INPUT: a column index, a comparison type [=,<,>], and a value
    // FUNCTION: removes rows from "data" where the value at the specified column idx
    //           matches the input value; updates any indexing
    // OUTPUT: the number of rows removed
    void DELETE_FROM(const int col_idx, const char op, const string value);
    
    // INPUT: a column index, a map type [h=hash, b=bst],
    // FUNCTION: creates a map (either hash table or a binary search tree) from the
    //           column at the specified idx to
    void GENERATE_INDEX(const char h_or_b, const int col_idx);
    
    // INPUT:
    // FUNCTION:
    // OUTPUT:
    void PRINT(const vector<string> &print_cols, const bool is_quiet_mode);
    
    
    // MEMBER VARIABLES
    //////////////////////////////////////////////////////////////////
    vector<vector<TableEntry>> data; // a vector of rows, where each row is a vector<TableEntry>
    unordered_map<string, int> colname_to_idx_map; // allows for quick lookup of a column's index given its name
    
    vector<char> col_types; // the types [i=int, b=bool, d=double, s=string] stored in each column
    const string name; // the table name
    const int num_cols; // the number of columns in the table
    
    // KEYS: the different values in column "indexed_col"
    // VALUES: the (indices of) rows in "data" which possess KEY in spot "indexed_col"
    unordered_map<TableEntry, vector<int>> hash;
    map<TableEntry, vector<int>> bst;
    
    int indexed_col; // the value i of the ith column, indexed in the above maps
    bool bst_exists; // whether or not a bst map exists, or if it has been cleared
    bool hash_exists; // whether or not a hash map exists, or if it has been cleared
}; // Table


///////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////////////
TableEntry* create_temp_entry(char type_in, const string val_in);
bool get_options(int argc, char** argv);
void print_error1(const string &tablename_in);
void print_error2(const string &tablename_in);
void print_error3(const string &tablename_in, const string &colname_in);


///////////////////////////////////////////////////////////////////////////
// COMPARATOR
///////////////////////////////////////////////////////////////////////////
struct comp {
    comp(const TableEntry &item_in, const int col_idx_in, const char comp_type_in) :
        item(item_in), col_idx(col_idx_in), comp_type(comp_type_in) {}

    bool operator() (const vector<TableEntry> &row_in) const {
        if (comp_type == '=') { return row_in[col_idx] == item; }
        else if (comp_type == '<') { return row_in[col_idx] < item; }
        else { return row_in[col_idx] > item; } // comp_type == '>'
    }
private:
    const TableEntry item;
    const int col_idx;
    const char comp_type;
}; // comp



#endif /* Table_h */
