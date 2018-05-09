#include "666.h"
#include "ui_666.h"

int tx_code[][4]={{456,258,456,258},{142,142,142,142},{246,268,123,248},{128,467,289,346},
                  {238,146,278,469},{247,126,247,126},{269,234,269,234}};//图形代码

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    connect(this, &MainWindow::GameOverSignal, this, &MainWindow::GameOver);
    //this->resize((SCENE_H+4)*REC_SIZE,(SCENE_W+0)*REC_SIZE);  //设置窗口大小
    SCENE_H = 10;  //场景列数
    SCENE_W = 20;  //场景行数
    REC_SIZE = 20; //方块大小
    /*
    for(int i=0;i<SCENE_W;i++)  //清空舞台信息
        memset(scene_num[i],0,sizeof(int) * SCENE_H);
        */
    ui->setupUi(this);
    Startflag = false;
    ui->menuSpeed->setTitle(tr("Speed: 0")); //设置标签信息
    this->resize((SCENE_H+SCENE_H/2)*REC_SIZE,(SCENE_W+0)*REC_SIZE);  //设置窗口大小
    ui->line->move((SCENE_H)*REC_SIZE,0);//移动分割线位置
    SetSpeed(0);//设置下落速度
    //SetLevel(1);
    memset(scene_num,0,sizeof(scene_num));
    //StartGame();  //开始游戏
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartGame()       //初始化
{
    Startflag = true;
    memset(uselevel,false,sizeof(uselevel));
    ui->actionStart_S->setText(tr("New Game"));
    ui->label_3->setText(tr("Level :  0"));
    ui->scoreLCD->display(score);
//    this->resize((SCENE_H)*REC_SIZE,(SCENE_W+0)*REC_SIZE);  //设置窗口大小
    /***************/
    /*
     * ui->line->setLineWidth((SCENE_W+0)*REC_SIZE+100);
    ui->line->move((SCENE_H)*REC_SIZE,0);//移动分割线位置
    //ui->label->
    ui->label->setText(tr("Score"));//设置label的文字
    ui->label->move((SCENE_H)*REC_SIZE+6,(SCENE_W+0)*REC_SIZE-70);//移动label的位置
    ui->label_2->setText(QString::number(score));//设置label的文字
    ui->label_2->move((SCENE_H)*REC_SIZE+6,(SCENE_W+0)*REC_SIZE-50);//移动label的位置
    for(int i=0;i<SCENE_W;i++)  //清空舞台信息
        memset(scene_num[i],0,sizeof(int) * SCENE_H);
        */
    /***************/
    ui->label_2->setText("");//设置label的文字
    ui->label->setText("");//设置label的文字
    score = 0; //分数初始化
    fresh_ms = 100;//设置刷新速度
    ui->scoreLCD->display(score);//显示分数
    color1 = Qt::red, color2 = Qt::blue;
    memset(scene_num,0,sizeof(scene_num));//清空舞台信息
    if(Levelflag) UseLevel();//设置等级并消除等级标记
    else {
        SetLevel(0);
        UseLevel();
    }
    rpoint[0][0].pos_x = 1;
    rpoint[0][0].pos_y = SCENE_H/2;//设置主方块位置信息
    srand(time(0));
    REC_tx[0] = (qrand()%1000)%7;
    REC_tx[1] = (qrand()%1000)%7;//随机出现第一个图形
    //change(3,SCENE_H+3,tx_code[REC_tx[1]][0],2);
    change(rpoint[0][0].pos_x,rpoint[0][0].pos_y,tx_code[REC_tx[0]][0],2);
    Pauseflag = false;
    //Levelflag=false;
    SetSpeed(0);
    PlayGame();//开始下落
}

void MainWindow::trychange(int x, int y, int num, REC_Point tmppoint[4]) //对方块进行模拟变换
{
    int temp = num;
    for(int i=1;i<4;i++){
         tmppoint[i].pos_x=x-1+(num%10-1)/3;
         tmppoint[i].pos_y=y-1+(num%10-1)%3;
         num = num/10;
     }
    tmppoint[0].pos_x = x;
    tmppoint[0].pos_y = y;
    if(258 == temp){
         tmppoint[2].pos_x=x+2;
         tmppoint[2].pos_y=y;
     }
    else if(456 == temp){
         tmppoint[2].pos_x=x;
         tmppoint[2].pos_y=y+2;
     }
}

