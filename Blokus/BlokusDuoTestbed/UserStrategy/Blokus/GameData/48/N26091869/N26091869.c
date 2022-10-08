#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
typedef enum _rotation Rotation;
typedef enum _reflection Reflection;
typedef struct _opp_log{
    int x;
    int y;
}Opp_Log;
enum _rotation{
    t_ori,rotate90,rotate180,rotate270
};
enum _reflection{
    f_ori,ref
};
char color;
char opp_color;
bool pass;
char left_pattern[21];
char board_state[14][14];
char step_log[50];
int game_round;
bool is_first_round();
bool legality(int,int,int,Rotation,Reflection,bool);
bool put_block(int,int,int,Rotation,Reflection,char[][14]);
bool dead(int);
void opp_coordinate(int*,int*,Opp_Log*);
int main(int argc,char* argv[]){
    srand(time(NULL));
    int i=0,j=0,k=0,l=0,m=0;
    int choice;
    int x,y;
    Opp_Log opp_log[6];
    bool flag;
    bool putted=false;
    game_round=0;
    pass=false;
    Rotation rotation;
    Reflection reflection;
    FILE *BoardStateF,*LeftPatternF,*LogF;
    color=argv[1][0];
    opp_color=(color=='R')?'B':'R';
    BoardStateF=fopen(argv[2],"r");
    LeftPatternF=fopen(argv[3],"r");
    LogF=fopen(argv[4],"r");
    for(i=0;i<6;i++){
        opp_log[i].x=0;
        opp_log[i].y=0;
    }
    //write board state to board_state[][]
    for(i=0;i<14;i++){
        for(j=0;j<14;j++){
            char c;
            c=fgetc(BoardStateF);
            while(c!='N' && c!='R' && c!='B'){
                c=fgetc(BoardStateF);
            }
            board_state[j][i]=c;
        }
    }
    //write last line of left pattern to left_pattern[]
    char temp[50];
    for(i=0;i<50;i++) temp[i]='\0';
    while(!feof(LeftPatternF)){
        fscanf(LeftPatternF,"%s",temp);
    }
    j=0;
    for(i=0;i<21;i++){
        while(temp[j]!='0' && temp[j]!='1') j++;
        left_pattern[i]=temp[j];
        j++;
    }
    //write last line of step log to step_log[]
    i=0;k=0;
    for(j=0;j<50;j++) step_log[j]='\0';
    while(!feof(LogF)){
        char c=fgetc(LogF);
        if(c==opp_color && step_log[i-1]==' ') flag=true;
        if(flag && c=='-'){
            opp_log[k].x=(step_log[i-1])-48;
            opp_log[k].y=fgetc(LogF)-48;
            k++;
            flag=false;
        }
        if(c == '\n' && !feof(LogF)){
            i=0;
            for(j=0;j<50;j++) step_log[j]='\0';
        }
        else{
            step_log[i] = c;
            i++;
        }
    }
    //read game round
    if((int)(step_log[0]-48)<10 && step_log[1]==' '){
        game_round=step_log[0]-47;
    }
    else{
        int ten=(step_log[0]-48)*10;
        int one=step_log[1]-48;
        game_round=ten+one+1;
    }

    fclose(BoardStateF);
    fclose(LeftPatternF);
    fclose(LogF);

    /////////////////////////////////////////
    /*----------------start----------------*/
    /////////////////////////////////////////
    //if round = 1
    if(is_first_round()){
        //block number's range = [10,20] (larger size better)
        choice=(rand()%10+10);
        //random rotation & reflection

        if(board_state[4][4]=='N'){
            x=4;y=4;
            flag=true;
        }
        else{
            x=9;y=9;
            flag=false;
        }
        if(flag==true){
            rotation=0;
            reflection=0;
        }
        else{
            rotation=2;
            reflection=0;
        }
        put_block(x,y,choice,rotation,reflection,board_state);
        left_pattern[choice]='0';

    }
    //if round > 1
    else{
        int r,p=6,q=4,cnt=36;
        //larger block first & from inside to outside
        int start=clock();
        do{
            //choose block
            do{
                //range = 10~20
                choice=(rand()%11+9);
            }while(( left_pattern[choice]=='0' || dead(choice) ) && (clock()-start<1200) );
            while(( left_pattern[choice]=='0' || dead(choice) ) &&  ((clock()-start)<2000) &&  ((clock()-start)>=1200)){
                //range = 3~21
                choice=(rand()%19)+2;
            }
            //chance of degree
            r=0;
            while(!putted && (r<10) && ((clock()-start)<2000)){
                //random degree
                rotation=(rand()%4);
                reflection=(rand()%2);
                //consider opponent's log
                do{
                    opp_coordinate(&x,&y,opp_log);
                    x=x+rand()%11-5;
                    y=y+rand()%11-5;
                    putted=legality(choice,x,y,rotation,reflection,false);
                }while(!putted && game_round<=12 && ((clock()-start)<500) );
                //consider oppside
                while(!putted && (cnt>0) && ((clock()-start)<1200) && ((clock()-start)>=500)){
                    if(board_state[4][4]==color){
                        x=rand()%14;
                        y=rand()%(x+3)+(11-x);
                        if(y>13) y=rand()%14;
                    }
                    else{
                        x=rand()%14;
                        y=rand()%(13-x+3);
                        if(y>13) y=rand()%14;
                    }
                    putted=legality(choice,x,y,rotation,reflection,false);
                }
                //from inside to outside
                cnt=p*p;
                while(!putted && (cnt>0) && ((clock()-start)<2000) && ((clock()-start)>=1200)){
                    x=rand()%p+q;
                    y=rand()%p+q;
                    cnt--;
                    putted=legality(choice,x,y,rotation,reflection,false);
                }
                r++;
            }
            p+=2;
            q--;
        }while(!putted && p<=14 && ((clock()-start)<2000)) ;
        //brute force
        if(!putted){
            for(i=0;i<14;i++){
                for(j=0;j<14;j++){
                    for(k=0;k<21;k++){
                        for(l=0;l<4;l++){
                            for(m=0;m<2;m++){
                                if(left_pattern[k]=='1' && board_state[i][j]=='N'){
                                    putted=legality(k,i,j,l,m,false);
                                    choice=k;
                                    if(putted){
                                        goto EXIT;
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
        EXIT:
        if(putted){
            pass=false;
            left_pattern[choice]='0';
        }
        else {
            pass=true;
        }
    }
    /////////////////////////////////////////////////////
    /*----------------write to file--------------------*/
    /////////////////////////////////////////////////////
    BoardStateF=fopen(argv[2],"w");
    LeftPatternF=fopen(argv[3],"a");
    //LogF=fopen(argv[4],"a");
    for(i=0;i<14;i++){
        for(j=0;j<14;j++){
            fprintf(BoardStateF,"%c",board_state[j][i]);
            if(j!=13) fprintf(BoardStateF,",");
        }
        fprintf(BoardStateF,"\n");
    }
    if(!pass){

        fprintf(LeftPatternF,"\n[");
        for(i=0;i<21;i++){
            fprintf(LeftPatternF,"%c",left_pattern[i]);
            if(i!=20) fprintf(LeftPatternF,",");
        }
        fprintf(LeftPatternF,"]");
    }
    fclose(BoardStateF);
    fclose(LeftPatternF);
    //fclose(LogF);
    if(!pass)
        return 0;
    else
        return 1;

}
bool is_first_round(){
    int i=0;
    for(i=0;i<21;i++){
        if(left_pattern[i]=='0') return false;
    }
    return true;
}
void opp_coordinate(int* x,int* y,Opp_Log* opp_log){
    int i,n;
    int count=0;
    for(i=0;i<6;i++){
        if(opp_log[i].x){
            count++;
        }
    }
    n=rand()%count;
    *x=opp_log[n].x;
    *y=opp_log[n].y;
}
bool legality(int choice,int x,int y,Rotation rot,Reflection ref,bool test){
    int i,j;
    int cnt=0;
    char temp[14][14];
    bool success;
    bool flag=false;
    for(i=0;i<14;i++){
        for(j=0;j<14;j++) temp[i][j]='N';
    }
    //put into temp[][] and test first round
    success=put_block(x,y,choice,rot,ref,temp);
    if(!success) return false;

    for(i=0;i<14;i++){
        for(j=0;j<14;j++){

            //condition with overlay
            if(board_state[i][j]!='N'&&temp[i][j]!='N')return false;
            //condition with same color
            if(i>0&&i<13&&j>0&&j<13){
                if(board_state[i][j]!='N'){
                    if(temp[i+1][j]==board_state[i][j]||
                        temp[i-1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j]
                        )
                        return false;
                    if(temp[i-1][j-1]==board_state[i][j]||
                       temp[i+1][j-1]==board_state[i][j]||
                       temp[i-1][j+1]==board_state[i][j]||
                       temp[i+1][j+1]==board_state[i][j])
                       flag=true;

                }
            }
            else if(i==0 && j!=0 && j!=13){
                if(board_state[i][j]!='N'){
                    if(temp[i+1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j])
                        return false;
                    if(
                       temp[i+1][j-1]==board_state[i][j]||
                       temp[i+1][j+1]==board_state[i][j])
                       flag=true;
                }
            }
            else if(i==0 && j==0){
                if(board_state[i][j]!='N'){
                    if(temp[i+1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]
                        )
                        return false;
                    if(

                       temp[i+1][j+1]==board_state[i][j])
                       flag=true;
                }
            }
            else if(i==0 && j==13){
                if(board_state[i][j]!='N'){
                    if(temp[i+1][j]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j])
                        return false;
                    if(
                       temp[i+1][j-1]==board_state[i][j]
                       )
                       flag=true;
                }
            }
            else if(i==13 && j!=0 && j!=13){
                if(board_state[i][j]!='N'){
                    if(temp[i-1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j])
                        return false;
                    if(temp[i-1][j-1]==board_state[i][j]||
                       temp[i-1][j+1]==board_state[i][j]
                       )
                       flag=true;
                }
            }
            else if(i==13 && j==0){
                if(board_state[i][j]!='N'){
                    if(temp[i-1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]
                        )
                        return false;
                    if(

                       temp[i-1][j+1]==board_state[i][j])
                       flag=true;
                }
            }
            else if(i==13 && j==13){
                if(board_state[i][j]!='N'){
                    if(temp[i-1][j]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j])
                        return false;
                    if(
                       temp[i-1][j-1]==board_state[i][j]
                       )
                       flag=true;
                }
            }
            else if(j==0 && i!=0 && i!=13){
                if(board_state[i][j]!='N'){
                    if(temp[i+1][j]==board_state[i][j]||
                        temp[i][j+1]==board_state[i][j]||
                        temp[i-1][j]==board_state[i][j]
                        )
                        return false;
                    if(
                       temp[i+1][j+1]==board_state[i][j]||
                       temp[i-1][j+1]==board_state[i][j])
                       flag=true;
                }
            }
            else if(j==13 && i!=0 && i!=13){
                if(board_state[i][j]!='N'){
                    if(temp[i-1][j]==board_state[i][j]||
                        temp[i+1][j]==board_state[i][j]||
                        temp[i][j-1]==board_state[i][j]
                        )
                        return false;
                    if(
                       temp[i+1][j-1]==board_state[i][j]||
                       temp[i-1][j-1]==board_state[i][j])
                       flag=true;
                }
            }

            if(temp[i][j]!='N') cnt++;
        }
    }
    //condition with out of board
    if(cnt == 0) return false;
    //just test?
    if(!flag) return false;
    if(test){
        return true;
    }
    else{
        put_block(x,y,choice,rot,ref,board_state);
        return true;
    }
    //if work , put into board_state[][]
}


bool put_block(int x,int y,int choice,Rotation rotation,Reflection reflection,char temp[][14]){
    //does y coordinate extend to up or down
    int ref_sign;
    //coefficient add on  x,y
    int c,d;
    if(reflection==f_ori){
        ref_sign=1;
    }
    else {
        ref_sign=-1;
    }
    int rot_0,rot_90,rot_180,rot_270;
    //mask of rotation
    switch(rotation){
    case t_ori:
        rot_0=1;
        rot_90=0;
        rot_180=0;
        rot_270=0;
        break;
    case rotate90:
        rot_0=0;
        rot_90=1;
        rot_180=0;
        rot_270=0;
        break;
    case rotate180:
        rot_0=0;
        rot_90=0;
        rot_180=1;
        rot_270=0;
        break;
    case rotate270:
        rot_0=0;
        rot_90=0;
        rot_180=0;
        rot_270=1;
        break;
    }

    switch(choice){
    case 0:
        temp[x][y]=color;
        break;
    case 1:
        temp[x][y]=color;
        c=1;d=0*ref_sign;
        //result of formula derivation
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 2:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 3:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 4:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 5:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 6:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
            temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
            temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 7:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 8:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=3;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 9:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 10:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 11:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=2*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 12:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
           c=0;d=2*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 13:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
           c=2;d=1*ref_sign;
           if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
           c=2;d=2*ref_sign;
           if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
           c=1;d=1*ref_sign;
           if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 14:

        temp[x][y]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=2*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 15:

        temp[x][y]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=2*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 16:

        temp[x][y]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=-1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=2*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 17:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=3;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=0;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 18:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=3;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 19:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=3;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=1;d=1*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    case 20:

        temp[x][y]=color;
        c=1;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=2;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=3;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;
        c=4;d=0*ref_sign;
        if((x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)>13||
           (x+c*rot_0-d*rot_90-c*rot_180+d*rot_270)<0||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)>13||
           (y+d*rot_0+c*rot_90-d*rot_180-c*rot_270)<0) return false;
        temp[x+c*rot_0-d*rot_90-c*rot_180+d*rot_270][y+d*rot_0+c*rot_90-d*rot_180-c*rot_270]=color;

        break;
    }
    return true;
}
bool dead(int k){
    int i,j;

    for(i=0;i<14;i++){
        for(j=0;j<14;j++){
            if(left_pattern[k] && board_state[i][j]=='N'){
                //test all degree
                if(legality(k,i,j,0,0,true)||
                   legality(k,i,j,0,1,true)||
                   legality(k,i,j,1,0,true)||
                   legality(k,i,j,1,1,true)||
                   legality(k,i,j,2,0,true)||
                   legality(k,i,j,2,1,true)||
                   legality(k,i,j,3,0,true)||
                   legality(k,i,j,3,1,true))
                    return false;
            }

        }
    }
    return true;
}