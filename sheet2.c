#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct  {
    int no_of_cell;
    char content[101];
}cell_t;


typedef struct {
    int no_of_columns;
    int cap;
    cell_t* column ;
}row_t;


typedef struct {
    int no_of_rows;
    row_t* row ;
    int cap;
}table_t;

typedef struct {
    char col_sel;
    char row_sel;
}rowcol_sel;

row_t row_ctor(int no_of_columns) {
    row_t row;
    cell_t *ptr = malloc(no_of_columns*sizeof(cell_t));
    row.no_of_columns = no_of_columns;
    row.column = ptr;
    return row;
}
table_t table_ctor(int no_of_rows) {
    table_t tab;
    row_t *ptr = malloc(no_of_rows*sizeof(row_t));
    tab.no_of_rows = no_of_rows;
    tab.row = ptr;
    tab.cap = 1;
    return tab;
}

void table_append(table_t *tab, int line_count, int cell_count){
    row_t row = row_ctor(cell_count);
    tab->cap += 1;
    tab->row = realloc(tab->row, tab->cap * sizeof(row_t));
    tab->row[line_count-1] = row;
    tab->no_of_rows++;
}
void table_allocate(char* argv, char* delim, table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    int ch;
    int cell_count = 0, line_count = 0;
    while((ch = fgetc(file)) != EOF) {
        if(strchr(delim,ch))
            cell_count++;
        if(ch == '\n') {
            line_count++;
            table_append(tab, line_count, cell_count);
            tab->row[line_count].no_of_columns = cell_count;
            cell_count = 1;
        }

    }
    fclose(file);
}
void read_table(char* argv, char* delim,table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    int ch;
    int i = 0, cur_line = 0, cur_cell = 0;
    while((ch = fgetc(file)) != EOF) {
        tab->row[cur_line].column[cur_cell].content[i] = ch;
        i++;
        if(strchr(delim,ch)) {
            i = 0;
            cur_cell++;
        }
        if(ch == '\n') {
            cur_line++;
            i = 0;
            cur_cell = 0;
        }

    }
    fclose(file);
}
void table_dtor(table_t *tab) {
    for(int j = 0; j < tab->no_of_rows; j++){
        free(tab->row[j].column);
    }
    free(tab->row);

}
int is_find(char* arg){
    char* beg_find = "[find";
    int find_len = 5;
    for(int i = 0; i < find_len; i++) {
        if(arg[i] != beg_find[i])
            return 1;
    }
    if(arg[strlen(arg)] != ']')
        return 1;

    return 0;
}
int check_sel(char* arg){
    printf("This is a selection: %s\n", arg);
    int len = strlen(arg);
    if(!strcmp("[max]", arg)) {
        return 0;
    }
    else if (!strcmp("[min]", arg)) {
        return 0;
    }
    //TODO FIX THIS
    else if(is_find(arg)) {
        return 0;
    }
    else {
        if (arg[0] != '[')
            return 1;
        if (arg[1] != '_' && (arg[1] < '0' || arg[1] > '9'))
            return 1;
        if (arg[2] != ',')
            return 1;
        if (arg[3] != '_' && (arg[3] < '0' || arg[3] > '9'))
            return 1;
        if (arg[4] != ']')
            return 1;
    }
    return 0;
}
int is_struct_command(char* arg) {
    if(!strcmp("irow", arg))
        return 0;
    else if(!strcmp("arow", arg))
        return 0;
    else if(!strcmp("drow", arg))
        return 0;
    else if(!strcmp("acol", arg))
        return 0;
    else if(!strcmp("icol", arg))
        return 0;
    else if(!strcmp("dcol", arg))
        return 0;
    return 1;
}
int check_commands(char* arg) {
    printf("This is a command: %s\n", arg);
    if(is_struct_command(arg))
        return 1;
    return 0;
}
int validate_arguments(char arguments[5000][30], int len){
    for(int i = 0; i < len; i++){
        if(i%2 == 0) {
            if(check_sel(arguments[i]))
                return 1; // validation failed
        }
        else
            if(check_commands(arguments[i]))
                return 1; // validation failed
    }
    return 0;
}
void parse_args(char*argv) {

    const char s[2] = ";";
    char *token;

    token = strtok(argv, s);
    char arguments[5000][30];
    int j = 0;
    while( token != NULL ) {
        strcpy(arguments[j], token);
        j++;
        token = strtok(NULL, s);
    }
    if(validate_arguments(arguments, j))
        printf("arguments not ok\n");
    else
        printf("arguments ok\n");
}

int main(int argc, char* argv[]) {
    char delim[100] = " ";
    int arg_idx = 2;
//    if (strcmp("-d", argv[1]) == 0) {
//        if(argv[2]) {
//            strcpy(delim, argv[2]);
//            arg_idx = 3;
//        }
//        else {
//            fprintf(stderr, "Missing delimeter\n");
//            return 1;
//
//        }
//    }
    if(!argv[2]) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }
    parse_args(argv[2]);//TEMPORARY ARGV[1] = FILE, ARGV[2] = CMDSEQUENCE
    table_t tab = table_ctor(1);
    table_allocate(argv[1], delim, &tab);
    read_table(argv[1], delim, &tab);

    printf("%s\n", tab.row[0].column[0].content);
    printf("%s\n", tab.row[0].column[1].content);
    printf("%s\n", tab.row[1].column[0].content);
    printf("%s\n", tab.row[2].column[0].content);
    printf("%s\n", tab.row[2].column[2].content);

//    for(int j = 0; j < tab.no_of_rows; j++){
//        free(tab.row[j].column);
//    }
//    free(tab.row);

    table_dtor(&tab);






//    tab = split(argv[1],&file, line_count, cell_count, delim);
//
    return 0;
}