#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QTableWidget>
#include <QRadioButton>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addToch(float x, float y);

    void on_pushButton_click();
    void obnTochs();

    void setX();

    void setY();

protected:

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_toggled(bool checked);

    void on_pushButton_6_toggled(bool checked);

    void on_ds_xmin_valueChanged(double arg1);

    void on_ds_xmax_valueChanged(double arg1);

    void on_ds_ymin_valueChanged(double arg1);

    void on_ds_ymax_valueChanged(double arg1);

    void on_pushButton_7_clicked();

    void setRB(bool checked);
    void delAct();
private:
    QList<QPushButton*> pb;
    QList<QRadioButton*> rb;
    QList<QTableWidget*> tw;
    QList<QList<QPointF> > toch;
    QList<QList<QPointF> > tochZn;

    QList<QList<QPointF> > toch_Old;
    QList<QList<QPointF> > tochZn_Old;
    QPixmap pm_scal;
    Qt::MouseButton butPress;
    QPointF posMous,posMousCTRLNach,posNach;
    QPointF ZnNUL;

    QPointF linX0,linX1,linY0,linY1;
    float AX;
    float BX;
    float CX;

    float AY;
    float BY;
    float CY;

    float Y0;
    float X0;

    float xzY;
    float xzX;

    bool boolCtrl;

    bool boolToch,boolSter,boolPres;
    QPointF xmin_p,xmax_p,ymin_p,ymax_p;
    float xmin,xmax,ymin,ymax;
    QRect rect,scal_rect,zoom_rect;
    float scal, scal_zoom;
    float smx,smy,smx_zoom,smy_zoom;
    QPixmap pm;
    Ui::MainWindow *ui;
    bool eventFilter(QObject *target, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void paintFun(QPainter &painter);
    void paintFunZoom(QPainter &painter);
};

#endif // MAINWINDOW_H
