#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QDebug"
#include "QPainter"
#include "QBrush"
#include "QPaintEvent"
#include "QFileDialog"
#include "QDialog"
#include "QDoubleSpinBox"
#include "QClipboard"

#include <cmath>

using namespace std;

bool ctrlz=false;
int sch=-1;
int ActLin=-1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    scal_zoom=5.0;
    boolCtrl=false;
    ui->setupUi(this);

    butPress = Qt::LeftButton;
    ui->labelPaint->installEventFilter(this);
    ui->labelZoom->installEventFilter(this);
    ui->labelPaint->setMouseTracking(true);

    boolSter=boolToch=boolPres=false;
    ui->splitter->setStretchFactor(0,17);
    ui->splitter->setStretchFactor(1,10);

    on_pushButton_7_clicked();
}

void MainWindow::setRB(bool checked)
{
    foreach (QRadioButton* rb_, rb) {
        rb_->setChecked(false);
    }
    QRadioButton* rb_=(QRadioButton*)QObject::sender();
    rb_->setChecked(true);
    int n=rb.indexOf(rb_);
    ActLin=n;
}

void MainWindow::paintFun(QPainter& painter)
{
    if(scal<1)
        painter.drawPixmap(scal_rect,pm_scal);
    else
        painter.drawPixmap(scal_rect,pm);

    painter.setPen(QPen(Qt::green, 1, Qt::SolidLine));
    painter.drawLine(linX0.x()*scal+smx,linX0.y()*scal+smy,
                    linX1.x()*scal+smx,linX1.y()*scal+smy);

    painter.setPen(QPen(Qt::green, 5, Qt::SolidLine));
    painter.drawPoint(xmin_p.x()*scal+smx,xmin_p.y()*scal+smy);
    painter.drawPoint(xmax_p.x()*scal+smx,xmax_p.y()*scal+smy);

    painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
    painter.drawLine(linY0.x()*scal+smx,linY0.y()*scal+smy,
                     linY1.x()*scal+smx,linY1.y()*scal+smy);

    painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine));
    painter.drawPoint(ymin_p.x()*scal+smx,ymin_p.y()*scal+smy);
    painter.drawPoint(ymax_p.x()*scal+smx,ymax_p.y()*scal+smy);

    painter.setPen(QPen(Qt::red, 5, Qt::SolidLine));
    for(int j=0;j<toch.size();j++){
        for(int i=0;i<toch[j].size();i++){
            QPointF p=QPointF(toch[j].at(i).x()*scal+smx,toch[j].at(i).y()*scal+smy);
            painter.drawPoint(p);
        }
    }



    if(boolSter){
        painter.setBrush(QBrush(Qt::white, Qt::NoBrush));
        painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
        painter.drawEllipse(posMous,20,20);
    }
}

void MainWindow::addToch(float x,float y)
{
    float CXX=-AX*x-BX*y;
    float CYY=-AY*x-BY*y;

    float dCX=CXX-CX;
    float dCY=CYY-CY;

    float Y1=(-CY*AX+AY*CXX)/(BY*AX-AY*BX);
    float X1=(-CXX-BX*Y1)/AX;
    if(AX==0.0){
        X1=(-CY-BY*Y1)/AY;
    }

    float Y2=(-CX+AX*CYY/AY)/(BX-AX*BY/AY);
    float X2=(-CYY-BY*Y2)/AY;

    float RezX,RezY;
    if(dCX>0){
        RezY=xzY*sqrt((Y1-Y0)*(Y1-Y0)+(X1-X0)*(X1-X0));
    }
    else {
        RezY=-xzY*sqrt((Y1-Y0)*(Y1-Y0)+(X1-X0)*(X1-X0));
    }
    RezY=RezY+ZnNUL.y();

    if(dCY>0){
        RezX=-xzX*sqrt((Y2-Y0)*(Y2-Y0)+(X2-X0)*(X2-X0));
    }
    else {
        RezX=xzX*sqrt((Y2-Y0)*(Y2-Y0)+(X2-X0)*(X2-X0));
    }
    RezX=RezX+ZnNUL.x();

    tochZn[ActLin]<<QPointF(RezX,RezY);

    tw[ActLin]->insertRow(tw[ActLin]->rowCount());
    QTableWidgetItem *newItemX = new QTableWidgetItem(tr("%1").arg(RezX));
    QTableWidgetItem *newItemY = new QTableWidgetItem(tr("%1").arg(RezY));
    tw[ActLin]->setItem(tw[ActLin]->rowCount()-1,0, newItemX);
    tw[ActLin]->setItem(tw[ActLin]->rowCount()-1,1, newItemY);
}

