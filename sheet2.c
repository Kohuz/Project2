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
    char spec_sel[100];
    char command[100];
}commands;

row_t row_ctor(int no_of_columns) {
    row_t row;
    cell_t *ptr = malloc(no_of_columns*sizeof(cell_t));
    row.no_of_columns = no_of_columns;
    row.column = ptr;
    row.cap = no_of_columns;
    return row;
}
table_t table_ctor(int no_of_rows) {
    table_t tab;
    row_t *ptr = malloc(no_of_rows*sizeof(row_t));
    tab.no_of_rows = 0;
    tab.row = ptr;
    tab.cap = 0;
    return tab;
}

void table_append(table_t *tab, int line_count, int cell_count){
    row_t row = row_ctor(cell_count);
    tab->cap += 1;
    tab->row = realloc(tab->row, tab->cap * sizeof(row_t));
    tab->row[line_count-1] = row;
    tab->no_of_rows++;
}
void print_table(table_t *tab, char delim){
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = 0; j < tab->row[i].no_of_columns; j++){
            if(j == 0)
                printf("%s", tab->row[i].column[j].content);
            else
                printf(":%s", tab->row[i].column[j].content);
        }
        putchar('\n');
    }
}
void table_resize(table_t *tab) {
    tab->row = realloc(tab->row, 2 * tab->cap * sizeof(row_t));
    tab->cap = 2 * tab->cap;
}
void table_shrink(table_t *tab) {
    tab->row = realloc(tab->row, tab->no_of_rows * sizeof(row_t));
    tab->cap = tab->no_of_rows;
}
void column_shrink(table_t *tab) {
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = 0; j < tab->row[i].no_of_columns; j++) {
            tab->row[i].column = realloc(tab->row[i].column, tab->row[i].no_of_columns * sizeof(cell_t));
            tab->row[i].cap = tab->row[i].no_of_columns;
        }
    }
}

