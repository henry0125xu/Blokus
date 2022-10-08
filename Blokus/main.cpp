#include<stdio.h>
#include<stdlib.h>
#include<graphics.h>
#include<conio.h>
#include<pthread.h>
#include<semaphore.h>
#include<windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
/*-------data structure--------*/
char turn,winner;
int _round;
bool out_A,out_B;
enum State{
    START,CHOOSE_SIDE,TO_CHOOSE,CHOOSE_BLOCK,TO_PUT,GAMEOVER,RETURN
};
enum Color{
    null,red,blue
};
State state;

Color board_DS[14][14];
bool A_block_pool[21];
bool B_block_pool[21];

Color A_color;
Color B_color;

/*-------initial function----------*/
void init_game();
void clear_and_put_logo();
void choose_side();
//related to board_DS
void show_board();
void* parallel_draw_board(void*);
void* parallel_draw_block(void*);
void to_choose();
void to_put(int);
void start_btn();
void turn_btn();
void put_hint();
bool dead();
//related to block_pool
int choose_block();
void show_A_pool();
void show_B_pool();
Color current_color();
void game_over();
/*-----------others-----------*/
void click_sound();
void error_sound();
bool intersection(long long*,long long*);
bool in_area(int,int,int,int*);
long long determinant(long long*,long long*);