bool zzzbool=false;

void MainWindow::setX()
{
    AX=xmin_p.y()-xmax_p.y();
    if(AX==0.0)
        AX=0.0;
    BX=xmax_p.x()-xmin_p.x();
    if(BX==0.0)
        BX=0.0;
    CX=xmin_p.x()*xmax_p.y()-xmax_p.x()*xmin_p.y();

    linX0=QPointF(0,-CX/BX);
    linX1=QPointF(rect.width(),(-CX-AX*rect.width())/BX);
}

void MainWindow::setY()
{
    AY=ymin_p.y()-ymax_p.y();
    if(AY==0.0)
        AY=0.0;
    BY=ymax_p.x()-ymin_p.x();
    CY=ymin_p.x()*ymax_p.y()-ymax_p.x()*ymin_p.y();

    linY0=QPointF(-CY/AY,0);
    linY1=QPointF((-CY-BY*rect.height())/AY,rect.height());

    Y0=(-CY*AX+AY*CX)/(BY*AX-AY*BX);
    X0=(-CX-BX*Y0)/AX;
    if(AX==0.0){
        X0=(-CY-BY*Y0)/AY;
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->labelPaint) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent* re=static_cast<QResizeEvent*>(event);

        }
        if (event->type() == QEvent::MouseMove ) {
            QMouseEvent* me=static_cast<QMouseEvent*>(event);
            QPointF dpm=QPointF(me->pos().x()-posMous.x(),me->pos().y()-posMous.y());
            posMous=QPointF(me->pos().x(),me->pos().y());
            if(boolCtrl){
                if(!zzzbool){
                    smx_zoom=smx_zoom-dpm.x();
                    smy_zoom=smy_zoom-dpm.y();

                    zoom_rect.moveTo(smx_zoom,smy_zoom);

                    QPointF dpmF=QPointF(posMousCTRLNach.x()-posMous.x(),posMousCTRLNach.y()-posMous.y());
                    if(abs(dpmF.x())>20 || abs(dpmF.y())>20){
                        zzzbool=true;
                        posMousCTRLNach=posMous;
                        QCursor::setPos(posNach.x(),posNach.y());
                    }
                }
                else
                    zzzbool=false;
            }
            else{
                if(boolPres){
                    if(boolSter){
                        for(int i=toch[ActLin].size()-1;i>=0;i--){
                            QPointF p=QPointF(toch[ActLin].at(i).x()*scal+smx,toch[ActLin].at(i).y()*scal+smy);
                            float f=(p.x()-posMous.x())*(p.x()-posMous.x())+(p.y()-posMous.y())*(p.y()-posMous.y());
                            if(f<400){
                                toch[ActLin].removeAt(i);
                                tochZn[ActLin].removeAt(i);
                                tw[ActLin]->removeRow(i);
                            }
                        }
                    }
                    else{
                        if(!boolToch || butPress==Qt::RightButton){
                            smx=smx+dpm.x();
                            smy=smy+dpm.y();
                            scal_rect.moveTo(smx,smy);
                        }
                    }
                }
                {
                    float dx=me->pos().x();
                    float dy=me->pos().y();

                    dx=dx-smx;
                    dy=dy-smy;

                    float ds=scal_zoom/scal;

                    smx_zoom=-dx*ds+100;
                    smy_zoom=-dy*ds+100;

                    zoom_rect.moveTo(smx_zoom,smy_zoom);
                }
            }


            ui->labelPaint->repaint();
            ui->labelZoom->repaint();
        }
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* me=static_cast<QMouseEvent*>(event);
            if ( me->button() == Qt::RightButton ){
                QPointF p1=QPointF(me->pos().x(),me->pos().y());
                for(int i=toch[ActLin].size()-1;i>=0;i--){
                    QPointF p=QPointF(toch[ActLin].at(i).x()*scal+smx,toch[ActLin].at(i).y()*scal+smy);
                    float f=(p.x()-p1.x())*(p.x()-p1.x())+(p.y()-p1.y())*(p.y()-p1.y());
                    if(f<200){
                        toch[ActLin].removeAt(i);
                        tochZn[ActLin].removeAt(i);
                        tw[ActLin]->removeRow(i);
                    }
                }
                ui->labelPaint->repaint();
            }
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            boolPres=false;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            boolPres=true;
            QMouseEvent* me=static_cast<QMouseEvent*>(event);
            butPress=me->button();
            if ( butPress == Qt::LeftButton ){

                float x=(100-smx_zoom)/scal_zoom;
                float y=(100-smy_zoom)/scal_zoom;
                QPointF p1=QPointF(x,y);
                if(sch==0){
                    xmin_p=p1;
                    setX();
                }
                if(sch==1){
                    xmax_p=p1;
                    setX();
                }
                if(sch==2){
                    ymin_p=p1;
                    setY();
                }
                if(sch==3){
                    ymax_p=p1;
                    setY();
                }
                if(boolToch && sch==-1){
                    toch[ActLin]<<p1;
                    addToch(x,y);
                }

                ui->labelPaint->repaint();
            }
        }

        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke=static_cast<QKeyEvent*>(event);


        }

        if (event->type() == QEvent::Wheel) {
            QWheelEvent* we=static_cast<QWheelEvent*>(event);
            float scal_old=scal;
            if(we->delta()>0)
                scal=scal*0.8;
            else
                if(we->delta()<0)
                    scal=scal*1.2;

            float dx=we->posF().x();
            float dy=we->posF().y();

            dx=dx-smx;
            dy=dy-smy;

            float ds=scal/scal_old;

            smx=smx-dx*(ds-1);
            smy=smy-dy*(ds-1);
            scal_rect.setWidth(rect.width()*scal);
            scal_rect.setHeight(rect.height()*scal);
            scal_rect.moveTo(smx,smy);

            if(scal<1){
                pm_scal=pm.scaled(scal_rect.width(),scal_rect.height(), Qt::IgnoreAspectRatio,
                                  Qt::SmoothTransformation);
            }

            ui->labelPaint->repaint();
        }
        if (event->type() == QEvent::Paint) {
            QPaintEvent* pe=static_cast<QPaintEvent*>(event);
            QPainter painter(ui->labelPaint);
//            painter.setRenderHint(QPainter::Antialiasing);
//            painter.setRenderHint(QPainter::SmoothPixmapTransform);
//            painter.setRenderHint(QPainter::HighQualityAntialiasing);
//            painter.setRenderHint(QPainter::TextAntialiasing);
            painter.setPen(QPen(Qt::black, 1, Qt::NoPen));
            painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
            painter.drawRect(pe->rect());
            paintFun(painter);
        }
    }
    if (target == ui->labelZoom) {
        if (event->type() == QEvent::Paint) {
            QPaintEvent* pe=static_cast<QPaintEvent*>(event);
            QPainter painter(ui->labelZoom);
            painter.setPen(QPen(Qt::black, 1, Qt::NoPen));
            painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
            painter.drawRect(pe->rect());
            paintFunZoom(painter);
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::paintFunZoom(QPainter& painter)
{
    painter.drawPixmap(zoom_rect,pm);

    painter.setPen(QPen(Qt::green, 1, Qt::SolidLine));
    painter.drawLine(linX0.x()*scal_zoom+smx_zoom,linX0.y()*scal_zoom+smy_zoom,
                    linX1.x()*scal_zoom+smx_zoom,linX1.y()*scal_zoom+smy_zoom);

    painter.setPen(QPen(Qt::green, 5, Qt::SolidLine));
    painter.drawPoint(xmin_p.x()*scal_zoom+smx_zoom,xmin_p.y()*scal_zoom+smy_zoom);
    painter.drawPoint(xmax_p.x()*scal_zoom+smx_zoom,xmax_p.y()*scal_zoom+smy_zoom);

    painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine));
    painter.drawLine(linY0.x()*scal_zoom+smx_zoom,linY0.y()*scal_zoom+smy_zoom,
                     linY1.x()*scal_zoom+smx_zoom,linY1.y()*scal_zoom+smy_zoom);

    painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine));
    painter.drawPoint(ymin_p.x()*scal_zoom+smx_zoom,ymin_p.y()*scal_zoom+smy_zoom);
    painter.drawPoint(ymax_p.x()*scal_zoom+smx_zoom,ymax_p.y()*scal_zoom+smy_zoom);

    painter.setPen(QPen(Qt::red, 5, Qt::SolidLine));
    painter.drawPoint(100,100);

    for(int j=0;j<toch.size();j++){
        for(int i=0;i<toch[j].size();i++){
            QPointF p=QPointF(toch[j].at(i).x()*scal_zoom+smx_zoom,toch[j].at(i).y()*scal_zoom+smy_zoom);
            painter.drawPoint(p);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    QString files = QFileDialog::getOpenFileName(0, "Open Dialog", "", "Image (*.*)");
    if (files!="")
    {
        setWindowTitle("files");
        tw[ActLin]->setRowCount(0);
        toch[ActLin].clear();
        tochZn[ActLin].clear();
        boolSter=boolToch=false;
        ui->pushButton_5->setChecked(false);
        xmin_p=xmax_p=ymin_p=ymax_p=QPointF(0,0);
        smx=smy=0.0;
        xmin=ymin=0.0;
        xmax=ymax=1.0;

        pm.load(files);
        float kpm=(float)pm.rect().height()/pm.rect().width();
        float kle=(float)ui->labelPaint->rect().height()/ui->labelPaint->rect().width();
        rect=pm.rect();
        if(kpm>kle){
            scal=(float)ui->labelPaint->rect().height()/pm.rect().height();
        }
        else{
            scal=(float)ui->labelPaint->rect().width()/pm.rect().width();
        }

        scal_rect.setWidth(rect.width()*scal);
        scal_rect.setHeight(rect.height()*scal);

        zoom_rect.setWidth(rect.width()*scal_zoom);
        zoom_rect.setHeight(rect.height()*scal_zoom);

        Qt::TransformationMode mode;
        if(scal<1){
            mode=Qt::SmoothTransformation;
            pm_scal=pm.scaled(scal_rect.width(),scal_rect.height(), Qt::IgnoreAspectRatio,
                              mode);
        }

        ui->labelPaint->repaint();
    }
}

QDialog*d;
QDoubleSpinBox*sb;

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Control:
    {
        boolCtrl=false;
        if(boolToch || sch>-1)
           ui->labelPaint->setCursor(Qt::CrossCursor);
        else
            ui->labelPaint->setCursor(Qt::ArrowCursor);
    }
        break;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Control:
    {
        posMousCTRLNach=posMous;
        posNach=QCursor::pos();
        boolCtrl=true;
        ui->labelPaint->setCursor(Qt::BlankCursor);
    }
        break;
    case Qt::Key_Escape:
    {
        obnTochs();
    }
        break;
    case Qt::Key_Z:
        if(event->modifiers() & Qt::CTRL) {
            if(ctrlz) {
                toch=toch_Old;
                tochZn[ActLin].clear();
                tw[ActLin]->setRowCount(0);
                for(int i=0;i<toch[ActLin].size();i++){
                    QPointF p=toch[ActLin].at(i);
                    float x=p.x();
                    float y=p.y();
                    addToch(x,y);
                }
            }
        }
        break;
    }


    QMainWindow::keyPressEvent(event);
}

