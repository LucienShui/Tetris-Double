#include "666.h"
#include "ui_666.h"

//图形代码
int tx_code[][4]={{456,258,456,258},{142,142,142,142},{246,268,123,248},{128,467,289,346},
                  {238,146,278,469},{247,126,247,126},{269,234,269,234}};

struct RANK{
    int rank,score;
    QString name;
    bool operator < (const RANK tmp) {
        return score > tmp.score;
    }
}ranklist[7];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    connect(this, &MainWindow::GameOverSignal, this, &MainWindow::GameOver);
    SCENE_H = 10;  //场景列数
    SCENE_W = 20;  //场景行数
    REC_SIZE = 20; //方块大小
    ui->setupUi(this);
    Startflag = false;
    ui->menuSpeed->setTitle(tr("Speed: 0"));  //设置标签信息
    this->resize((SCENE_H+SCENE_H/2)*REC_SIZE,(SCENE_W+0)*REC_SIZE);  //设置窗口大小
    ui->line->move((SCENE_H)*REC_SIZE,0);  //移动分割线位置
    SetSpeed(0);  //设置下落速度
    memset(scene_num,0,sizeof(scene_num));
    db.createConnection();
    db.createTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * 初始化游戏
 */
void MainWindow::StartGame()
{
    Startflag = true;
    memset(uselevel,false,sizeof(uselevel));
    ui->actionStart_S->setText(tr("New Game"));
    ui->levelLCD->display(1);
    ui->scoreLCD->display(score);
    ui->label_2->setText("");  //设置label的文字
    ui->label->setText("");  //设置label的文字
    score = 0;  //分数初始化
    fresh_ms = 100;  //设置刷新速度
    ui->scoreLCD->display(score);  //显示分数
    color1 = Qt::red, color2 = Qt::blue;
    memset(scene_num,0,sizeof(scene_num));  //清空舞台信息
    if(Levelflag) UseLevel();  //设置等级并消除等级标记
    else
    {
        SetLevel(0);
        UseLevel();
    }
    rpoint[0][0].pos_x = 1;
    rpoint[0][0].pos_y = SCENE_H/2;  //设置主方块位置信息
    srand(time(0)); //设置随机数种子
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));  //设置随机数种子
    REC_tx[0] = (qrand()%7+qrand()%7)%7;
    REC_tx[1] = qrand()%7;  //随机出现第一个图形
    change(rpoint[0][0].pos_x,rpoint[0][0].pos_y,tx_code[REC_tx[0]][0],2);
    Pauseflag = false;
    SetSpeed(0);
    PlayGame();  //开始下落
}

/*
 * 对方块进行模拟变换
 */
void MainWindow::trychange(int x, int y, int num, REC_Point tmppoint[4])
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

/*
 * 设置本次位置信息
 */
void MainWindow::change(int x, int y, int num,int state)
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

/*
 * 清除上次位置信息
 */
void MainWindow::clear(int x, int y, int num)
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

