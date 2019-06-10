#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "waypoint.h"
#include "enemy.h"
#include "bullet.h"
//#include "audioplayer.h"
#include "plistreader.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMessageBox>
#include <QTimer>
#include <QXmlStreamReader>
#include <QtDebug>

static const int Towercost = 300;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , waves(1)
  , playerHp(4)
  , playerGold(1000)
  , first_win(false)
  , first_lose(false)
{
    ui->setupUi(this);
    //preLoadWavesInfo();
    loadTowerPositions();
    addwaypoint();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMap()));
    timer->start(30);
    QTimer::singleShot(300, this, SLOT(gameStart()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/pictures/grass_02.jpg"));

    foreach (const towerposition &towerPos, t_towerpositionlist) {
        towerPos.drawTower(&painter);
    }
    foreach (Tower *tower,t_towerlist) {
        tower->drawTower(&painter);
    }

    foreach (Enemy*enemy1, enemylist1) enemy1->draw(&painter);
    foreach (Enemy*enemy2, enemylist2) enemy2->draw(&painter);
    foreach (Enemy*enemy3, enemylist3) enemy3->draw(&painter);
    foreach (Enemy*enemy4, enemylist4) enemy4->draw(&painter);

    //foreach (const waypoint *wayPoint, waypointlist)
        //wayPoint->draw(&painter);

    foreach (const Bullet *bullet, bulletlist)
        bullet->drawBullet(&painter);

    if (first_lose || first_win)
    {
        QString text = first_lose ? "YOU LOST!!!" : "YOU WIN!!!";
        QPainter painter(this);
        painter.setPen(QPen(Qt::red));
        painter.drawText(rect(), Qt::AlignCenter, text);
        return;
    }

    drawWave(&painter);
    drawHP(&painter);
    drawPlayerGold(&painter);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {

    QPoint pressPos = event->pos();
    auto it = t_towerpositionlist.begin();
    while (it != t_towerpositionlist.end())
    {
        if (canbuyTower() && it->containTower(pressPos) && !it->canhaveTower())
        {
            //m_audioPlayer->playSound(TowerPlaceSound);
            playerGold -= Towercost;
            it->sethaveTower();

            Tower *tower = new Tower(it->centerPos(), this);
            t_towerlist.push_back(tower);
            update();
            break;
        }

        ++it;
    }
}

void MainWindow::loadTowerPositions() {
    QPoint pos[]={
        QPoint(0, 35),
        QPoint(200, 35),
        QPoint(400, 35),
        QPoint(600, 35),//

        QPoint(0, 210),
        QPoint(200, 210),
        QPoint(400, 210),
        QPoint(600, 210),//

        QPoint(0, 385),
        QPoint(200, 385),
        QPoint(400, 385),
        QPoint(600, 385),

        QPoint(0, 560),
        QPoint(200, 560),
        QPoint(400, 560),
        QPoint(600, 560)
            };
    int len	= sizeof(pos) / sizeof(pos[0]);
    for (int i = 0; i < len; ++i)
        t_towerpositionlist.push_back(pos[i]);

}

void MainWindow::removeEnemy(Enemy *enemy) {
    Q_ASSERT(enemy);
    enemylist1.removeOne(enemy);
    delete enemy;

    if (enemylist1.empty()) {
        waves++;
        if (!loadWaves()) {
            first_win=true;
        }
    }
}

QList<Enemy *> MainWindow::enemyList1() const {
    return enemylist1;
}

QList<Enemy *> MainWindow::enemyList2() const {
    return enemylist2;
}

QList<Enemy *> MainWindow::enemyList3() const {
    return enemylist3;
}

QList<Enemy *> MainWindow::enemyList4() const {
    return enemylist4;
}

void MainWindow::removeBullet(Bullet *bullet) {
    Q_ASSERT(bullet);

    bulletlist.removeOne(bullet);
    delete bullet;
}

void MainWindow::addwaypoint() {
    waypoint *wayPoint11 = new waypoint(QPoint(10,100));
    waypointlist1.push_back(wayPoint11);

    waypoint *wayPoint12 = new waypoint(QPoint(1280, 100));
    waypointlist1.push_back(wayPoint12);
    wayPoint12->setNextWayPoint(wayPoint11);

    waypoint *wayPoint21 = new waypoint(QPoint(10, 280));
    waypointlist2.push_back(wayPoint21);

    waypoint *wayPoint22 = new waypoint(QPoint(1280, 280));
    waypointlist2.push_back(wayPoint22);
    wayPoint22->setNextWayPoint(wayPoint21);

    waypoint *wayPoint31 = new waypoint(QPoint(10,460));
    waypointlist3.push_back(wayPoint31);

    waypoint *wayPoint32 = new waypoint(QPoint(1280,460));
    waypointlist3.push_back(wayPoint32);
    wayPoint32->setNextWayPoint(wayPoint31);

    waypoint *wayPoint41 = new waypoint(QPoint(10, 640));
    waypointlist4.push_back(wayPoint41);

    waypoint *wayPoint42 = new waypoint(QPoint(1280, 640));
    waypointlist4.push_back(wayPoint42);
    wayPoint42->setNextWayPoint(wayPoint41);

}

/*bool MainWindow::loadWaves()
{
    if (waves >= 18)
        return false;

    waypoint *startWayPoint1 = waypointlist1.back();
    waypoint *startWayPoint2 = waypointlist2.back();
    waypoint *startWayPoint3 = waypointlist3.back();
    waypoint *startWayPoint4 = waypointlist4.back();

    int enemyInterval[]={100,500,600,1000,3000,6000};
    int g = 0;
    double spawnTime;
    for (int i = 0; i < waves; ++i)
    {
        int g = 0;
        double spawnTime;
        if(waves <= 5) g = qrand() % 2+1;
        else if(waves >= 6 && waves <= 10) {
            g = qrand() % 3+1;
            spawnTime = qrand() % 6/10;
        }
        else if(waves >= 11 && waves <= 13) {
            g = qrand() % 4+1;
            spawnTime = qrand() % 5/10;
        }
        else if(waves >= 14 && waves <= 15) {
            g = qrand() % 5+1;
            spawnTime =qrand() % 4/10;
        }
        else if(waves >= 16 && waves <= 18) {
            g = qrand() % 6+1;
            spawnTime = qrand() % 3/10;
        }
        else {
            g = qrand() % 7+1;
            spawnTime = qrand() % 7/10;
        }
        Enemy *enemy1 = new Enemy(this, g);
        enemylist1.push_back(enemy1);
        QTimer::singleShot(spawnTime, enemy1, SLOT(doAlive()));
    }

    return true;
}*/

bool MainWindow::loadWaves(){
    if(waves>=20){
        return false;
    }//大于波数则结束游戏
   QList<waypoint *> startpoint;
   waypoint * startpoint1=waypointlist1.back();
   waypoint * startpoint2=waypointlist2.back();
   waypoint * startpoint3=waypointlist3.back();
   waypoint * startpoint4=waypointlist4.back();
   startpoint.push_back(startpoint1);
   startpoint.push_back(startpoint2);
   startpoint.push_back(startpoint3);
   startpoint.push_back(startpoint4);
   int enemyInterval[]={100,500,600,1000,3000,6000};
   for(int i=0;i<5;i++){
       int q;
        if (waves < 10) q = qrand() % ((waves+1)/2) + 1;
        else q = qrand() % ((waves+1)/4) + 1;
        Enemy *enemy=new Enemy(startpoint[q-1],this);
        enemylist1.push_back(enemy);
        QTimer::singleShot(enemyInterval[i],enemy,SLOT(do_Activate()));
        }
    return true;
}

void MainWindow::getHpDamage(int damage/* = 1*/)
{
    //audioPlayer->playSound(LifeLoseSound);
    playerHp -= damage;
    if (playerHp <= 0)
        doGameOver();
}

void MainWindow::doGameOver()
{
    if (!first_lose)
    {
        first_lose = true;
        // 此处应该切换场景到结束场景
        // 暂时以打印替代,见paintEvent处理
    }
}

void MainWindow::updateMap() {
    foreach (Enemy *enemy1, enemylist1)
        enemy1->move();
    foreach (Enemy *enemy2, enemylist2)
        enemy2->move();
    foreach (Enemy *enemy3, enemylist3)
        enemy3->move();
    foreach (Enemy *enemy4, enemylist4)
        enemy4->move();
    foreach (Tower *tower, t_towerlist)
        tower->checkEnemyInRange();

    update();
}

void MainWindow::addBullet(Bullet *bullet) {
    Q_ASSERT(bullet);

    bulletlist.push_back(bullet);
}


bool MainWindow::canbuyTower() const {
    if (playerGold >= Towercost) return true;
    else return false;
}

void MainWindow::awardGold(int gold) {
    playerGold+=gold;
    update();
}

void MainWindow::drawWave(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
        painter->drawPixmap(370,750,350,150,QPixmap(":/pictures/wave_1.png"));
    painter->drawText(QRect(520,770,200,100), QString("%1").arg(waves));
}

void MainWindow::drawHP(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
    painter->drawPixmap(0,750,350,150,QPixmap(":/pictures/blood_1.png"));
    painter->drawText(QRect(150,770,200,100), QString("%1").arg(playerHp));
    //painter->drawText(hdc,QString("HP : %1").arg(waves),-1,QRect(0,750,350,150), DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
}

void MainWindow::drawPlayerGold(QPainter *painter)
{
    painter->setPen(QPen(Qt::white));
    painter->setFont(QFont("黑体", 20, QFont::Bold, false));
        painter->drawPixmap(740,750,350,150,QPixmap(":/pictures/gold_1.png"));
    painter->drawText(QRect(890,770,200,100), QString("%1").arg(playerGold));
}
void MainWindow::gameStart()
{
    loadWaves();
}

/*void MainWindow::preLoadWavesInfo() {
    QFile file(":/pictures/Waves.plist");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "course_project", "Cannot Open TowersPosition.plist");
        return;
    }
    PListReader reader;
    reader.read(&file);

    // 获取波数信息
    wavesInfo = reader.data();

    file.close();
}*/