//point for mouse
POINT mouse;
//window id handler
HWND hw;
//semaphore
sem_t mutex;
#include "blokush.h"
int main(){
    printf("%d",initwindow);
    //initial window
    initwindow(1000,1000,"Blokus");
    //get id of window
    hw=FindWindow(NULL,"Blokus");
    //thread for catching keydown
    pthread_t gktt;
    pthread_create(&gktt,NULL,get_key_to_terminate,NULL);
    sem_init(&mutex,0,1);
    //start
    while(1){
        init_game();
        init_block();
        start_btn();
        choose_side();
        to_choose();
        game_over();
    }
    //
    pthread_join(gktt,NULL);
    return 0;
}
void init_game(){
    state=START;
    turn=NULL;
    winner=NULL;
    _round=0;
    out_A=false;
    out_B=false;
    A_color=null;
    B_color=null;
    clear_and_put_logo();
    for(int i=0;i<14;i++){
        for(int j=0;j<14;j++){
            board_DS[i][j]=null;
        }
    }
    for(int i=0;i<21;i++){
        A_block_pool[i]=true;
        B_block_pool[i]=true;
    }

}
void start_btn(){
    state=START;
    setcolor(7);
    rectangle(312,394,688,530);
    setfillstyle(0,7);
    floodfill(320,415,7);
    setbkcolor(BLACK);
    settextstyle(3,0,10);
    outtextxy(318,400,"START!");
    setcolor(BLACK);
    setlinestyle(2,0,3);
    rectangle(310,392,690,532);

    while(1){
        GetCursorPos(&mouse);
        ScreenToClient(hw,&mouse);
        int x=mouse.x,y=mouse.y;
        if(GetAsyncKeyState(MK_LBUTTON)){
            if(x>310&&x<690&&y>392&&y<532){
                click_sound();
                Sleep(150);
                setcolor(WHITE);
                settextstyle(3,0,10);
                outtextxy(318,400,"START!");
                Sleep(350);
                break;
            }
        }
    }
}
void choose_side(){
    clear_and_put_logo();
    state=CHOOSE_SIDE;
    setcolor(WHITE);
    setbkcolor(BLACK);
    settextstyle(8,0,5);
    outtextxy(160,200,"Choose first gamer's color:");
    setlinestyle(0,0,1);

    setcolor(RED);
    rectangle(200,450,450,700);
    setfillstyle(1,RED);
    floodfill(215,465,RED);

    setcolor(BLUE);
    rectangle(550,450,800,700);
    setfillstyle(1,BLUE);
    floodfill(575,475,BLUE);

    while(1){
        GetCursorPos(&mouse);
        ScreenToClient(hw,&mouse);
        int x=mouse.x,y=mouse.y;
        if(GetAsyncKeyState(MK_LBUTTON)){
            if(x>200&&x<450&&y>450&&y<700){
                A_color=red;
                B_color=blue;
                click_sound();
                break;
            }
            else if(x>550&&x<800&&y>450&&y<700){
                A_color=blue;
                B_color=red;
                click_sound();
                break;
            }
        }
    }
}
void show_board(){
    if(state!=TO_PUT){
        clear_and_put_logo();
        //light yellow
        setlinestyle(0,0,0);
        setcolor(YELLOW);
        pthread_t draw_board[25];
        int data1[25],data2[14];
        for(int i=0;i<25;i++){
            data1[i]=i*28+150;
            pthread_create(&draw_board[i],NULL,parallel_draw_board,(void*)(data1+i));
        }
        for(int i=0;i<25;i++){
            pthread_join(draw_board[i],NULL);
        }
        setlinestyle(SOLID_LINE,0,4);
        setcolor(BLACK);

        for(int i=150;i<=850;i+=50){
            line(150,i,850,i);
            line(i,150,i,850);
        }

        if(_round<3){
            readimagefile("flag.jpg",353,353,397,397);
            readimagefile("flag.jpg",603,603,647,647);
        }

    }

    pthread_t draw_block[14];
    int data[14];
    for(int i=0;i<14;i++){
        data[i]=i;
        pthread_create(&draw_block[i],NULL,parallel_draw_block,(void*)(data+i));
    }
    for(int i=0;i<14;i++){
        pthread_join(draw_block[i],NULL);
    }

}
void* parallel_draw_board(void* i){
    for(int j=*((int*)i);j<*((int*)i)+28;j+=4){
        line(150,j,850,j);
        line(j,150,j,850);
    }
}
void* parallel_draw_block(void* i){
    for(int j=0;j<14;j++){
            switch(board_DS[*((int*)i)][j]){
            case red:
                setcolor(7);
                setlinestyle(0,0,6);
                rectangle(*((int*)i)*50+150,j*50+150,*((int*)i)*50+200,j*50+200);
                //entry section
                sem_wait(&mutex);
                setfillstyle(1,RED);
                floodfill(*((int*)i)*50+170,j*50+170,7);
                sem_post(&mutex);
                //exit section
                break;
            case blue:
                setcolor(7);
                setlinestyle(0,0,6);
                rectangle(*((int*)i)*50+150,j*50+150,*((int*)i)*50+200,j*50+200);
                //entry section
                sem_wait(&mutex);
                setfillstyle(1,BLUE);
                floodfill(*((int*)i)*50+170,j*50+170,7);
                sem_post(&mutex);
                //exit section
                break;
            }
    }
}
void to_choose(){

    //show:
    int choice;
    A:
        choice=-1;
        turn='A';
        show_board();
        readimagefile("blank.jpg",50,50,150,120);
        state=TO_CHOOSE;
        turn_btn();
        if(dead()){
            if(out_B==true) return;
            out_A=true;
            goto B;
        }
        while(1){
            GetCursorPos(&mouse);
            ScreenToClient(hw,&mouse);
            if(GetAsyncKeyState(MK_LBUTTON)){
                if(mouse.x>258&&mouse.x<737&&mouse.y>911&&mouse.y<942){
                    click_sound();
                A_AGAIN:
                    choice=choose_block();

                    if(state==RETURN){
                        show_board();
                        state=TO_CHOOSE;
                        turn_btn();
                    }
                    else if(state==CHOOSE_BLOCK){
                        show_board();
                        readimagefile("back.jpg",50,50,150,120);
                        state=TO_PUT;
                        put_hint();

                        to_put(choice);
                        if(state==RETURN){
                            state=TO_CHOOSE;
                            goto A_AGAIN;
                        }
                        break;
                    }
                }
            }
        }
        A_block_pool[choice]=false;

    B:
        choice=-1;
        turn='B';
        show_board();
        readimagefile("blank.jpg",50,50,150,120);
        state=TO_CHOOSE;
        turn_btn();
        if(dead()){
            if(out_A==true) return;
            out_B=true;
            goto A;
        }
        while(1){
            GetCursorPos(&mouse);
            ScreenToClient(hw,&mouse);
            if(GetAsyncKeyState(MK_LBUTTON)){
                if(mouse.x>258&&mouse.x<737&&mouse.y>911&&mouse.y<942){
                    click_sound();
                B_AGAIN:
                    choice=choose_block();

                    if(state==RETURN){
                        show_board();
                        state=TO_CHOOSE;
                        turn_btn();
                    }
                    else if(state==CHOOSE_BLOCK){
                        show_board();
                        readimagefile("back.jpg",50,50,150,120);
                        state=TO_PUT;
                        put_hint();

                        to_put(choice);
                        if(state==RETURN){
                            state=TO_CHOOSE;
                            goto B_AGAIN;
                        }
                        break;
                    }
                }
            }
        }
        B_block_pool[choice]=false;
        goto A;

}
void to_put(int choice){
    int grid[2];

    get_grid_on_board(grid,choice);
    if(state==RETURN)
        return;
    _round++;
}