void MainWindow::position(int x,int y,int tx,direction direct)
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
        if(temp_flag)  //执行下移动作
        {
            clear(x,y,tx_code[tx][temp_tx]);change(x+1,y,tx_code[tx][temp_tx],2);
        }
        else  //落下后更改状态为静止
        {
            change(x,y,tx_code[tx][temp_tx],1); //判断是否满行
            int score_add = SCENE_H;
            for(int i=0;i<SCENE_W;i++)
            {
                for(int j=0;j<SCENE_H;j++)  //将满行的信息消除
                {
                    if(scene_num[i][j]==0)
                        break;
                    else if(j==SCENE_H-1)
                    {
                        move(i,DOWN);  //从第i行开始整体下移
                        score += score_add;
                        score_add += SCENE_H/2;

                        /*
                         * 难度判定与处理
                         */
                        if(!uselevel[5]) { //开始难度判定
                            if(!uselevel[5]&&score>=50 * SCENE_H) {
                                SetLevel(5);
                                ui->levelLCD->display(6);
                                uselevel[5] = true;
                            }
                            else if(!uselevel[4]&&score>=32 * SCENE_H) {
                                SetLevel(4);
                                SetSpeed(4);
                                ui->levelLCD->display(5);
                                uselevel[4] = true;
                            }
                            else if(!uselevel[3]&&score>=15 * SCENE_H) {
                                SetLevel(3);
                                SetSpeed(3);
                                ui->levelLCD->display(4);
                                uselevel[3] = true;
                            }
                            else if(!uselevel[2]&&score>=8 * SCENE_H) {
                                SetLevel(2);
                                SetSpeed(2);
                                ui->levelLCD->display(3);
                                uselevel[2] = true;
                            }
                            else if(!uselevel[1]&&score>=6 * SCENE_H) {
                                SetLevel(1);
                                SetSpeed(1);
                                ui->levelLCD->display(2);
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
        for(int j=0;j<SCENE_W;j++){
            for(int i=0;i<4;i++)        //判断是否允许下移
                temp_flag = temp_flag && scene_num[rpoint[0][i].pos_x+1][rpoint[0][i].pos_y] !=1 &&
                            scene_num[rpoint[0][i].pos_x][rpoint[0][i].pos_y] !=1 &&
                        rpoint[0][i].pos_x+1 < SCENE_W;
            if(temp_flag){              //执行下移动作
                clear(x+j,y,tx_code[tx][temp_tx]);
                change(x+j+1,y,tx_code[tx][temp_tx],2);
            }else break;
        }
        break;
    }
}
void MainWindow::paintEvent(QPaintEvent */*event*/)
{
    //设置画笔
    QPainter painter(this);
    painter.drawPixmap(0,0,400,1000,QPixmap(":/icon/white.jpg"));
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
        position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],DOWN);//下移
    }
    if(!Pauseflag && timerid2==event->timerId())  this->update(); //刷新绘图
}

void MainWindow::keyPressEvent(QKeyEvent *event)  //键盘事件
{
    if(!Pauseflag)
        switch(event->key()){
        case Qt::Key_Up:        //按下方向键上时
            position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],UP);break;
        case Qt::Key_Down:        //按下方向键下时
            position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],DOWN);break;
        case Qt::Key_Left:        //按下方向键左时
            position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],LEFT);break;
        case Qt::Key_Right:        //按下方向键右时
            position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],RIGHT);break;
        case Qt::Key_Space:     //快速下移
            position(rpoint[0][0].pos_x,rpoint[0][0].pos_y,REC_tx[0],SPACE);break;
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
void MainWindow::GameOver()
{
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
    QMessageBox::about(this,"How to Play","Press Enter to start!\nClear a line to get 10 points\nEvery extra line for 5 points!");
}

/*****************/

//DataBase:


//建立一个数据库连接
void DataBase::createConnection()
{
    //以后就可以用"sqlite1"与数据库进行连接了
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "sqlite1");
    db.setDatabaseName(".//qtDb.db");
    db.open();
}

//创建数据库表
void DataBase::createTable()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.exec("create table rank(id int primary key,Name varchar(36),"
                              "Score int)");
}

//向数据库中插入记录
void DataBase::insert(int i, QString Name, int Score)
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare("insert into rank values(?, ?, ?)");
    query.bindValue(0, i);
    query.bindValue(1, Name);
    query.bindValue(2, Score);
    rankcnt++;
    query.exec();
}

int cmp(const void *a, const void *b) {
    return ((RANK*)b)->score - ((RANK*)a)->score;
}

void DataBase::updateAll() {
    for(int i=0 ; i<=6 ; i++) {
        deleteById(i);
    }
    qsort(ranklist+1,5,sizeof(ranklist[0]),cmp);
    //sort(ranklist+1,ranklist+1+rankcnt);
    rankcnt = rankcnt<5 ? rankcnt : 5;
    for(int i=1 ; i<=rankcnt ; i++) {
        insert(i,ranklist[i].name,ranklist[i].score);
    }
}

//查询所有信息
void DataBase::queryAll()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.exec("select * from rank");
    int id,scr;
    QString name;
    //qDebug() << query.size();
    rankcnt = 0;
    while(query.next())
    {
        id = query.value(0).toInt();
        name = query.value(1).toString();
        scr = query.value(2).toInt();
        ranklist[++rankcnt].rank = id;
        ranklist[rankcnt].name = name;
        ranklist[rankcnt].score = scr;
        //qDebug() << "id = " << id << " name = " << name << " score = " << score;
    }
    for(int i=1 ; i<=rankcnt ; i++) {
        qDebug() << ranklist[i].rank << " " << ranklist[i].name << " " << ranklist[i].score;
    }
}

//根据ID删除记录
void DataBase::deleteById(int id)
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("delete from rank where id = %1").arg(id));
    query.exec();
}

/*
//根据ID更新记录
void DataBase::updateById(int id,QString Name, int Score)
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("update rank set Name=?,Score=? where id=%1").arg(id));

     query.bindValue(0,Name);
     query.bindValue(1,Score);

     query.exec();
}


//排序
void DataBase::sortById()
{
    QSqlDatabase db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.exec("select * from rank order by score desc");
}
*/






/*****************/
