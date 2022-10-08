#ifndef _BLOKUSH_H
#define _BLOKUSH_H

void init_block();
void get_grid_on_board(int*,int);
bool legality(int,int*);
void block_project_on_DS(int,int,int,Color,Color[][14]);
/*-------------------------------------------*/
//thread
void* get_key_to_terminate(void*){

    if(getch()){
        closegraph();
        pthread_exit(NULL);
    }

}
//block standard
struct Block{
    int edge;
    int* shape;
};
Block block[21];

int shape0[]={100,150,150,150,150,200,100,200,100,150},
    shape1[]={200,150,300,150,300,200,200,200,200,150},
    shape2[]={350,150,450,150,450,250,400,250,400,200,350,200,350,150},
    shape3[]={500,150,650,150,650,200,500,200,500,150},
    shape4[]={700,150,800,150,800,250,700,250,700,150},
    shape5[]={100,280,250,280,250,330,150,330,150,380,100,380,100,280},
    shape6[]={300,280,450,280,450,330,400,330,400,380,350,380,350,330,300,330,300,280},
    shape7[]={500,280,600,280,600,330,650,330,650,380,550,380,550,330,500,330,500,280},
    shape8[]={700,330,900,330,900,380,700,380,700,330},
    shape9[]={100,410,250,410,250,460,200,460,200,510,100,510,100,410},
    shape10[]={300,410,450,410,450,510,400,510,400,460,350,460,350,510,300,510,300,410},
    shape11[]={500,410,650,410,650,460,600,460,600,560,550,560,550,460,500,460,500,410},
    shape12[]={700,410,850,410,850,460,750,460,750,560,700,560,700,410},
    shape13[]={100,590,200,590,200,640,250,640,250,740,200,740,200,690,150,690,150,640,100,640,100,590},
    shape14[]={300,590,350,590,350,640,450,640,450,740,400,740,400,690,300,690,300,590},
    shape15[]={500,590,550,590,550,640,650,640,650,690,600,690,600,740,550,740,550,690,500,690,500,590},
    shape16[]={750,590,800,590,800,640,850,640,850,690,800,690,800,740,750,740,750,690,700,690,700,640,750,640,750,590},
    shape17[]={100,770,300,770,300,820,150,820,150,870,100,870,100,770},
    shape18[]={350,770,500,770,500,820,550,820,550,870,450,870,450,820,350,820,350,770},
    shape19[]={650,770,850,770,850,820,750,820,750,870,700,870,700,820,650,820,650,770},
    shape20[]={350,900,600,900,600,950,350,950,350,900};

int grid_num[]={1,2,3,3,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5};

void init_block(){
    block[0].edge=5;
    block[0].shape=shape0;

    block[1].edge=5;
    block[1].shape=shape1;

    block[2].edge=7;
    block[2].shape=shape2;

    block[3].edge=5;
    block[3].shape=shape3;

    block[4].edge=5;
    block[4].shape=shape4;

    block[5].edge=7;
    block[5].shape=shape5;

    block[6].edge=9;
    block[6].shape=shape6;

    block[7].edge=9;
    block[7].shape=shape7;

    block[8].edge=5;
    block[8].shape=shape8;

    block[9].edge=7;
    block[9].shape=shape9;

    block[10].edge=9;
    block[10].shape=shape10;

    block[11].edge=9;
    block[11].shape=shape11;

    block[12].edge=7;
    block[12].shape=shape12;

    block[13].edge=11;
    block[13].shape=shape13;

    block[14].edge=9;
    block[14].shape=shape14;

    block[15].edge=11;
    block[15].shape=shape15;

    block[16].edge=13;
    block[16].shape=shape16;

    block[17].edge=7;
    block[17].shape=shape17;

    block[18].edge=9;
    block[18].shape=shape18;

    block[19].edge=9;
    block[19].shape=shape19;

    block[20].edge=5;
    block[20].shape=shape20;
}
bool legality(int choice,int* grid){
    int grid_cnt=0;
    Color temp[14][14];
    //initial
    for(int i=0;i<14;i++){
        for(int j=0;j<14;j++) temp[i][j]=null;
    }
    if(_round<2){
        //first round
        block_project_on_DS(grid[0],grid[1],choice,current_color(),temp);
        if(temp[4][4]&&board_DS[4][4]==null||temp[9][9]&&board_DS[9][9]==null){

            block_project_on_DS(grid[0],grid[1],choice,current_color(),board_DS);

            return true;
        }
        else return false;
    }
    else{
        //generate condition
        block_project_on_DS(grid[0],grid[1],choice,current_color(),temp);

        for(int i=0;i<14;i++){
            for(int j=0;j<14;j++){
                //condetion with overlay
                if(board_DS[i][j]!=null&&temp[i][j]!=null)return false;
                //condition with same color
                if(i>0&&i<13&&j>0&&j<13){
                    if(board_DS[i][j]){
                       if(temp[i+1][j]==board_DS[i][j]||
                          temp[i-1][j]==board_DS[i][j]||
                          temp[i][j+1]==board_DS[i][j]||
                          temp[i][j-1]==board_DS[i][j])
                          return false;
                    }
                }
                if(i==0){
                    if(board_DS[i][j]){
                       if(temp[i+1][j]==board_DS[i][j]||
                          temp[i][j+1]==board_DS[i][j]||
                          temp[i][j-1]==board_DS[i][j])
                          return false;
                    }
                }
                else if(i==13){
                    if(board_DS[i][j]){
                       if(temp[i-1][j]==board_DS[i][j]||
                          temp[i][j+1]==board_DS[i][j]||
                          temp[i][j-1]==board_DS[i][j])
                          return false;
                    }
                }
                if(j==0){
                    if(board_DS[i][j]){
                       if(temp[i+1][j]==board_DS[i][j]||
                          temp[i-1][j]==board_DS[i][j]||
                          temp[i][j+1]==board_DS[i][j])
                          return false;
                    }
                }
                else if(j==13){
                    if(board_DS[i][j]){
                       if(temp[i+1][j]==board_DS[i][j]||
                          temp[i-1][j]==board_DS[i][j]||
                          temp[i][j-1]==board_DS[i][j])
                          return false;
                    }
                }
                if(temp[i][j]>0) grid_cnt++;
            }
        }
        //condition with out of board
        if(grid_cnt==0) return false;
        if(state==TO_CHOOSE) return true;

        block_project_on_DS(grid[0],grid[1],choice,current_color(),board_DS);

        return true;
    }
}