void MainWindow::change(int x, int y, int num,int state)        //设置本次位置信息
{
   int temp = num;
   for(int i=1;i<4;i++){
        rpoint[0][i].pos_x=x-1+(num%10-1)/3;
        rpoint[0][i].pos_y=y-1+(num%10-1)%3;
        scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y]=state;
        num = num/10;
    }
   scene_num[x][y]=state;
   rpoint[0][0].pos_x = x;rpoint[0][0].pos_y = y;
   if(258 == temp){
        scene_num[x+2][y]=state;rpoint[0][2].pos_x=x+2;rpoint[0][2].pos_y=y;
    }
   else if(456 == temp){
        scene_num[x][y+2]=state;rpoint[0][2].pos_x=x;rpoint[0][2].pos_y=y+2;
    }
}

void MainWindow::clear(int x, int y, int num)     //清除上次位置信息
{
    if(258 == num)
        scene_num[x+2][y]=0;
    else if(456 == num)
        scene_num[x][y+2]=0;
    for(int i=0;i<3;i++){
        scene_num[x-1+(num%10-1)/3][y-1+(num%10-1)%3]=0;
        num = num/10;
    }
    scene_num[x][y] =0;
}

void MainWindow::weizhi(int x,int y,int tx,direction direct)
{                           //x,y是坐标,tx是7种形状，direct是方向
    if(Startflag==false || Pauseflag == true) return ;

    static int temp_tx;
    bool temp_flag;
    temp_flag = true;

    switch(direct) {

    case UP: //变换方块动作
        /**/
        REC_Point tmppoint[4];
        memcpy(tmppoint,rpoint[0],sizeof(tmppoint));//复制当前方块位置信息
        trychange(x,y,tx_code[tx][(temp_tx+1)%4],tmppoint);//对方块进行模拟变换
        /**/
        for(int i=0;i<4;i++)        //判断方块是否允许变换
            temp_flag = temp_flag
                    && tmppoint[i].pos_y >= 0
                    && tmppoint[i].pos_x < SCENE_W-1
                    && tmppoint[i].pos_x != 0
                    && tmppoint[i].pos_y < SCENE_H
                    && scene_num[tmppoint[i].pos_x][tmppoint[i].pos_y]!=1;
        if(temp_flag){              //执行变换动作
            clear(x,y,tx_code[tx][temp_tx++]);
            if(temp_tx >= 4)
                temp_tx = 0;
            change(x,y,tx_code[tx][temp_tx],2);
        }break;

    case DOWN://下移
        for(int i=0;i<4;i++)        //判断是否允许下移
            temp_flag = temp_flag && scene_num[rpoint[0][i].pos_x+1][rpoint[0][i].pos_y] !=1 &&
                        scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y] !=1 && rpoint[0][i].pos_x+1 < SCENE_W;
        if(temp_flag){              //执行下移动作
            clear(x,y,tx_code[tx][temp_tx]);change(x+1,y,tx_code[tx][temp_tx],2);
        }else{
            //落下后更改状态为静止
            change(x,y,tx_code[tx][temp_tx],1);
            //判断是否满行
            int score_add = SCENE_H;
            for(int i=0;i<SCENE_W;i++){
                for(int j=0;j<SCENE_H;j++){     //将满行的信息消除
                    if(scene_num[i][j]==0)
                        break;
                    else if(j==SCENE_H-1){
                        move(i,DOWN);//从第i行开始整体下移
                        score += score_add;
                        score_add += SCENE_H/2;
                        /*
                        int tmp = score, cnt = 0;
                        char str[17];
                        while(tmp) {
                            str[cnt++] = tmp%10 + '0';
                            tmp /= 10;
                        }
                        str[cnt] = '\0';
                        for(int p=0, q=cnt-1 ; p<q ; p++,q--) {
                            char temp = str[p];
                            str[p] = str[q];
                            str[q] = temp;
                        }
                        */
                        //ui->label_2->setText(QString::number(score));//设置label的文字
                        //ui->menuScore->setTitle(QString::number(score));
                        //emit CurrentScore();

                        /*******/ //难度判定与处理
                        if(!uselevel[5]) { //开始难度判定
                            if(!uselevel[5]&&score>=50 * SCENE_H) {
                                SetLevel(5);
                                ui->label_3->setText(tr("Level :  6"));
                                uselevel[5] = true;
                            }
                            else if(!uselevel[4]&&score>=32 * SCENE_H) {
                                SetLevel(4);
                                SetSpeed(4);
                                ui->label_3->setText(tr("Level :  5"));
                                uselevel[4] = true;
                            }
                            else if(!uselevel[3]&&score>=15 * SCENE_H) {
                                SetLevel(3);
                                SetSpeed(3);
                                ui->label_3->setText(tr("Level :  4"));
                                uselevel[3] = true;
                            }
                            else if(!uselevel[2]&&score>=8 * SCENE_H) {
                                SetLevel(2);
                                SetSpeed(2);
                                ui->label_3->setText(tr("Level :  3"));
                                uselevel[2] = true;
                            }
                            else if(!uselevel[1]&&score>=6 * SCENE_H) {
                                SetLevel(1);
                                SetSpeed(1);
                                ui->label_3->setText(tr("Level :  2"));
                                uselevel[1] = true;
                            }
                        } else { //到达Level5之后，每消4行增加一行随机行
                            LevelFive ++;
                            if(LevelFive >= 4) {
                                SetLevel(1);
                                LevelFive = 0;
                            }
                        }      //结束难度判定
                        if(Levelflag) {
                            UseLevel();
                        }
                        /*******/
//                        this->setWindowTitle(QString::number(score));
                        ui->scoreLCD->display(score);
                        break;
                    }
                }
            }
            //判断是否结束游戏
            if(scene_num[0][y] == 1){
                killTimer(timerid1);killTimer(timerid2);
                Startflag = false;
                emit GameOverSignal();
                break;
            }
            /*
            //判断是否加难度
            if(Levelflag){
                SetLevel();
                Levelflag=false;
            }
            */
            //重新构造一个方块
            rpoint[0][0].pos_x = 1;rpoint[0][0].pos_y = SCENE_H/2;//设置主方块位置信息
            REC_tx[0]=REC_tx[1];//随机出现第一个图形
            REC_tx[1] = rand()%7;
            change(rpoint[0][0].pos_x,rpoint[0][0].pos_y,tx_code[REC_tx[0]][0],2);
            temp_tx = 0;
        }
        break;

    case LEFT://左移
        for(int i=0;i<4;i++)        //判断是否允许左移
            temp_flag = temp_flag && scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y-1] !=1 && rpoint[0][i].pos_y-1 >=0;
        if(temp_flag){              //执行左移动作
        clear(x,y,tx_code[tx][temp_tx]);change(x,y-1,tx_code[tx][temp_tx],2);
        }break;

    case RIGHT://右移
        for(int i=0;i<4;i++)        //判断是否允许右移
            temp_flag = temp_flag && scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y+1] !=1 && rpoint[0][i].pos_y+1 < SCENE_H;
        if(temp_flag){              //执行右移动作
        clear(x,y,tx_code[tx][temp_tx]);change(x,y+1,tx_code[tx][temp_tx],2);
        }break;

    case SPACE://快速下移
        //int tempx;
        for(int j=0;j<SCENE_W;j++){
            for(int i=0;i<4;i++)        //判断是否允许下移
                temp_flag = temp_flag && scene_num[rpoint[0][i].pos_x+1][rpoint[0][i].pos_y] !=1 &&
                            scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y] !=1 &&
                        rpoint[0][i].pos_x+1 < SCENE_W;
            if(temp_flag){              //执行下移动作
                clear(x+j,y,tx_code[tx][temp_tx]);
                change(x+j+1,y,tx_code[tx][temp_tx],2);
                //change(x+j+1,y,tx_code[tx][temp_tx],1);
                //tempx = x+j+1;
            }else break;
        }
        //落下后更改状态为静止
        //change(tempx,y,tx_code[tx][temp_tx],1);
        break;
    }
}

