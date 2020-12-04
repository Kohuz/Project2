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
    if(ptr == NULL){
        exit(EXIT_FAILURE);
    }
    row.no_of_columns = no_of_columns;
    row.column = ptr;
    row.cap = no_of_columns;
    return row;
}
table_t table_ctor(int no_of_rows) {
    table_t tab;
    row_t *ptr = malloc(no_of_rows*sizeof(row_t));
    if(ptr == NULL) {

        exit(EXIT_FAILURE);
    }
    tab.no_of_rows = 0;
    tab.row = ptr;
    tab.cap = 1;
    return tab;
}

void table_append(table_t *tab, int line_count, int cell_count){
    tab->no_of_rows++;
    row_t row = row_ctor(cell_count);
    tab->cap += 1;
    row_t* help;
    help = realloc(tab->row, tab->cap * sizeof(row_t));
    if(help == NULL) {
        free(tab->row);
        exit(EXIT_FAILURE);
    }
    tab->row = help;
    tab->row[line_count-1] = row;
}
void print_table(table_t *tab, char delim, char* argv){
    FILE* file;
    file = fopen(argv, "w");
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = 0; j < tab->row[i].no_of_columns; j++){
            if(j == 0)
                fprintf(file,"%s", tab->row[i].column[j].content);
            else
                fprintf(file,":%s", tab->row[i].column[j].content);
        }
        fprintf(file, "\n");
    }
}