void MainWindow::on_pushButton_clicked()
{
    sch=-1;
    on_pushButton_click();
}

void MainWindow::obnTochs()
{
    ui->labelPaint->setCursor(Qt::ArrowCursor);
    sch=-1;
    xzX=(xmax-xmin)/sqrt((xmax_p.x()-xmin_p.x())*(xmax_p.x()-xmin_p.x())+
                  (xmax_p.y()-xmin_p.y())*(xmax_p.y()-xmin_p.y()));
    xzY=(ymax-ymin)/sqrt((ymax_p.x()-ymin_p.x())*(ymax_p.x()-ymin_p.x())+
                  (ymax_p.y()-ymin_p.y())*(ymax_p.y()-ymin_p.y()));

    float zn0x;
    if(X0<xmin_p.x()){
        zn0x=xmin-xzX*sqrt((X0-xmin_p.x())*(X0-xmin_p.x())+
                                  (Y0-xmin_p.y())*(Y0-xmin_p.y()));
    }
    else{
        zn0x=xmin+xzX*sqrt((X0-xmin_p.x())*(X0-xmin_p.x())+
                                  (Y0-xmin_p.y())*(Y0-xmin_p.y()));
    }

    float zn0y;
    if(Y0>ymin_p.y()){
        zn0y=ymin-xzY*sqrt((X0-ymin_p.x())*(X0-ymin_p.x())+
                                  (Y0-ymin_p.y())*(Y0-ymin_p.y()));
    }
    else{
        zn0y=ymin+xzY*sqrt((X0-ymin_p.x())*(X0-ymin_p.x())+
                                  (Y0-ymin_p.y())*(Y0-ymin_p.y()));
    }

    ZnNUL=QPointF(zn0x,zn0y);

    tochZn[ActLin].clear();
    tw[ActLin]->setRowCount(0);
    for(int i=0;i<toch[ActLin].size();i++){
        QPointF p=toch[ActLin].at(i);
        float x=p.x();
        float y=p.y();
        addToch(x,y);
    }
    ui->labelPaint->repaint();
}