void row_resize(table_t *tab) {
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = 0; j < tab->row[i].no_of_columns; j++) {
            tab->row[i].column = realloc(tab->row[i].column, tab->row[i].cap * 2 * sizeof(cell_t));
            tab->row[i].cap *= 2;
        }
    }
}
void table_allocate(char* argv, char* delim, table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    int ch;
    int cell_count = 1, line_count = 0;
    while((ch = fgetc(file)) != EOF) {
        if(strchr(delim,ch))
            cell_count++;
        if(ch == '\n') {
            line_count++;
            table_append(tab, line_count, cell_count);
            tab->row[line_count].no_of_columns = cell_count;
            tab->row[line_count].cap = cell_count;
            cell_count = 1;
        }

    }
    fclose(file);
}
void read_table(char* argv, char* delim,table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    if(file == NULL)
        return;
    int ch;
    int i = 0, cur_line = 0, cur_cell = 0;
    while((ch = fgetc(file)) != EOF) {
        if(ch != delim[0] && ch != '\n')
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
    for(int j = 0; j < tab->cap; j++){
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
void parse_args(char*argv, commands *comms_array, int* comm_counter) {
    const char s[2] = ";";
    char *token;

    token = strtok(argv, s);
    char arguments[5000][30];
    int j = 0;
    while (token != NULL) {
        strcpy(arguments[j], token);
        j++;
        token = strtok(NULL, s);
    }
    if (validate_arguments(arguments, j)) {
        printf("arguments not ok\n");
        return;
    }
    else{
        printf("arguments ok\n");
    }
    int help = 0, help2 = 0;
    for(int i = 0; i < j; i++) {
        if(i % 2 == 0) {
            comms_array[help].row_sel = arguments[i][1];
            comms_array[help].col_sel = arguments[i][3];
        }
        else {
            strcpy(comms_array[help].command, arguments[i]);
            help++;
        }

    }

    *comm_counter = help;

}


void irow(table_t *tab, char sel) {
    int rsel = sel - '0';
    rsel -= 1;
    if(tab->cap == tab->no_of_rows)
        table_resize(tab);
//    if(rsel > tab->cap)
//        table_resize(tab);
//    for(int i = tab->no_of_rows; i < rsel; i++) {
//            table_append(tab,tab->no_of_rows, tab->row[i].no_of_columns);
//            for(int j = 0; j < tab->row[i].no_of_columns; j++)
//                strcpy(tab->row[i].column[j].content, ":");
//    }


    for(int i = tab->no_of_rows -1; i >= rsel; i--)
        tab->row[i+1] = tab->row[i];
    tab->row[rsel] = row_ctor(tab->row[0].no_of_columns);
    for(int j = 0; j < tab->row[rsel].no_of_columns; j++)
            strcpy(tab->row[rsel].column[j].content, "");

    tab->no_of_rows++;
}
void arow(table_t *tab, char sel) {
    int rsel = sel - '0';

    if(tab->cap == tab->no_of_rows)
        table_resize(tab);
//    if(rsel > tab->cap)
//        table_resize(tab);
//    for(int i = tab->no_of_rows; i < rsel; i++) {
//            table_append(tab,tab->no_of_rows, tab->row[i].no_of_columns);
//            for(int j = 0; j < tab->row[i].no_of_columns; j++)
//                strcpy(tab->row[i].column[j].content, ":");
//    }


    for(int i = tab->no_of_rows -1; i >= rsel; i--)
        tab->row[i+1] = tab->row[i];
    tab->row[rsel] = row_ctor(tab->row[0].no_of_columns);
    for(int j = 0; j < tab->row[rsel].no_of_columns; j++)
        strcpy(tab->row[rsel].column[j].content, "");

    tab->no_of_rows++;
}
void drow(table_t *tab, char sel) { // TODO leaking memory
    int rsel = sel - '0';
    rsel -= 1;
    for(int i = rsel; i < tab->no_of_rows; i++)
        tab->row[i] = tab->row[i+1];
    tab->no_of_rows--;
    table_shrink(tab);
}
void icol(table_t *tab, char sel) {
    int rsel = sel - '0';
    rsel -= 1;
    row_resize(tab);
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = tab->row[i].no_of_columns -1; j >= rsel; j--) {
            tab->row[i].column[j + 1] = tab->row[i].column[j];
        }
        tab->row[i].no_of_columns++;
    }
    for(int i = 0; i < tab->no_of_rows; i++) {
        strcpy(tab->row[i].column[rsel].content, "");
    }
}
void acol(table_t *tab, char sel) { //TODO fix right edgecase
    int rsel = sel - '0';
    row_resize(tab);
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = tab->row[i].no_of_columns -1; j >= rsel; j--) {
            tab->row[i].column[j + 1] = tab->row[i].column[j];
        }
        tab->row[i].no_of_columns++;
    }
    for(int i = 0; i < tab->no_of_rows; i++) {
        strcpy(tab->row[i].column[rsel].content, "");
    }
}
void dcol(table_t *tab, char sel) {
    int rsel = sel - '0';
    rsel -= 1;
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = rsel; j < tab->row[i].no_of_columns; j++)
            tab->row[i].column[j] = tab->row[i].column[j + 1];
        tab->row[i].no_of_columns--;
    }
    column_shrink(tab);

}
void process_commands(table_t *tab,commands *comms_array, int comm_counter) {
    for(int i = 0; i < comm_counter; i++) {
        if(!strcmp(comms_array[i].command, "irow")) {
            irow(tab, comms_array[i].row_sel);
        }
        if(!strcmp(comms_array[i].command, "drow")) {
            drow(tab, comms_array[i].row_sel);
        }
        if(!strcmp(comms_array[i].command, "arow")) {
            arow(tab, comms_array[i].row_sel);
        }
        if(!strcmp(comms_array[i].command, "icol")) {
            icol(tab, comms_array[i].col_sel);
        }
        if(!strcmp(comms_array[i].command, "acol")) {
            acol(tab, comms_array[i].col_sel);
        }
        if(!strcmp(comms_array[i].command, "dcol")) {
            dcol(tab, comms_array[i].col_sel);
        }

    }
}

int main(int argc, char* argv[]) {
    char delim[100] = ":";
    commands comms_array[1000];
    int comm_counter = 0;
    int *ptr = &comm_counter;
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
    parse_args(argv[2],comms_array, &comm_counter);//TEMPORARY ARGV[1] = FILE, ARGV[2] = CMDSEQUENCE
    table_t tab = table_ctor(1);
    table_allocate(argv[1], delim, &tab);
    read_table(argv[1], delim, &tab);
    process_commands(&tab, comms_array, comm_counter);



    print_table(&tab, ':');
    table_dtor(&tab);






//    tab = split(argv[1],&file, line_count, cell_count, delim);
//
    return 0;
}