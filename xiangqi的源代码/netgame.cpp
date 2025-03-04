#include "Netgame.h"
#include "ui_gamescene.h"
#include<QDebug>
#include <QPainter>
#include <QPen>
#include <QColor>
#include<QMessageBox>
#include <QLineEdit>
#include <QLabel>
NetGame::NetGame(bool isServer,QWidget* parent):Gamescene(parent)
{
    Server=NULL;
    Socket=NULL;
    disconnect(ui->pushButton_3,nullptr,nullptr,nullptr);
    ui->pushButton_3->disconnect();

    button1 = new QPushButton(this);
    button1->setGeometry(QRect(QPoint(740, 175), QSize(150, 50)));
    button1->setCursor(Qt::PointingHandCursor);

    // 创建第二个按钮
    button2 = new QPushButton(this);
    button2->setGeometry(QRect(QPoint(740, 250), QSize(150, 50)));
    button2->setCursor(Qt::PointingHandCursor);

    // 显示按钮
    button1->show();

    button2->show();


    if(isServer)
    {
        this->setWindowTitle("中国象棋对战（服务器端）");
        Server=new QTcpServer(this);//new 一个服务器
        Server->listen(QHostAddress::Any,80);
        //QHostAddresss保存IPv4或IPv6地址,此处表示服务器将监听所有网络接口
        //9999表示自己服务器的端口

        connect(Server,&QTcpServer::newConnection,this,&NetGame::afterConnection);
        connect(ui->pushButton_3,&QPushButton::clicked,[=](){
            char buf[3];
            buf[0]=3;//表示选择位置报文
            Socket->write(buf,1);
            selectid=-1;//取消选择，避免bug
            disconnect(ui->pushButton_3,nullptr,nullptr,nullptr);
            ui->pushButton_3->disconnect();
        });//悔棋按钮连接再添加功能,传报文去Socket


        connect(button1, &QPushButton::clicked, [=]() {
            disconnect(button1,nullptr,nullptr,nullptr);
            button1->disconnect();
            handleResign();//认输
        });

        connect(button2, &QPushButton::clicked, [=]() {
            disconnect(button2,nullptr,nullptr,nullptr);
            button2->disconnect();
            handleDrawRequest();//和棋
        });


    }
    else
    {
        this->setWindowTitle("中国象棋对战（客户端）");
        Socket=new QTcpSocket(this);//与上面同理

                /* 创建一个连接用的窗口*/
        QWidget *trycnt=new QWidget;
        trycnt->setWindowTitle("客户端连接尝试");
        trycnt->setFixedSize(600,150);

        QLabel *ipLabel=new QLabel(trycnt);
        ipLabel->setText("请输入要连接的服务器的ip(若主机环回，则填127.0.0.1):");
        ipLabel->setGeometry(0,20,300,30);

        QLabel *portLabel=new QLabel(trycnt);
        portLabel->setText("请输入要连接的服务器的端口（默认填80）:");
        portLabel->setGeometry(0,70,300,30);

        QLineEdit *ipEdit=new QLineEdit(trycnt);
        ipEdit->setGeometry(310,20,150,30);

        QLineEdit *portEdit=new QLineEdit(trycnt);
        portEdit->setGeometry(240,70,70,30);

        QPushButton *connect_=new QPushButton("连接",trycnt);
        connect_->setGeometry(450,70,100,30);

        trycnt->show();

        connect(connect_,&QPushButton::clicked,trycnt,[=](){
            //在点击连接按钮后，记录输入的ip和端口，然后尝试连接
            QString ip=ipEdit->text();
            QString port=portEdit->text();
            Socket->connectToHost(QHostAddress(ip),port.toInt());

            if(Socket->waitForConnected())//成功则关闭连接窗口
            {
                trycnt->hide();
                this->setWindowTitle("中国象棋对战（客户端）（服务器端连接成功）");
                QMessageBox test;
                test.setWindowTitle("连接成功！");
                test.setText("已成功连接到服务器！");
                test.exec();
            }


            else//连接失败，则提示连接失败
            {
                QMessageBox test;
                test.setWindowTitle("无法连接！");
                test.setText("没有能能连接到服务器，ip错误，或是端口错误。");
                test.exec();
            }
        });


        connect(button1, &QPushButton::clicked, [=]() {
            disconnect(button1,nullptr,nullptr,nullptr);
            button1->disconnect();
            handleResign();//认输
        });

        connect(button2, &QPushButton::clicked, [=]() {
            disconnect(button2,nullptr,nullptr,nullptr);
            button2->disconnect();
            handleDrawRequest();//和棋
        });


            /* 创建一个连接用的窗口*/

        connect(Socket,&QTcpSocket::disconnected,this,[=](){
             qDebug()<<"服务器端关闭";
            this->setWindowTitle("中国象棋对战（客户端）(服务器端连接已断开)");
        });

        //QHostAddress("127.0.0.1")
        // QString address="127.0.0.1";
        // Socket->connectToHost(QHostAddress(address),80);//连接到服务器
        /*
        127.0.0.1 是主机环回地址。主机环回是指地址为 127.0.0.1 的任何数
        据包都不应该离开计算机（主机），发送它——而不是被发送到本地网络
        或互联网，它只是被自己“环回”，并且发送数据包的计算机成为接收者。
        */
        connect(Socket,&QTcpSocket::readyRead,this,&NetGame::slotRect);
        connect(ui->pushButton_3,&QPushButton::clicked,[=](){
            char buf[3];
            buf[0]=3;//表示选择位置报文
            Socket->write(buf,1);
            disconnect(ui->pushButton_3,nullptr,nullptr,nullptr);
            ui->pushButton_3->disconnect();
        });//悔棋按钮连接再添加功能,传报文去Socket
    }
}