void turn_btn(){
    readimagefile("blank.jpg",280,890,800,980);
    setcolor(BLACK);
    setlinestyle(0,0,3);
    if(turn=='A')
        readimagefile("A_Turn.jpg",258,911,737,942);
    else if(turn=='B') readimagefile("B_Turn.jpg",258,911,737,942);
    rectangle(258,911,737,942);
}

bool dead(){
    bool flag=false;
    if(_round>20){
        switch(turn){
        case 'A':
            for(int i=0;i<14;i++){
                for(int j=0;j<14;j++){
                    int grid[2];
                    grid[0]=i;
                    grid[1]=j;
                    for(int k=0;k<21;k++){
                        if(A_block_pool[k]&&!board_DS[i][j]){
                            if(legality(k,grid))
                                return false;
                        }
                    }
                }
            }
            break;
        case 'B':
            for(int i=0;i<14;i++){
                for(int j=0;j<14;j++){
                    int grid[2];
                    grid[0]=i;
                    grid[1]=j;
                    for(int k=0;k<21;k++){
                        if(B_block_pool[k]&&!board_DS[i][j]){
                            if(legality(k,grid))
                                return false;
                        }
                    }
                }
            }
            break;
        }
        return true;
    }
    return false;
}
void put_hint(){
    setcolor(0);
    setbkcolor(15);
    settextstyle(8,0,3);
    outtextxy(285,893,"Put your block on the board,");
    outtextxy(285,915,"cursor position points to the ");
    outtextxy(285,937,"upper left corner of the block.");
}
int choose_block(){
    state=CHOOSE_BLOCK;
    clear_and_put_logo();
    readimagefile("back.jpg",50,50,150,120);
    if(turn=='A'){
        show_A_pool();
        while(1){
            GetCursorPos(&mouse);
            ScreenToClient(hw,&mouse);
            if(mouse.y>=140&&mouse.y<265&&mouse.y!=200){
                for(int i=0;i<5;i++){
                  if(A_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(A_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(A_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=265&&mouse.y<395&&mouse.y!=330){
                for(int i=5;i<9;i++){
                  if(A_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(A_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(A_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=395&&mouse.y<575&&mouse.y!=460){
                for(int i=9;i<13;i++){
                  if(A_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(A_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(A_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=575&&mouse.y<755&&mouse.y!=640&&mouse.y!=690){
                for(int i=13;i<17;i++){
                  if(A_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(A_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(A_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=755&&mouse.y<960&&mouse.y!=820){
                for(int i=17;i<21;i++){
                  if(A_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(A_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(A_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            //back
            if(GetAsyncKeyState(MK_LBUTTON)){
                if(mouse.x>55&&mouse.x<145&&mouse.y>55&&mouse.y<115){
                    state=RETURN;
                    click_sound();
                    return -1;
                }
            }

        }
    }
    else if(turn=='B'){
        show_B_pool();
        while(1){
            GetCursorPos(&mouse);
            ScreenToClient(hw,&mouse);
            if(mouse.y>=140&&mouse.y<265&&mouse.y!=200){
                for(int i=0;i<5;i++){
                  if(B_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(B_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(B_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=265&&mouse.y<395&&mouse.y!=330){
                for(int i=5;i<9;i++){
                  if(B_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(B_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(B_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }

            }
            else if(mouse.y>=395&&mouse.y<575&&mouse.y!=460){
                for(int i=9;i<13;i++){
                  if(B_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(B_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(B_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=575&&mouse.y<755&&mouse.y!=640&&mouse.y!=690){
                for(int i=13;i<17;i++){
                  if(B_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(B_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(B_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            else if(mouse.y>=755&&mouse.y<960&&mouse.y!=820){
                for(int i=17;i<21;i++){
                  if(B_block_pool[i]){
                    if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                        setfillstyle(1,YELLOW);
                        floodfill(mouse.x,mouse.y,LIGHTGRAY);
                        if(GetAsyncKeyState(MK_LBUTTON)){
                            if(in_area(mouse.x,mouse.y,block[i].edge,block[i].shape)){
                                if(B_block_pool[i]){
                                    click_sound();
                                    return i;
                                }
                            }
                        }
                    }
                    else{
                        if(B_color==red){
                            setfillstyle(1,RED);
                        }
                        else{
                            setfillstyle(1,BLUE);
                        }
                        floodfill(block[i].shape[0]+20,block[i].shape[1]+20,7);
                    }
                  }
                }
            }
            //back
            if(GetAsyncKeyState(MK_LBUTTON)){
                if(mouse.x>55&&mouse.x<145&&mouse.y>55&&mouse.y<115){
                    state=RETURN;
                    click_sound();
                    return -1;
                }
            }
        }
    }
}
void show_A_pool(){
    switch(A_color){
    case red:
        for(int i=0;i<21;i++){
            if(A_block_pool[i]){
                setcolor(RED);
                drawpoly(block[i].edge,block[i].shape);
                setfillstyle(1,RED);
                floodfill(block[i].shape[0]+10,block[i].shape[1]+10,RED);
                setcolor(LIGHTGRAY);
                setlinestyle(1,0,7);
                drawpoly(block[i].edge,block[i].shape);
            }
        }

        break;
    case blue:
        for(int i=0;i<21;i++){
            if(A_block_pool[i]){
                setcolor(BLUE);
                drawpoly(block[i].edge,block[i].shape);
                setfillstyle(1,BLUE);
                floodfill(block[i].shape[0]+10,block[i].shape[1]+10,BLUE);
                setcolor(LIGHTGRAY);
                setlinestyle(1,0,7);
                drawpoly(block[i].edge,block[i].shape);
            }
        }
        break;
    }

}
void show_B_pool(){
    switch(B_color){
    case red:
        for(int i=0;i<21;i++){
            if(B_block_pool[i]){
                setcolor(RED);
                drawpoly(block[i].edge,block[i].shape);
                setfillstyle(1,RED);
                floodfill(block[i].shape[0]+10,block[i].shape[1]+10,RED);
                setcolor(LIGHTGRAY);
                setlinestyle(1,0,7);
                drawpoly(block[i].edge,block[i].shape);
            }
        }
        break;
    case blue:
        for(int i=0;i<21;i++){
            if(B_block_pool[i]){
                setcolor(BLUE);
                drawpoly(block[i].edge,block[i].shape);
                setfillstyle(1,BLUE);
                floodfill(block[i].shape[0]+10,block[i].shape[1]+10,BLUE);
                setcolor(LIGHTGRAY);
                setlinestyle(1,0,7);
                drawpoly(block[i].edge,block[i].shape);
            }
        }
        break;
    }
}
void game_over(){
    state=GAMEOVER;
    int red_cnt=0,blue_cnt=0;
    for(int i=0;i<14;i++){
        for(int j=0;j<14;j++){
            if(board_DS[i][j]==red) red_cnt++;
            else if(board_DS[i][j]==blue) blue_cnt++;
        }
    }
    if(A_color==red){
        if(red_cnt>blue_cnt) winner='A';
        else if(red_cnt<blue_cnt)winner='B';
        else winner='T';
    }
    else{
        if(red_cnt>blue_cnt) winner='B';
        else if(red_cnt<blue_cnt)winner='A';
        else winner='T';
    }
    if(winner=='A'){
        readimagefile("A_go.jpg",230,353,770,453);
    }
    else if(winner=='B'){
        readimagefile("B_go.jpg",230,353,770,453);
    }
    else readimagefile("tie_go.jpg",230,353,770,453);
    while(1){
        GetCursorPos(&mouse);
        ScreenToClient(hw,&mouse);
        if(GetAsyncKeyState(MK_LBUTTON)){
            if(mouse.x>230&&mouse.x<770&&mouse.y>353&&mouse.y<453){
                click_sound();
                return;
            }
        }
    }
}
void clear_and_put_logo(){
    cleardevice();
    setfillstyle(1,WHITE);
    floodfill(0,0,WHITE);
    readimagefile("logo.jpg",205,35,795,118);
}
void click_sound(){
    PlaySoundA(TEXT("click.wav"),NULL,SND_ASYNC|SND_NODEFAULT);
    Sleep(100);
}
void error_sound(){
    PlaySoundA(TEXT("error.wav"),NULL,SND_ASYNC|SND_NODEFAULT);
    Sleep(100);
}
bool in_area(int x,int y,int poly,int* area){
    long long ray[4],edge[4];
    int inter_count=0;
    ray[0]=x;
    ray[1]=y;
    ray[2]=1000;
    ray[3]=y;
    for(int i=0;i<(poly-1)*2;i+=2){
        edge[0]=(long long)area[0+i];
        edge[1]=(long long)area[1+i];
        edge[2]=(long long)area[2+i];
        edge[3]=(long long)area[3+i];
        if(intersection(ray,edge)) {
            inter_count++;
        }
    }
    if(inter_count%2!=0) return true;
    else return false;
}
bool intersection(long long* line1,long long* line2){
    long long v1[2],v2[2],v3[3];
    bool straddles1,straddles2;
    v1[0]=line2[0]-line1[0];
    v1[1]=line2[1]-line1[1];
    v2[0]=line1[2]-line1[0];
    v2[1]=line1[3]-line1[1];
    v3[0]=line2[2]-line1[0];
    v3[1]=line2[3]-line1[1];

    if(determinant(v2,v1)*determinant(v2,v3)<0)
        straddles1=true;
    else straddles1=false;
    v1[0]=line1[0]-line2[0];
    v1[1]=line1[1]-line2[1];
    v2[0]=line2[2]-line2[0];
    v2[1]=line2[3]-line2[1];
    v3[0]=line1[2]-line2[0];
    v3[1]=line1[3]-line2[1];

    if(determinant(v2,v1)*determinant(v2,v3)<0)
        straddles2=true;
    else straddles2=false;

    if(straddles1==true&&straddles2==true) return true;
    else return false;
}
long long determinant(long long* v1,long long* v2){
    return v1[0]*v2[1]-v1[1]*v2[0];
}
Color current_color(){
    if(turn=='A') return A_color;
    else if(turn=='B') return B_color;
}