void MainWindow::on_pushButton_click()
{
    sch++;
    if(sch==0)
        ui->labelPaint->setCursor(Qt::CrossCursor);
    d=new QDialog(this,Qt::CustomizeWindowHint|Qt::WindowTitleHint);
    connect(d,&QDialog::finished,
            [&]()
    {
        obnTochs();
    });

    d->installEventFilter(this);
    QVBoxLayout* LayoutNastr=new QVBoxLayout(d);
    d->setLayout(LayoutNastr);
    QLabel* l=new QLabel();
    LayoutNastr->addWidget(l);
    sb = new QDoubleSpinBox();
    sb->setMaximum(100000000);
    sb->setMinimum(-100000000);
    if(sch==0){
        d->setWindowTitle("Установите X_MIN");
        l->setText("Установите X_MIN");
        sb->setValue(xmin);
    }
    if(sch==1){
        d->setWindowTitle("Установите X_MAX");
        l->setText("Установите X_MAX");
        sb->setValue(xmax);
    }
    if(sch==2){
        d->setWindowTitle("Установите Y_MIN");
        l->setText("Установите Y_MIN");
        sb->setValue(ymin);
    }
    if(sch==3){
        d->setWindowTitle("Установите Y_MAX");
        l->setText("Установите Y_MAX");
        sb->setValue(ymax);
    }
    connect(sb,&QDoubleSpinBox::editingFinished,
            [&]()
    {
        if(sch==0){
            xmin=sb->value();
            ui->ds_xmin->setValue(xmin);
        }
        if(sch==1){
            xmax=sb->value();
            ui->ds_xmax->setValue(xmax);
        }
        if(sch==2){
            ymin=sb->value();
            ui->ds_ymin->setValue(ymin);
        }
        if(sch==3){
            ymax=sb->value();
            ui->ds_ymax->setValue(ymax);
        }
    });

    LayoutNastr->addWidget(sb);

    QPushButton* pcmdOk = new QPushButton("&Ok",this);
    LayoutNastr->addWidget(pcmdOk);

    connect(pcmdOk,&QPushButton::clicked,
            [&]()
    {
        d->deleteLater();
        if(sch<3){
            on_pushButton_click();
        }
        else {
            obnTochs();
        }
    });
    d->show();

    setFocus();
}