void NetGame::handleResign()//认输
{
    char buf[3];
    buf[0] = 7; // 表示认输报文
    buf[1] = token ? 1 : 0; // 1 表示红方认输，0 表示黑方认输
    selectid=-1;//取消选择，避免bug
    Socket->write(buf, 2);
    reset();
}

void NetGame::handleDrawRequest()//和棋
{
    char buf[2];
    selectid=-1;//取消选择，避免bug
    buf[0] = 8; // 表示和棋请求报文
    Socket->write(buf, 1);

}


NetGame::~NetGame()
{

}

void NetGame::closeEvent(QCloseEvent *event)
{
    if(Socket!=NULL)
    {
        Socket->close();
    }
    if(Server!=NULL)
    {
        Server->close();
    }
    event->accept();
}


void NetGame::StoneMove(int row,int col)//移动函数
{
    clicked = -1;
    int i;

    //判断是哪一个棋子被选中，根据ID（这里的局部i）来记录下来
    for(i = 0; i <= 31; i++)
    {
        if(stone[i].row == row && stone[i].col == col && stone[i].death == false)
            break;
    }

    if(i < 32)
    {
        clicked = i;//选中的棋子的ID
        //qDebug()<<stone[i].row<<' '<<stone[i].col;
    }

    if(selectid == -1)//选中棋子
    {
        if(clicked != -1)
        {
            if(redtrue == stone[clicked].red)//如果所点击的棋子是自己方，则选中
                selectid = clicked;
        }
    }
    else if(selectid!=-1&&clicked!=-1&&stone[selectid].red==stone[clicked].red)//想换个棋子选
    {
        selectid = clicked;
    }

    else//走棋子
    {
        if(canMove(selectid,clicked, row, col ))
        {
            Regretpacket();//记录当前棋盘

            int row_=stone[selectid].row;//记录原本的位置，若是触发“送将”则回溯
            int col_=stone[selectid].col;
            // 移动棋子的逻辑
            stone[selectid].row = row;
            stone[selectid].col = col;
            if(clicked != -1)
                stone[clicked].death = true;

            //当isDefeated输入的参数为0时，为“送将”函数。输入的参数为1时，为“被将”函数。

            if(isDefeated(0)&&redtrue)//一方移动完成后，判断是否送将，是则回溯
            {
                stone[selectid].row = row_;
                stone[selectid].col = col_;
                if(clicked != -1)
                    stone[clicked].death = false;
                //winMessageBox("提示", "本局结束，黑方胜利.");//test winmessageBox函数
                update();
                return ;

            }
            else if(isDefeated(1)&&!redtrue)
            {
                stone[selectid].row = row_;
                stone[selectid].col = col_;
                if(clicked != -1)
                    stone[clicked].death = false;
                //winMessageBox("提示", "本局结束，黑方胜利.");//test winmessageBox函数
                update();
                return ;
            }


            if(clicked != -1)
                stone[clicked].death = true;
            selectid = -1;// 取消选择
            //落子的声音
            stonemovemusic =new QSoundEffect(this);
            stonemovemusic->setSource(QUrl::fromLocalFile(":/Music/StoneKill.wav"));
            stonemovemusic->setLoopCount(1);
            stonemovemusic->play();
            if((isDefeated(1)||isDefeated(0))&&!stone[4].death&&!stone[20].death)
            {
                if(!tryflee())
                {
                    //这里弄个声音提示将军了
                    attackmusic =new QSoundEffect(this);
                    attackmusic->setSource(QUrl::fromLocalFile(":/Music/attack.wav"));
                    attackmusic->setLoopCount(1);
                    attackmusic->play();
                    selectid=-1;
                }
                else
                {
                    repaint();

                    if(redtrue)stone[20].death=true;
                    else stone[4].death=true;
                    selectid=-1;
                    //whoWin();    /*这个就是重载的意义*/
                    return ;
                }
            }


            if(redtrue)//红方回合，要悔棋就是黑方悔，所以搞成黑色
                ui->pushButton_3->setStyleSheet("color:black;font: 30pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: black;");

            else
                ui->pushButton_3->setStyleSheet("color:red;font: 30pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: red;");

            //判断完再换边
            redtrue = !redtrue;// 轮到对方
        }

    }
    update();// 刷新界面
}