void MainWindow::paintEvent(QPaintEvent */*event*/)
{
    //设置画笔
    QPainter painter(this);
    //刷新舞台
    for(int i=0;i<SCENE_W;i++){
        for(int j=0;j<SCENE_H;j++){
            if(scene_num[i][j] == 1) {
                painter.setBrush(QBrush(color2,Qt::SolidPattern));
                painter.drawRect(j*REC_SIZE,i*REC_SIZE,REC_SIZE,REC_SIZE);
            } else if(scene_num[i][j] == 2){
                painter.setBrush(QBrush(color1,Qt::SolidPattern));
                painter.drawRect(j*REC_SIZE,i*REC_SIZE,REC_SIZE,REC_SIZE);
            }
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(!Pauseflag && timerid1==event->timerId()){
        weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],DOWN);//下移
        //ui->scoreLCD->display(score);
    }
    if(!Pauseflag && timerid2==event->timerId())  this->update(); //刷新绘图
}

void MainWindow::keyPressEvent(QKeyEvent *event)  //键盘事件
{
    if(!Pauseflag)
        switch(event->key()){
        case Qt::Key_Up:        //按下方向键上时
            weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],UP);break;
        case Qt::Key_Down:        //按下方向键下时
            weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],DOWN);break;
        case Qt::Key_Left:        //按下方向键左时
            weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],LEFT);break;
        case Qt::Key_Right:        //按下方向键右时
            weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],RIGHT);break;
        case Qt::Key_Space:     //快速下移
            weizhi(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],SPACE);break;
        }
}

