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

int main(int argc, char* argv[]) {
    char delim[100] = " ";
//    int arg_idx = 1;
//    if(!strcmp(argv[1], "-d")) {
//        if(argv[2])
//        {
//            strcpy(delim, argv[2]);
//            arg_idx = 3;
//        }
//        else {
//            fprintf(stderr, "Missing delimeter");
//        }
//    }
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