void NetGame::mousePressEvent(QMouseEvent *ev)
{
    if(token!=redtrue)
        return;

    QPoint pt = ev->pos();
    //将pt转化成棋盘的像行列值
    int row,col,id;
    //判断这个行列值上面有没有棋子
    //点击棋盘外面就不做处理
    if(!getRowCol(pt, row, col))
        return;

    /* 然后发送报文给对方 */
    char buf[3];
    buf[0]=2;//表示选择位置报文
    buf[1]=row;
    buf[2]=col;
    Socket->write(buf,3);//无论是服务器还是客户端，都用Socket发送报文出去

    StoneMove(row,col);


    if(stone[4].death)
    {
        char win[2];
        win[0] = 6; // 表示绝杀的报文
        win[1]=1;
        Socket->write(win, 2);
    }
    else if(stone[20].death)
    {
        char win[2];
        win[0] =6 ; // 表示绝杀的报文
        win[1]=2;
        Socket->write(win, 2);
    }
    whoWin();
}



void NetGame::reset() // 分出胜负之后重置界面
{
    while(!Regret.empty())
    {
        Regret.pop();
    }

    // 重置所有棋子的状态
    for(int i=0;i<32;i++)
    {
        stone[i].initialize(i);//(initialize)初始化棋子
    }

    selectid = -1;
    clicked = -1;
    redtrue = true;//强制初始化为红方


    if(token)
    {
        for(int i=0;i<32;i++)
        {
            stone[i].col=8-stone[i].col;
            stone[i].row=9-stone[i].row;
        }
    }

    // 刷新界面
    update();
}


void NetGame::afterConnection()
{

    if(Socket)return;//如果已经有客户端连接了，则返回结束

    this->setWindowTitle("中国象棋对战（服务器端）（客户端成功连接）");

    Socket=Server->nextPendingConnection();//返回一个（Socket）

    connect(Socket,&QTcpSocket::readyRead,this,&NetGame::slotRect);
    //站在服务器角度，通过Socket接收数据。

    //qDebug()<<"123";

    /* 给服务器对方发送选方数据 */
    char buf[2];
    QMessageBox::StandardButton ret;
    ret=QMessageBox::question(NULL,"选红黑方","是否为服务器端选择红方（客户端玩家自动选择对应颜色）。");
    if(ret==QMessageBox::No)
    {
        buf[0]=1;
        buf[1]=0;//输入0，自己选黑方，对方选红方
        token=false;
    }
    else
    {
        buf[0]=1;
        buf[1]=1;//输入1，自己选红方，对方选黑方

        token=true;
    }

    if(token)//用token限制
    {
        button1->setText("红方认输");
        button2->setText("红方求和");
        button1->setStyleSheet("color:red;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: red;");
        button2->setStyleSheet("color:red;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: red;");
    }
    else
    {
        button1->setText("黑方认输");
        button2->setText("黑方求和");
        button1->setStyleSheet("color:black;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: black;");
        button2->setStyleSheet("color:black;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: black;");
    }


    Socket->write(buf,2);
    if(buf[1]==1)//棋盘翻转
    {
        for(int i=0;i<32;i++)
        {
            stone[i].col=8-stone[i].col;
            stone[i].row=9-stone[i].row;
        }
    }
}