void MainWindow::on_pushButton_3_clicked()
{
    QString s;
    QTextStream out(&s);

    for(int j=0;j<tochZn.size();j++){
        out<<"X"<<'\t'<<"Y"<<'\n';
        for(int i=0;i<tochZn[j].size();i++){
            QPointF p=tochZn[j].at(i);
            out<<QString().number(p.x())<<'\t'<<QString().number(p.y())<<'\n';
        }
        out<<'\n';
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(s);
}

void MainWindow::on_pushButton_4_clicked()
{
    ctrlz=true;
    tochZn_Old=tochZn;
    tochZn[ActLin].clear();
    toch_Old=toch;
    toch[ActLin].clear();
    tw[ActLin]->setRowCount(0);
    ui->labelPaint->repaint();
}

void MainWindow::on_pushButton_5_toggled(bool checked)
{
    boolToch=checked;
    if(checked){
        ui->labelPaint->setCursor(Qt::CrossCursor);
        boolSter=false;
        ui->pushButton_6->blockSignals(true);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_6->blockSignals(false);
    }
    else
        ui->labelPaint->setCursor(Qt::ArrowCursor);
    ui->labelPaint->repaint();
}

void MainWindow::on_pushButton_6_toggled(bool checked)
{
    boolSter=checked;
    if(checked){
        ui->labelPaint->setCursor(Qt::BlankCursor);
        boolToch=false;
        ui->pushButton_5->blockSignals(true);
        ui->pushButton_5->setChecked(false);
        ui->pushButton_5->blockSignals(false);
    }
    else
        ui->labelPaint->setCursor(Qt::ArrowCursor);
    ui->labelPaint->repaint();
}

void MainWindow::on_ds_xmin_valueChanged(double arg1)
{
    xmin=arg1;
    obnTochs();
}

void MainWindow::on_ds_xmax_valueChanged(double arg1)
{
    xmax=arg1;
    obnTochs();
}

void MainWindow::on_ds_ymin_valueChanged(double arg1)
{
    ymin=arg1;
    obnTochs();
}

void MainWindow::on_ds_ymax_valueChanged(double arg1)
{
    ymax=arg1;
    obnTochs();
}

void MainWindow::on_pushButton_7_clicked()
{
    ActLin++;
    foreach (QRadioButton* rb_, rb) {
        rb_->setChecked(false);
    }
    rb<<new QRadioButton();
    rb[ActLin]->setChecked(true);
    connect(rb[ActLin],&QRadioButton::clicked,this,&MainWindow::setRB);
    ui->verticalLayout->addWidget(rb[ActLin]);
    pb<<new QPushButton("Удалить");
    connect(pb[ActLin],&QPushButton::clicked,this,&MainWindow::delAct);
    ui->verticalLayout->addWidget(pb[ActLin]);
    tw<<new QTableWidget();
    tw[ActLin]->setColumnCount(2);
    QStringList sl;
    sl<<"X"<<"Y";
    tw[ActLin]->setHorizontalHeaderLabels(sl);
    ui->verticalLayout->addWidget(tw[ActLin]);
    toch<<QList<QPointF>();
    tochZn<<QList<QPointF>();

    toch_Old<<QList<QPointF>();
    tochZn_Old<<QList<QPointF>();
}

void MainWindow::delAct()
{
    if(pb.size()>1){
        QPushButton* pb_=(QPushButton*)QObject::sender();
        int n=pb.indexOf(pb_);
        delete pb.takeAt(n);
        delete rb.takeAt(n);
        delete tw.takeAt(n);
        toch.takeAt(n);
        tochZn.takeAt(n);
        toch_Old.takeAt(n);
        tochZn_Old.takeAt(n);

        if(pb.size()-1<ActLin){
            ActLin--;
            foreach (QRadioButton* rb_, rb) {
                rb_->setChecked(false);
            }
            rb[ActLin]->setChecked(true);
        }
        else{
            if(n==ActLin){
                rb.at(n)->setChecked(true);
            }
        }
        ui->labelPaint->repaint();
    }
}
