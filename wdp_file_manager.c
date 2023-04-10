#include <stdio.h>
#include <string.h>
#include <ncurses.h> 
#include <stdlib.h>
#include "manager.h"
#define CURRENT_WINDOW 5
#define WIN_DIM 20
#define WIN_ROWS WIN_DIM
#define WIN_COLS WIN_DIM * 1.8 

/*-------------helper functions-------------*/

/*drawing border*/
void bord(WINDOW * win){
    wclear(win);
    char c='+';
    int left, right, top, bottom;
    bottom = top = 45;
    left = right = 124;
    wborder(win, left, right, top, bottom, (int)c, (int)c, (int)c, (int)c);
    wrefresh(win);
}

/*take and check input*/
void getInput(char bufor[], int yStart, int xStart){    
    for(int i=0; i <15; i++){
        char c=getch(); 
        if((c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && c < 123) || (c=='@')){
            bufor[i] = c;
            printw("%c", bufor[i]);
        }else if (c=='\n') { //enter to accept
            if(i<3){
                curs_set(0);
                mvprintw(yStart+15,xStart+10,"Input too short!");
                mvprintw(yStart+17,xStart+5,"Press any key to exit");
                getch();
                endwin();
                exit(-1);
                break;
            }else
            bufor[i]='\0';
            break;
            
        }else if(c==27) {   //escape 
            break;
            
        }    
    }
}

/*drawing and running menu options*/
void menu(WINDOW * win,char* options[],int size){
    
    wclear(win);
    bord(win);  
    noecho();
    keypad(win,true); 

    int winX=(WIN_DIM/2)+4;
    int winY=WIN_DIM/2;
    int choice;
    int current_highlited=0;
    bool choosing=true;

    /*looping menu*/
    while(choosing){
        for(int i=0; i<size; i++){
            if(i==current_highlited){
                wattron(win, A_STANDOUT);
                mvwprintw(win, i+winY, winX, "%s", options[i]); 
                wattroff(win, A_STANDOUT);
            }else{
                mvwprintw(win, i+winY, winX, "%s", options[i]); 
            }
        }
        choice=wgetch(win);
    
    /*menu choices*/    
        switch (choice)
        {
         case KEY_UP:
            if(current_highlited == 0)     
              current_highlited = size-1;  
            else
              current_highlited--;
            break;
        case KEY_DOWN:
            if(current_highlited == size-1)
              current_highlited = 0;
            else
              current_highlited++;           
            break;

        case 10: //enter
        state=current_highlited;
            choosing=false;
            break;
        default:
          break;
        }
    }
    refresh();
}

/*get info about user function*/
void getUserInfo(WINDOW* win,char nick[],char pass[],int yStart, int xStart){
    curs_set(1);
    wclear(win);
    bord(win);
    mvwprintw(win,8,10,"Enter nick:");
    mvwprintw(win,10,10,"Enter password:");
    wrefresh(win);
    move(yStart+9,xStart+10);
    getInput(nick,yStart,xStart);  //wait for input
    move(yStart+11,xStart+10);
    getInput(pass,yStart,xStart);
    curs_set(0);
}

/*window printing functions*/
void oknoLogin(WINDOW* win){
    
    int size=2;
    char* options[2]={"LOGIN","REGISTER"};
    menu(win,options,size);
    
}

void oknoUsera(WINDOW* win){
    
    int size=3;
    char* options[3]={"CREATE RECORD","SHOW RECORDS","EXIT"};
    menu(win,options,size);    
    
}

void oknoRecordu(WINDOW* win){
    
    int size=3;
    char* options[3]={"NEXT","PREV","EXIT"};
    menu(win,options,size);


}

void oknoWpisywania(WINDOW* win){
    
    int size=2;
    char options[5]={};
    //menu(win,options,size);

}

/*--------------------------------------------*/