void NetGame::slotRect()//接收
{
    QByteArray ba=Socket->readAll();//字节数组接收报文存放

    char cmd=ba[0];
    if(cmd==1)//接收选棋子颜色的信息
    {
        char data=ba[1];
        token=false;
        qDebug()<<data;
        if(data==0)//棋盘翻转
        {
            token=true;
            for(int i=0;i<32;i++)
            {
                stone[i].col=8-stone[i].col;
                stone[i].row=9-stone[i].row;
            }
        }

        if(token)//用token限制
        {
            button1->setText("红方认输");
            button2->setText("红方求和");
            button1->setStyleSheet("color:red;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: red;");
            button2->setStyleSheet("color:red;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: red;");
        }
        else
        {
            button1->setText("黑方认输");
            button2->setText("黑方求和");
            button1->setStyleSheet("color:black;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: black;");
            button2->setStyleSheet("color:black;font: 25pt 华文行楷;background-color:#ffbb46;border-radius:40px;border-style: solid; border-width: 2px; border-radius: 10px; border-color: black;");
        }
    }
//172.0.0.1
    else if(cmd==2)//接收对战信息
    {
        int row=ba[1];
        int col=ba[2];


        StoneMove(9-row,8-col);
        if(stone[4].death)
        {
            qDebug()<<"stone[4].death=true";
            char win[2];
            win[0] = 6; // 表示不同意悔棋的报文
            win[1]=1;
            Socket->write(win, 2);
        }
        else if(stone[20].death)
        {
            qDebug()<<"stone[20].death=true";
            char win[2];
            win[0] =6 ; // 表示不同意悔棋的报文
            win[1]=2;
            Socket->write(win, 2);
        }
        whoWin();
    }

    else if(cmd==6)
    //有个bug，解决方法很简单，一定会有一个一个棋盘是正常的。
    //我把whowin放在Gamescene的printevent里，whowin就变成时常检测的函数
    //正常运行的棋盘会有个将军被将死，然后发送相应的报文给对方
    //对方的将军接收报文，将军马上死掉，然后触发whowin
    {
        char win=ba[1];
        if(win==1)
        {
            stone[4].death=true;
            qDebug()<<"stone[4].death=true";
        }
        else if(win==2)
        {
            stone[20].death=true;
            qDebug()<<"stone[20].death=true";
        }
        whoWin();
    }

    else if(cmd==3)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("是否同意对方悔棋");
        msgBox.setText("对方请求悔棋，是否同意？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        selectid=-1;//取消选择，避免bug
        if (ret == QMessageBox::Yes)
        {
            Regretstone();
            // 发送同意悔棋的消息
            char response[2];
            response[0] = 4; // 表示同意悔棋的报文
            Socket->write(response, 1);
        }
        else
        {
            // 发送不同意悔棋的消息
            char response[2];
            response[0] = 5; // 表示不同意悔棋的报文
            Socket->write(response, 1);
        }
    }


    else if(cmd==4)//接收同意悔棋的消息
    {
        Regretstone();

        connect(ui->pushButton_3,&QPushButton::clicked,[=](){
            char buf[3];
            buf[0]=3;//表示选择位置报文
            Socket->write(buf,1);
            selectid=-1;//取消选择，避免bug
            disconnect(ui->pushButton_3,nullptr,nullptr,nullptr);
            ui->pushButton_3->disconnect();
        });//悔棋按钮连接再添加功能,传报文去Socket
    }
    else if(cmd==5)//接收不同意悔棋的消息
    {
        QMessageBox::information(this, "悔棋请求", "对方不同意悔棋。");
        connect(ui->pushButton_3,&QPushButton::clicked,[=](){
            char buf[3];
            buf[0]=3;//表示选择位置报文
            Socket->write(buf,1);
            selectid=-1;//取消选择，避免bug
            disconnect(ui->pushButton_3,nullptr,nullptr,nullptr);
            ui->pushButton_3->disconnect();
        });//悔棋按钮连接再添加功能,传报文去Socket}
    }
    else if(cmd==7)
    {
        //当用在认输时，可以这么重连
        connect(button1, &QPushButton::clicked, [=]() {
            disconnect(button1,nullptr,nullptr,nullptr);
            button1->disconnect();
            handleResign();//认输
        });

        char win=ba[1];
        if(win==1)
        {
            stone[4].death=true;
            whoWin();
            qDebug()<<"stone[4].death=true";
        }
        else if(win==0)
        {
            stone[20].death=true;
            whoWin();
            //qDebug()<<"stone[20].death=true";
        }
        //selectid=-1;//取消选择，避免bug
        //reset();
    }

    else if (cmd == 8) // 接收和棋请求报文
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("和棋请求");
        msgBox.setText("对方请求和棋，是否同意？");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        selectid=-1;//取消选择，避免bug
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            // 发送同意和棋的消息
            char response[2];
            response[0] = 9; // 表示同意和棋的报文
            Socket->write(response, 1);
            QMessageBox::information(this, "和棋", "双方同意和棋，游戏结束。");
            reset();
        }
        else
        {

            // 发送不同意和棋的消息
            char response[2];
            response[0] = 10; // 表示不同意和棋的报文
            Socket->write(response, 1);
        }
    }
    else if (cmd == 9) // 接收同意和棋的消息
    {
        connect(button2, &QPushButton::clicked, [=]() {
            disconnect(button2,nullptr,nullptr,nullptr);
            button2->disconnect();
            handleDrawRequest();//和棋
        });

        QMessageBox::information(this, "和棋", "对方同意和棋，游戏结束。");
        selectid=-1;//取消选择，避免bug
        reset();
    }
    else if (cmd == 10) // 接收不同意和棋的消息
    {
        connect(button2, &QPushButton::clicked, [=]() {
            disconnect(button2,nullptr,nullptr,nullptr);
            button2->disconnect();
            handleDrawRequest();//和棋
        });

        selectid=-1;//取消选择，避免bug
        QMessageBox::information(this, "和棋请求", "对方不同意和棋。");
    }

}




