#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_COMMAND_LENGTH 20
#define MAX_SENTENCE_LENGTH 1024

char command[MAX_COMMAND_LENGTH];
char sentence[MAX_SENTENCE_LENGTH];

enum commands {
    CHANGE = 0, DELETE = 1, PRINT = 2, UNDO = 3, REDO = 4, QUIT = 5
};

//FILE *fptr;
//FILE *fout;
char **text;
int **history;
int *element_history;
int *current;
int *temp_array;
int index_text = 0;
int n_undo = 0;
int size_text = 0;
int element_current = 0;
int number_of_commands = 0;
int number_of_delete = 0;
int current_command=0;

int parse_command(){
    fgets(command, MAX_COMMAND_LENGTH + 1, stdin);
    if (strchr(command,'c')!=NULL){
        return CHANGE;
    }
    else if (strchr(command,'d')!=NULL){
        return DELETE;
    }
    else if (strchr(command,'p')!=NULL){
        return PRINT;
    }
    else if (strchr(command,'u')!=NULL){
        return UNDO;
    }
    else if (strchr(command,'r')!=NULL){
        return REDO;
    }
    else if (strchr(command,'q')!=NULL){
        return QUIT;
    }
    else{
        return 0;
    }
}

void do_undo() {
    int temp = number_of_commands - n_undo;
    if (temp==0){
        current_command=0;
    }
    else {
        if (temp < current_command) {
            temp_array = realloc(temp_array, (element_history[temp] + 1) * sizeof(int));
            bool flag = false;
            if (history[temp][0] == 1) {
                for (int i = 1; i <= element_history[temp]; i++) {
                    temp_array[i] = history[temp][element_history[temp - 1] + i + 2];
                }
            } else {
                for (int i = temp + 1; i <= number_of_commands; i++) {
                    if (history[i][0] == 1) {
                        current_command = i - 1;
                        i = number_of_commands + 1;
                        flag = true;
                    }
                }
                if (flag == true) {
                    for (int i = 1; i <= element_history[temp]; i++) {
                        temp_array[i] = history[current_command + 1][i + 2];
                    }
                } else {
                    for (int i = 1; i <= element_history[temp]; i++) {
                        if (i <= element_current) {
                            temp_array[i] = current[i];
                        }
                    }
                    current_command = number_of_commands;
                }
                for (int i = current_command; i > temp; i--) {   //number_of_commands non va bene se ci sono delete
                    int p = 4;
                    if (i > 0) {
                        for (int j = history[i][1]; j <= history[i][2]; j++) {
                            if (j <= element_history[temp]) {
                                temp_array[j] = history[i][p];
                                p++;
                            } else {
                                j = history[i][2] + 1;
                            }
                        }
                    }
                }
            }
            current_command = temp;
        } else if (temp > current_command) {
            temp_array = realloc(temp_array, (element_history[temp] + 1) * sizeof(int));
            bool flag = false;
            if (history[temp][0] == 1) {
                for (int i = 1; i <= element_history[temp]; i++) {
                    temp_array[i] = history[temp][element_history[temp - 1] + i + 2];
                }
            } else {
                for (int i = temp - 1; i > current_command; i--) {
                    if (history[i][0] == 1) {
                        current_command = i;
                        i = current_command + 1;
                        flag = true;
                    }
                }
                if (flag == true) {
                    for (int i = 1; i <= element_history[temp]; i++) {
                        temp_array[i] = history[current_command][element_history[current_command - 1] + 2 + i];
                    }
                }
                for (int i = current_command + 1; i <= temp; i++) {
                    int p = 0;
                    if (i > 0) {
                        for (int j = history[i][1]; j <= history[i][2]; j++) {
                            if (j <= element_history[temp]) {
                                temp_array[j] = history[i][3] + p;
                                p++;
                            } else {
                                j = history[i][2] + 1;
                            }
                        }
                    }
                }
            }
            current_command = temp;
        }
    }
}

void temp_into_current(){
    current = realloc(current, (element_history[current_command] + 1) * sizeof(int));
    for (int i = 1; i <= element_history[current_command]; i++) {
        current[i] = temp_array[i];
    }
    for (int i = current_command + 1; i <= number_of_commands; i++) {
        free(history[i]);
    }
    number_of_commands = current_command;
    element_current = element_history[current_command];
}

void handle_CHANGE(int x, int y){
    if (n_undo > 0){
        do_undo();
        temp_into_current();
    }
    number_of_commands++;
    current_command++;
    int space;
    space = y - x + 1;
    history = realloc(history, (number_of_commands+1) * sizeof(int *));
    history[number_of_commands] = malloc((space + space + 4)*sizeof(int ));
    history[number_of_commands][0] = 0;
    history[number_of_commands][1] = x;
    history[number_of_commands][2] = y;
    history[number_of_commands][3] = index_text;
    int j=4;
    if(x != 0 && y!=0){
    	for(int i=x;i<=y;i++){
        	fgets(sentence, MAX_SENTENCE_LENGTH + 1, stdin);
        	int len=strlen(sentence);
        	size_text++;
       		text = realloc(text, (size_text+1) * sizeof(char *));
        	text[index_text] = malloc((len+1)*sizeof(char ));
        	strcpy(text[index_text],sentence);
        	if(i <= element_current && i>0){
            	    element_current--;
              	    history[number_of_commands][j] = current[i];
        	}
        	else{
            	    history[number_of_commands][j] = 0;
        	}
		element_current++;
		current = realloc(current, (element_current+1) * sizeof(int ));
        	current[i] = index_text;
        	index_text++;
        	j++;
    	}
    	fgets(sentence, 10, stdin);
    element_history = realloc(element_history, (number_of_commands+1) * sizeof(int ));
    element_history[number_of_commands] = element_current;
    }
    n_undo = 0;
}