int main(){	
    char filename_users[]="users.txt";
    char nick[128];
    char pass[128];
    char email[64];
    char record_pass[128];
    char name_of_record[64];
    char record_chosen[64];
    bool is_entering_data;
    /*initializing ncurses*/
    initscr();
    raw();
    curs_set(0);
    noecho();
    refresh();

    /*initializing main window*/
    int xMax, yMax;
    getmaxyx(stdscr,yMax,xMax);
    int xStart=((xMax/2)-(WIN_COLS/2));
    int yStart=((yMax/2)-(WIN_ROWS/2));
    WINDOW * win = newwin(WIN_ROWS, WIN_COLS, yStart, xStart);
    keypad(win, true);
    
    /*LOGIN REGISTER*/
    if(checkIfExists(filename_users)==false){  //automatic registration on the first run
        bord(win);
        mvwprintw(win,4,14,"NO USERS!");
        mvwprintw(win,5,11,"CREATE PROFILE!");
        mvwprintw(win,12,8,"To begin press ENTER");
        wrefresh(win);
        if(getch()==10){
            getUserInfo(win,nick,pass,yStart,xStart);
            registerUser(nick,pass);
        }
    }else{
        oknoLogin(win);
        if(state==1){
            getUserInfo(win,nick,pass,yStart,xStart);
            registerUser(nick,pass);
        }else if(state==0){
            getUserInfo(win,nick,pass,yStart,xStart);
            if(login(nick,pass)==true){
                wclear(win);
                mvwprintw(win,4,14,"LOGED IN!");
                mvwprintw(win,7,6,"Press any key to continue");
                wrefresh(win);
                getch();
            }else{
                wclear(win);
                mvwprintw(win,4,14,"LOGIN INCORRECT!");
                mvwprintw(win,7,9,"Press any key to exit");
                wrefresh(win);
                getch();
                endwin();
                return -1;
            }
        }
    }
    
    // USER LOGED IN
    /*struct initialization*/
    User current_user;
    strcpy(current_user.nick,nick);
    strcpy(current_user.pass,pass);

    UserRecordArray current_user_array;
    initializeUserRecordArray(&current_user_array,current_user);
    showRecords(&current_user_array,current_user.nick);
    bool y =true;
    while(y){    
        oknoUsera(win);
        switch(state){
            case 0: //CREATE RECORD
            /*getting input*/
            if(current_user_array.last!=32){
                    bord(win);
                    mvwprintw(win,4,7,"Enter unique name: ");
                    mvwprintw(win,6,7,"Enter email: ");
                    mvwprintw(win,8,7,"Enter passsword:");
                    wrefresh(win);
                
                    move(yStart+5,xStart+7);    
                    getInput(name_of_record,yStart,xStart);  //move on after input
                    is_entering_data=true;
                    move(yStart+7,xStart+7);
                    getInput(email,yStart,xStart);
                    move(yStart+9,xStart+7);
                    getInput(record_pass,yStart,xStart);
                /*creating record */
                Record new_record;
                createRecord(&new_record,name_of_record,email,current_user.nick,record_pass);
                addRecordToArray(&current_user_array, new_record);
                exportPasswords(current_user_array);
            
            }else{
                wclear(win);
                mvwprintw(win, 7,9,"Max records");
                wrefresh(win);
                break;
            }
            break;
            case 1: //SHOW RECORD
            
            /*drawing list*/
            bord(win);
            mvwprintw(win,17,4,"enter name to expand:");
            for(int i=0; i<current_user_array.last;i++){
                mvwprintw(win,2+i,5,current_user_array.array[i].name);
                mvwprintw(win,2+i,2,"%d.",i+1);
            }
            wrefresh(win);
            move(yStart+18,xStart+4);
            getInput(record_chosen,yStart,xStart);
            
            /*getting and storing the searched record*/
            int index_of_searched_record=findRecordInArray(&current_user_array,record_chosen);
            Record searched_record = current_user_array.array[index_of_searched_record];
            strcpy(email,searched_record.email);
            strcpy(name_of_record,searched_record.name);
            strcpy(record_pass,searched_record.pass);
            /*showing the searched record*/
            bord(win);
            mvwprintw(win,4,7,"name: %s",name_of_record);
            mvwprintw(win,5,7,"email: %s",email);
            mvwprintw(win,6,7,"password: %s",record_pass);
            mvwprintw(win,8,7,"press any key to exit");
            wrefresh(win);
            getch();
            break;

            case 2: //exit
            y=false;
            break;

        }
    }
      
    /*END OF CORE EVENT LOOP*/
    /*END NCURSES MODE AND EXIT*/
    endwin();
	return 0;
}