bool NetGame::canMove(int moveId, int killId, int row, int col)
{
    //1.确定是选择其它棋子还是走棋
    //2.是否需要使用到canMoveXXX()来做限制
    //3.罗列出所有情况，和需要的得到的结果值 ==>  然后进行中间的逻辑层判断

    //tm这段注释别删，后面可能要用到
    if(stone[moveId].red==stone[killId].red)
    {
        if(killId==-1)
        {
            switch(stone[moveId].ty)//根据选中的棋子，来选择对应的走法
            {
            case Stone::JIANG:
                return canMoveJIANG(moveId, killId, row, col);
            case Stone::SHI:
                return canMoveSHI(moveId, killId, row, col);
            case Stone::XIANG:
                return canMoveXIANG(moveId, killId, row, col);
            case Stone::MA:
                return canMoveMA(moveId, killId, row, col);
            case Stone::CHE:
                return canMoveCHE(moveId, killId, row, col);
            case Stone::PAO:
                return canMovePAO(moveId, killId, row, col);
            case Stone::BING:
                return canMoveBING(moveId, killId, row, col);

            }
        }
        selectid=killId;
        update();
        return false;
    }

    else
    {
        switch(stone[moveId].ty)//根据选中的棋子，来选择对应的走法
        {
        case Stone::JIANG:
            return canMoveJIANG(moveId, killId, row, col);
        case Stone::SHI:
            return canMoveSHI(moveId, killId, row, col);
        case Stone::XIANG:
            return canMoveXIANG(moveId, killId, row, col);
        case Stone::MA:
            return canMoveMA(moveId, killId, row, col);
        case Stone::CHE:
            return canMoveCHE(moveId, killId, row, col);
        case Stone::PAO:
            return canMovePAO(moveId, killId, row, col);
        case Stone::BING:
            return canMoveBING(moveId, killId, row, col);

        }
        return true;
    }
}

/*根据token的选边，选用不同的棋子可走的策略
    比如选了服务器选了红方（toekn==true），那么服务器端就有自己棋子的走法
    于是客户端（token==false），接收服务器发送的报文后，对自己的棋子有相应的走法。

    不然，服务器的红方走的兵往前走,在客户端方接收的报文后，客户端红兵由于被视为往后走，就走不来了。


 */