void handle_DELETE(int x, int y){
    if (n_undo > 0){
        do_undo();
        temp_into_current();
    }
    number_of_commands++;
    current_command++;
    number_of_delete++;
    history = realloc(history, (number_of_commands+1) * sizeof(int *));
    history[number_of_commands] = malloc((element_current+element_current+3)*sizeof(int ));
    history[number_of_commands][0] = 1;
    history[number_of_commands][1] = x;
    history[number_of_commands][2] = y;
    int temp =element_current;
    for(int e=1; e<= element_current; e++){
        history[number_of_commands][e+2] = current[e];
    }
    int j=y+1;
    int del=0;
    if(y>0) {
        if(x==0){
            x=1;
        }
        for (int i = x; i <= element_current; i++) {
                del++;
                if(j <= element_current) {
                    current[i] = current[j];
                    j++;
                    del--;
                }
        }
        if(del>0) {
            element_current = element_current - del;
            current = realloc(current, (element_current+1) * sizeof(int ));
        }
    }
    element_history = realloc(element_history, (number_of_commands+1) * sizeof(int ));
    element_history[number_of_commands] = element_current;
    for(int e=1; e<=element_current; e++){
        history[number_of_commands][temp+2+e] = current[e];
    }
    n_undo = 0;
}

void handle_PRINT(int x, int y){
    int temp = number_of_commands - n_undo;
    if (n_undo > 0){
        do_undo();
        for(int i=x;i<=y;i++){
            if (i > element_history[temp] || i == 0 || temp==0) {
                fputs(".\n", stdout);
            } else {
                fputs(text[temp_array[i]], stdout);
            }
        }
    }else {
        for (int i = x; i <= y; i++) {
            if (i > element_current || i == 0) {
                fputs(".\n", stdout);
            } else {
                fputs(text[current[i]], stdout);
            }
        }
    }
}

void handle_UNDO(int x){
    if(x >= number_of_commands){
        n_undo = number_of_commands;
    }
    else {
        n_undo = n_undo + x;
        if(n_undo >= number_of_commands){
            n_undo = number_of_commands;
        }
    }
}

void handle_REDO(int x){
    if(x >= n_undo){
        n_undo = 0;
        current_command=number_of_commands;
    }
    else{
        n_undo = n_undo - x;
    }
}

void handle_QUIT(){
    free(text);
}

bool handle_command(){
    int input_command = parse_command();
    int t=0;
    int x;
    int y;

    switch (input_command) {
        case 0: {
            char *tok;
            char *token = strtok(command, ",");
            while (token != 0) {
                if (t == 0) {
                    x = atoi(token);
                } else if (t == 1) {
                    tok = strtok(token, "c");
                    y = atoi(tok);
                }
                t++;
                token = strtok(NULL, ",");
            }
            handle_CHANGE(x,y);
            return false;
        }
        case 1: {
            char *tok;
            char *token = strtok(command, ",");
            while (token != 0) {
                if (t == 0) {
                    x = atoi(token);
                } else if (t == 1) {
                    tok = strtok(token, "d");
                    y = atoi(tok);
                }
                t++;
                token = strtok(NULL, ",");
            }
            handle_DELETE(x,y);
            return false;
        }
        case 2: {
            char *tok;
            char *token = strtok(command, ",");
            while (token != 0) {
                if (t == 0) {
                    x = atoi(token);
                } else if (t == 1) {
                    tok = strtok(token, "p");
                    y = atoi(tok);
                }
                t++;
                token = strtok(NULL, ",");
            }
            handle_PRINT(x,y);
            return false;
        }
        case 3: {
            char *tok;
            tok = strtok(command, "u");
            x = atoi(tok);
            handle_UNDO(x);
            return false;
        }
        case 4: {
            char *tok;
            tok = strtok(command, "r");
            x = atoi(tok);
            handle_REDO(x);
            return false;
        }
        case 5:
            handle_QUIT();
            return true;
        default:
            return true;

    }
}

int main() {

//    fptr = fopen("test10000.txt","r");
//    fout = fopen("solpr.txt","w");
    bool stop = true;
    current = malloc(sizeof(int ));
    temp_array = malloc(sizeof(int ));
    element_history = malloc(sizeof(int ));
    element_history[0] = 0;
    history = malloc(sizeof(int *));
    text = malloc(sizeof(char *));
    size_text++;;
    do{
        stop=handle_command();
    }while(!stop);

//    fclose(fptr);
//    fclose(fout);

    return 0;
}