void table_resize(table_t *tab, int by) {
    void* help;
    help = realloc(tab->row, by * tab->cap * sizeof(row_t));
    if(help == NULL){
        free(tab->row);
        exit(EXIT_FAILURE);
    }
    tab->row = help;
    tab->cap = by * tab->cap;
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

void row_resize(table_t *tab, int by) {
    void* help;
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = 0; j < tab->row[i].no_of_columns; j++) {
            help = realloc(tab->row[i].column, tab->row[i].cap * by * sizeof(cell_t));
            if(help == NULL){
                //TODO
                exit(EXIT_FAILURE);
            }
            tab->row[i].column = help;
            tab->row[i].cap *= by;
        }
    }
}
int table_allocate(char* argv, char* delim, table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    if(file == NULL){
        return 1;
    }
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
    return 0;
}
int read_table(char* argv, char* delim,table_t *tab) {
    FILE* file;
    file = fopen(argv, "r");
    if(file == NULL)
        return 1;
    int ch;
    int i = 0, cur_line = 0, cur_cell = 0;
    while((ch = fgetc(file)) != EOF) {
        if(ch != delim[0] && ch != '\n' && ch != '\\')
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

int check_sel(char* arg){
    printf("This is a selection: %s\n", arg);
    if(!strcmp("[max]", arg)) {
        return 0;
    }
    else if (!strcmp("[min]", arg)) {
        return 0;
    }
//        //TODO FIX THIS
//    else if(is_find(arg)) {
//        return 0;
//    }
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


void split_sel(char* rowsel, char* colsel, char* sel) {
    char* s = ",";
    char* token;
    token = strtok(sel, s);
    int j = 0;
    while (token != NULL){
        int len = strlen(token);
        if (j == 0){
        for(int i = 1, k = 0; i < len; i++, k++){
            rowsel[k] = token [i];
            }
        }
        if(j == 1){
            for(int i = 0; i < (len-1); i++){
                colsel[i] = token [i];
            }
        }
        j++;
        token = strtok(NULL, s);
    }
}

void irow(table_t *tab, char* sel) {
//    int rsel;
//    char rowsel[30];
//    char colsel[30];
//    for(int i = 0; i < 30; i++){
//        rowsel[i] = '\0';
//        colsel[i] = '\0';
//    }
//    split_sel(rowsel, colsel, sel);
//
//    rsel = atoi(rowsel);
//    rsel -= 1;
    int rsel = atoi(sel);
    rsel -= 1;
    if(rsel > tab->no_of_rows) {
        table_resize(tab, rsel);
        for(int i = tab->no_of_rows; i < rsel; i++){
            tab->row[i] = row_ctor(tab->row[0].no_of_columns);
            {
                for (int j = 0; j < tab->row[i].no_of_columns; j++)
                    strcpy(tab->row[i].column[j].content, "");
            }
        }
        tab->no_of_rows = rsel;
    }
    if(tab->cap == tab->no_of_rows)
        table_resize(tab,2);

    for(int i = tab->no_of_rows -1; i >= rsel; i--)
        tab->row[i+1] = tab->row[i];
    tab->row[rsel] = row_ctor(tab->row[0].no_of_columns);
    for(int j = 0; j < tab->row[rsel].no_of_columns; j++)
        strcpy(tab->row[rsel].column[j].content, "");

    tab->no_of_rows++;
}
void arow(table_t *tab, char* sel) {
//    int rsel;
//    char rowsel[30];
//    char colsel[30];
//    for(int i = 0; i < 30; i++){
//        rowsel[i] = '\0';
//        colsel[i] = '\0';
//    }
//    split_sel(rowsel, colsel, sel);
    int rsel = atoi(sel);

    if(rsel > tab->no_of_rows) {
        table_resize(tab, rsel);
        for(int i = tab->no_of_rows; i < rsel; i++){
            tab->row[i] = row_ctor(tab->row[0].no_of_columns);
            {
                for (int j = 0; j < tab->row[i].no_of_columns; j++)
                    strcpy(tab->row[i].column[j].content, "");
            }
        }
        tab->no_of_rows = rsel;
    }
    if(tab->cap == tab->no_of_rows)
        table_resize(tab,2);

    for(int i = tab->no_of_rows -1; i >= rsel; i--)
        tab->row[i+1] = tab->row[i];
    tab->row[rsel] = row_ctor(tab->row[0].no_of_columns);
    for(int j = 0; j < tab->row[rsel].no_of_columns; j++)
        strcpy(tab->row[rsel].column[j].content, "");

    tab->no_of_rows++;
}
void drow(table_t *tab, char* sel) { // TODO leaking memory
//    int rsel;
//    char rowsel[30];
//    char colsel[30];
//    for(int i = 0; i < 30; i++){
//        rowsel[i] = '\0';
//        colsel[i] = '\0';
//    }
//    split_sel(rowsel, colsel, sel);

    int rsel = atoi(sel);
    rsel -= 1;
    for(int i = rsel; i < tab->no_of_rows; i++)
        tab->row[i] = tab->row[i+1];
    tab->no_of_rows--;
    table_shrink(tab);
}
void icol(table_t *tab, char* sel) {
//    int rsel;
//    char rowsel[30];
//    char colsel[30];
//    for(int i = 0; i < 30; i++){
//        rowsel[i] = '\0';
//        colsel[i] = '\0';
//    }
//    split_sel(rowsel, colsel, sel);

    int csel = atoi(sel);
    csel -= 1;
    if(csel > tab->no_of_rows) {
        row_resize(tab, csel);
        for(int i = 0; i < tab->no_of_rows; i++){
                for (int j = tab->row[i].no_of_columns; j < csel; j++)
                    strcpy(tab->row[i].column[j].content, "");
            tab->row[i].no_of_columns = csel;
        }
    }
    row_resize(tab,2);
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = tab->row[i].no_of_columns -1; j >= csel; j--) {
            tab->row[i].column[j + 1] = tab->row[i].column[j];
        }
        tab->row[i].no_of_columns++;
    }
    for(int i = 0; i < tab->no_of_rows; i++) {
        strcpy(tab->row[i].column[csel].content, "");
    }
}
void acol(table_t *tab, char* sel) { //TODO fix right edgecase
    int csel = atoi(sel);
    if(csel > tab->no_of_rows) {
        row_resize(tab, csel);
        for(int i = 0; i < tab->no_of_rows; i++){
            for (int j = tab->row[i].no_of_columns; j < csel; j++)
                strcpy(tab->row[i].column[j].content, "");
            tab->row[i].no_of_columns = csel;
        }
    }
    row_resize(tab, 2);
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = tab->row[i].no_of_columns -1; j >= csel; j--) {
            tab->row[i].column[j + 1] = tab->row[i].column[j];
        }
        tab->row[i].no_of_columns++;
    }
    for(int i = 0; i < tab->no_of_rows; i++) {
        strcpy(tab->row[i].column[csel].content, "");
    }
}
void dcol(table_t *tab, char* sel) {
    int rsel = atoi(sel);
    rsel -= 1;
    for(int i = 0; i < tab->no_of_rows; i++) {
        for(int j = rsel; j < tab->row[i].no_of_columns; j++)
            tab->row[i].column[j] = tab->row[i].column[j + 1];
        tab->row[i].no_of_columns--;
    }
    column_shrink(tab);

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
int is_set(char* arg){
    if(strlen(arg) < 5)
        return 0; //not valid
    char* beg_set = "set ";
    int find_len = 4;
    for(int i = 0; i < find_len; i++) {
        if(arg[i] != beg_set[i])
            return 0;
    }
    return 1;
}
int is_swap(char* arg){
    if(strlen(arg) < 10)
        return 0; //not valid
    char* beg_set = "swap ";
    if(arg[5] != '[')
        return 0;
    if((arg[6] < '0' || arg[6] > '9'))
        return 0;
    if(arg[7] != ',')
        return 0;
    if((arg[8] < '0' || arg[6] > '9'))
        return 0;
    if(arg[9] != ']')
        return 0;
    int find_len = 4;
    for(int i = 0; i < find_len; i++) {
        if(arg[i] != beg_set[i])
            return 0;
    }
    return 1;
}
int is_sum(char* arg){
    if(strlen(arg) < 8)
        return 0; //not valid
    char* beg_set = "sum ";
    if(arg[4] != '[')
        return 0;
    if((arg[5] < '0' || arg[6] > '9'))
        return 0;
    if(arg[6] != ',')
        return 0;
    if((arg[7] < '0' || arg[6] > '9'))
        return 0;
    if(arg[8] != ']')
        return 0;
    int find_len = 4;
    for(int i = 0; i < find_len; i++) {
        if(arg[i] != beg_set[i])
            return 0;
    }
    return 1;
}

void set (table_t *tab, char* sel1, char* sel2, char* arg) {
    int rsel = atoi(sel1);
    rsel -= 1;
    int csel = atoi(sel2);
    csel -= 1;
    int len = strlen(tab->row[rsel].column[csel].content);
    for(int i = 0; i < len; i++)
        tab->row[rsel].column[csel].content[i] = '\0';
    for(int i = 4, j = 0; arg[j] != '\0'; i++, j++)
        tab->row[rsel].column[csel].content[j] = arg[i];
//    strcpy(tab->row[rsel].column[csel].content, cell);

}
void clear(table_t *tab, char* sel1, char *sel2) {
    int rsel = atoi(sel1);
    int csel = atoi(sel2);
    rsel -= 1;
    csel -= 1;
    int len = strlen(tab->row[rsel].column[csel].content);
    for(int i = 0; i < len; i++)
        tab->row[rsel].column[csel].content[i] = '\0';
}
float is_num(char*arg){
    char *ptr;
    float number;
    number = strtof(arg, &ptr);
    if(strlen(ptr) == 0)
        return number;
    else
        return -1;
}
void sum(table_t *tab, char* sel1, char* sel2, char* arg) {
    int rsel1 = atoi(sel1);
    int csel1= atoi(sel2);
    rsel1 -= 1;
    csel1 -= 1;
    char rsel2, csel2;
    rsel2 = arg[5], csel2 = arg[7];
    rsel2 -= '0';
    rsel2 -= 1;
    csel2 -= '0';
    csel2 -= 1;
    float cell1 = is_num(tab->row[rsel1].column[csel1].content);
    float cell2 = is_num(tab->row[rsel2].column[csel2].content);
    if(cell1 == -1 || cell2 == -1)
        return;
    float result = cell1 + cell2;
    char cell[101];
    sprintf(cell, "%g",result);
    int len = strlen(tab->row[rsel1].column[csel1].content);
    for(int i = 0; i < len; i++)
        tab->row[rsel1].column[csel1].content[i] = '\0';
    strcpy(tab->row[rsel1].column[csel1].content, cell);

}
void swap(table_t *tab, char* sel1, char* sel2, char* arg) {
    int rsel1 = atoi(sel1);
    int csel1= atoi(sel2);//TODO fix swap not working for dvouciferné číslo
    int ridx1 = (int)rsel1, cidx1 = (int)csel1;
    ridx1 -= 1;
    cidx1 -= 1;
    char rsel2 = arg[6], csel2 = arg[8];
    rsel2 -= '0';
    csel2 -= '0';
    int ridx2 = (int)rsel2, cidx2 = (int)csel2;
    ridx2 -= 1;
    cidx2 -= 1;
    char tmp_cell[101];
    for(int i = 0; i < 101; i++)
        tmp_cell[i] = '\0';
    strcpy(tmp_cell,tab->row[ridx1].column[cidx1].content);
    strcpy(tab->row[ridx1].column[cidx1].content, tab->row[ridx2].column[ridx2].content);
    strcpy(tab->row[ridx2].column[cidx2].content, tmp_cell);
}
void process_commands(table_t *tab, char* argv) {

    char *token;
    char *s = ";";
    char current_selection[30];
    char* rest = argv;
    char current_row_selection[30];
    char current_col_selection[30];
    while ((token = strtok_r(rest, s, &rest))) {
//        if(!strcmp("[max]", token)) {
//            strcpy(current_row_selection, token);
//        }
//        else if (!strcmp("[min]", token)) {
//            strcpy(current_row_selection, token);
//        }
        //TODO FIX THIS
//        else if(is_find(token)) {
//            strcpy(current_selection, token);
//        }




        if(token[0] == '[') {
            split_sel(current_row_selection, current_col_selection, token);
        }
//            if (token[1] == '_' || (token[1] >= '0' && token[1] <= '9'))
//                current_selection[0] = token[1];
//            if (token[2] != ',')
//                return;
//            if (token[3] == '_' || (token[3] >= '0' && token[3] <= '9'))
//                current_selection[1] = token[3];
//            if (token[4] != ']')
//                return;

        //TODO FIX CHAR VS INT
        if (!strcmp(token, "irow")) {
            irow(tab, current_row_selection);
        }
        if(!strcmp(token, "drow")) {
            drow(tab, current_row_selection);
        }
        else if(!strcmp(token, "arow")) {
            arow(tab, current_row_selection);//TODO _ DOESNT WORK
        }
        else if(!strcmp(token, "icol")) {
            icol(tab, current_col_selection);
        }
        else if(!strcmp(token, "acol")) {//TODO col functions dont make table bigger
            acol(tab, current_col_selection);
        }
        else if(!strcmp(token, "dcol")) {
            dcol(tab, current_col_selection);
        }
        else if(is_set(token)) {
            set(tab,current_row_selection, current_col_selection, token);
        }
        else if(!strcmp(token, "clear")) {
            clear(tab,current_row_selection, current_col_selection);
        }
        else if(is_swap(token)) {
            swap(tab,current_row_selection, current_col_selection, token);
        }
        else if(is_sum(token)) {
            sum(tab,current_row_selection, current_col_selection, token);
        }

    }
}

int main(int argc, char* argv[]) {
    char delim[100] = " ";
    int arg_idx = 2;
    if (strcmp("-d", argv[1]) == 0) {
        if(argv[2]) {
            strcpy(delim, argv[2]);
            arg_idx = 4;
        }
        else {
            fprintf(stderr, "Missing delimeter\n");
            return 1;
        }
    }
    if(argc < 3)
        return 0;
    if(!argv[2]) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }
//    parse_args(argv[2],comms_array, &comm_counter);//TEMPORARY ARGV[1] = FILE, ARGV[2] = CMDSEQUENCE
    table_t tab = table_ctor(1);
    if(table_allocate(argv[arg_idx], delim, &tab)) {
        fprintf(stderr, "Couldn't find file");
        return 1;
    }
    read_table(argv[arg_idx], delim, &tab);
    process_commands(&tab, argv[arg_idx-1]);
    print_table(&tab, delim[0], argv[arg_idx]);
    table_dtor(&tab);

    return 0;
}