void MainWindow::move(int row, direction direct)
{
    if(direct == UP){
        for(int i=0;i<row;i++)
            for(int j=0;j<SCENE_H;j++)
                scene_num[i][j] = scene_num[i+1][j];
    }else if(direct == DOWN){
        for(int i=row;i>0;i--)
            for(int j=0;j<SCENE_H;j++)
                scene_num[i][j] = scene_num[i-1][j];
    }
}

/*
void MainWindow::SetHard(int level)
{
    Levelflag = true;
    SetLevel();
}
*/

void MainWindow::GameOver()
{
    /*
    if(score > MaxScore)
        QMessageBox::about(this, "New High Score",
                           QString("<p align='center'>Unbelievable!\n</p><p align='center'>Your score is : ")
                           + QString::number(score) + QString("</p>"));
    else
        QMessageBox::about(this, "GameOver",
                           QString("<p align='center'>Your score is : ")
                           + QString::number(score) + QString("</p>"));
        */
    QMessageBox::about(this, "GameOver",
                       QString("<p align='center'>Your score is : ")
                       + QString::number(score) + QString("</p>"));
    Startflag = false;
}

void MainWindow::SetSpeed(int level)  //设置速度等级
{
    speed_ms = 600 - level * 90;
    //speed_ms = 600;
}

void MainWindow::SetLevel(int level)//设置随机行标记
{
    HardLevel = level;
    Levelflag = true;
}

void MainWindow::UseLevel()//实现随机行
{
    for(int i=0;i<HardLevel;i++){
        move(SCENE_W-1,UP);
        for(int j=0;j<SCENE_H;j++)
            scene_num[SCENE_W-1][j] = rand()%2;
    }
    Levelflag = false;
}

void MainWindow::PlayGame()
{
    timerid1 = startTimer(speed_ms);//设置下落速度
    timerid2 = startTimer(fresh_ms);//设置刷新速度
    Pauseflag = false;
}

void MainWindow::PauseGame()
{
    killTimer(timerid1);killTimer(timerid2);
    Pauseflag = true;
}

void MainWindow::on_actionPause_P_triggered()
{
    if(Startflag) {
        if(Pauseflag) {
            PlayGame();
            ui->actionPause_P->setText(tr("Pause"));
        }
        else {
            PauseGame();
            ui->actionPause_P->setText(tr("Play"));
        }
    }
    else {
        StartGame();
    }
}

void MainWindow::on_actionStart_S_triggered()
{
    StartGame();
}
/************/
//设置下落速度
void MainWindow::on_action1_triggered()
{
    ui->menuSpeed->setTitle(tr("Speed: 1"));
    SetSpeed(1);
    if(Startflag && !Pauseflag) {
            PauseGame();
            PlayGame();
        }
}

void MainWindow::on_action2_triggered()
{
    ui->menuSpeed->setTitle(tr("Speed: 2"));
    SetSpeed(2);
    if(Startflag && !Pauseflag) {
            PauseGame();
            PlayGame();
        }
}

void MainWindow::on_action3_triggered()
{
    ui->menuSpeed->setTitle(tr("Speed: 3"));
    SetSpeed(3);
    if(Startflag && !Pauseflag) {
            PauseGame();
            PlayGame();
        }
}

void MainWindow::on_action4_triggered()
{
    ui->menuSpeed->setTitle(tr("Speed: 4"));
    SetSpeed(4);
    if(Startflag && !Pauseflag) {
            PauseGame();
            PlayGame();
        }
}

void MainWindow::on_action5_triggered()
{
    ui->menuSpeed->setTitle(tr("Speed: 5"));
    SetSpeed(5);
    if(Startflag && !Pauseflag) {
            PauseGame();
            PlayGame();
        }
}
/************/

/*
void MainWindow::on_actionRow_Column_triggered()
{
    //QMessageBox::about(this,"Hello World!","Hello World!");
    //query = new Query;
    QDialog *query = new QDialog;
    query->setWindowTitle("Enter the Width and Height");
    QLineEdit *WidthLineEdit = new QLineEdit, *HeightLineEdit = new QLineEdit;//宽和高编辑器
    query->setLayout();
    query->show();
    //QLineEdit *WidthLineEdit = new QLineEdit, *HeightLineEdit = new QLineEdit;//宽和高编辑器
}
*/
void MainWindow::on_action1_2_triggered()
{
    SetLevel(1);
}

void MainWindow::on_action2_2_triggered()
{
    SetLevel(2);
}

void MainWindow::on_action3_2_triggered()
{
    SetLevel(3);
}

void MainWindow::on_action4_2_triggered()
{
    SetLevel(4);
}

void MainWindow::on_action5_2_triggered()
{
    SetLevel(5);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,"About","666");
}

void MainWindow::on_actionHow_to_Play_triggered()
{
    QMessageBox::about(this,"How to Play","666");
}