void get_grid_on_board(int *grid,int choice){
    while(1){
        GetCursorPos(&mouse);
        ScreenToClient(hw,&mouse);
        for(int i=150;i<=850;i+=50){
            for(int j=150;j<=850;j+=50){
                if(mouse.x>=i&&mouse.x<i+50&&mouse.y>=j&&mouse.y<j+50){
                    grid[0]=(i-150)/50;
                    grid[1]=(j-150)/50;
                    //get grid number under mouse position
                }
            }
        }
        if(GetAsyncKeyState(MK_LBUTTON)){
            if(mouse.x>55&&mouse.x<145&&mouse.y>55&&mouse.y<115){
                state=RETURN;
                click_sound();
                return;
            }
            if(legality(choice,grid)){
                click_sound();
                break;
            }
            else{
                if(mouse.x>150&&mouse.x<850&&mouse.y>150&&mouse.y<850)
                    error_sound();
            }
        }
    }
}
void block_project_on_DS(int x,int y,int shape,Color color,Color board[][14]){
    switch(shape){
    case 0:
        board[x][y]=color;
        break;
    case 1:
        if(x<13){
            board[x][y]=color;
            board[x+1][y]=color;
        }
        break;
    case 2:
        if(x<13&&y<13){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x+1][y+1]=color;
        }
        break;
    case 3:
        if(x<12){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x+2][y]=color;
        }
        break;
    case 4:
        if(x<13&&y<13){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x][y+1]=color;
            board[x+1][y+1]=color;
        }
        break;
    case 5:
        if(x<12&&y<13){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x+2][y]=color;
            board[x][y+1]=color;
        }
        break;
    case 6:
        if(x<12&&y<13){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x+2][y]=color;
            board[x+1][y+1]=color;
        }
        break;
    case 7:
        if(x<12&&y<13){
            board[x][y]=color;
            board[x+1][y]=color;
            board[x+1][y+1]=color;
            board[x+2][y+1]=color;
        }
        break;
    case 8:
        if(x<11){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+3][y]=color;
        }
        break;
    case 9:
        if(x<12&&y<13){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x][y+1]=color;
           board[x+1][y+1]=color;
        }
        break;
    case 10:
        if(x<12&&y<13){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x][y+1]=color;
           board[x+2][y+1]=color;
        }
        break;
    case 11:
        if(x<12&&y<12){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+1][y+2]=color;
           board[x+1][y+1]=color;
        }
        break;
    case 12:
        if(x<12&&y<12){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x][y+1]=color;
           board[x][y+2]=color;
        }
        break;
    case 13:
        if(x<12&&y<12){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y+1]=color;
           board[x+2][y+2]=color;
           board[x+1][y+1]=color;
        }
        break;
    case 14:
        if(x<12&&y<12){
           board[x][y]=color;
           board[x][y+1]=color;
           board[x+1][y+1]=color;
           board[x+2][y+1]=color;
           board[x+2][y+2]=color;
        }
        break;
    case 15:
        if(x<12&&y<12){
           board[x][y]=color;
           board[x][y+1]=color;
           board[x+1][y+1]=color;
           board[x+2][y+1]=color;
           board[x+1][y+2]=color;
        }
        break;
    case 16:
        if(x>0&&x<13&&y<12){
           board[x][y]=color;
           board[x][y+1]=color;
           board[x-1][y+1]=color;
           board[x+1][y+1]=color;
           board[x][y+2]=color;
        }
        break;
    case 17:
        if(x<11&&y<13){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+3][y]=color;
           board[x][y+1]=color;
        }
        break;
    case 18:
        if(x<11&&y<13){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+2][y+1]=color;
           board[x+3][y+1]=color;
        }
        break;
    case 19:
        if(x<11&y<13){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+3][y]=color;
           board[x+1][y+1]=color;
        }
        break;
    case 20:
        if(x<10){
           board[x][y]=color;
           board[x+1][y]=color;
           board[x+2][y]=color;
           board[x+3][y]=color;
           board[x+4][y]=color;
        }
        break;
    }
}

#endif // _BLOKUSH_H