bool NetGame::canMoveJIANG(int moveId, int killId, int row, int col)
{
    if(col>5)
        return false;
    if(col<3)
        return false;
    // 检查将军是否面对面
    if (face())
    {
        // 如果两个将军之间没有棋子，那么将军的走法与车一致
        if (canMoveCHE(moveId, killId, row, col))
            return true;
    }

    /* 在服务器是选黑方的情况下*/
    if(token==false)
    {
        //将军不能超出营地
        if(stone[moveId].red)//红方
        {

            if(row > 2 || col < 3 )
                return false;
            if(col>5)
                return false;
        }
        else//黑方
        {
            if(row<7||col<3)
                return false;
            if(col>5)
                return false;
        }
    }
    else
    {
        //将军不能超出营地
        if(!stone[moveId].red)//红方
        {

            if(row > 2 || col < 3 )
                return false;
            if(col>5)
                return false;
        }
        else//黑方
        {
            if(row<7||col<3)
                return false;
            if(col>5)
                return false;
        }
    }

    //计算将军行与列分别走的步数，并加起来只能等于一
    int dr=abs(stone[moveId].row-row);
    int dc=abs(stone[moveId].col-col);

    int d=dr*10+dc;
    //为何要dr*10呢？假设填了(dr==1||dc==1),那么当dr=2，dc=1的时候，也能走
    //所以要让他们相加为d，行走了一步，算10；列走了一步，算1.
    if(d==1||d==10)
        return true;

    return false;

}


bool NetGame::canMoveSHI(int moveId, int killId, int row, int col)
{
    if(token==false)
    {
        //对于士行走范围的限制
        if(stone[moveId].red)//红方
        {

            if(row > 2 || col < 3 )
                return false;
            if(col>5)
                return false;
        }
        else//黑方
        {
            if(row<7||col<3)
                return false;
            if(col>5)
                return false;
        }
    }
    else
    {
        //对于士行走范围的限制
        if(!stone[moveId].red)//红方
        {

            if(row > 2 || col < 3 )
                return false;
            if(col>5)
                return false;
        }
        else//黑方
        {
            if(row<7||col<3)
                return false;
            if(col>5)
                return false;
        }
    }
    int dr=abs(stone[moveId].row-row);//行走的步数

    int dc=abs(stone[moveId].col-col);//列走的步数
    int d=dr*10+dc;
    if(d==11)//用将的走法原理类似
        return true;

    return false;
}



bool NetGame::canMoveXIANG(int moveId, int killId, int row, int col)
{
    if(token==false)
    {
        if(stone[moveId].red)
        {
            if(row>4)
                return false;
        }
        else
        {
            if(row<5)
                return false;
        }
    }
    else
    {
        if(!stone[moveId].red)
        {
            if(row>4)
                return false;
        }
        else
        {
            if(row<5)
                return false;
        }
    }


    int dr=abs(stone[moveId].row-row);//行走的步数
    int dc=abs(stone[moveId].col-col);//列走的步数

    if(dr==2&&dc==2)
    {
        //计算象脚的位置
        int kr=(stone[moveId].row+row)/2;
        int kc=(stone[moveId].col+col)/2;

        if(getStoneId(kr,kc)!=-1)//如果象脚有棋子，那么不允许移动
            return false;

        return true;
    }

    return false;

}

bool NetGame::canMoveBING(int moveId, int killId, int row, int col)
{
    int r = stone[moveId].row;
    int c = stone[moveId].col;

    if(token==false)
    {
        // 兵在红方
        if(stone[moveId].red)
        {
            // 兵未过河
            if(r <= 4)
            {
                // 只能向前走一步
                if(row == r + 1 && col == c)
                    return true;
            }
            else // 兵已过河
            {
                // 可以向前、左、右走一步
                if((row == r + 1 && col == c) || (row == r && col == c + 1) || (row == r && col == c - 1))
                    return true;
            }
        }
        else // 兵在黑方
        {
            // 兵未过河
            if(r >= 5)
            {
                // 只能向前走一步
                if(row == r - 1 && col == c)
                    return true;
            }
            else // 兵已过河
            {
                // 可以向前、左、右走一步
                if((row == r - 1 && col == c) || (row == r && col == c + 1) || (row == r && col == c - 1))
                    return true;
            }
        }
    }
    else
    {
        // 兵在红方
        if(!stone[moveId].red)
        {
            // 兵未过河
            if(r <= 4)
            {
                // 只能向前走一步
                if(row == r + 1 && col == c)
                    return true;
            }
            else // 兵已过河
            {
                // 可以向前、左、右走一步
                if((row == r + 1 && col == c) || (row == r && col == c + 1) || (row == r && col == c - 1))
                    return true;
            }
        }
        else // 兵在黑方
        {
            // 兵未过河
            if(r >= 5)
            {
                // 只能向前走一步
                if(row == r - 1 && col == c)
                    return true;
            }
            else // 兵已过河
            {
                // 可以向前、左、右走一步
                if((row == r - 1 && col == c) || (row == r && col == c + 1) || (row == r && col == c - 1))
                    return true;
            }
        }
    }

    return false;
}

