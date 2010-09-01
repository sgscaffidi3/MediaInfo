#include "columneditsheet.h"
#include "sheet.h"
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QToolButton>
#include "translate.h"

#include <ZenLib/Ztring.h>
using namespace ZenLib;
#define wstring2QString(_DATA) \
    QString::fromUtf8(Ztring(_DATA).To_UTF8().c_str())
#define QString2wstring(_DATA) \
    Ztring().From_UTF8(_DATA.toUtf8())

ColumnEditSheet::ColumnEditSheet(column c, int pos, int nb, Core* C, QWidget *parent) :
    QHBoxLayout(parent)
{
    this->pos = pos;
    this->C = C;
    this->col = c;

    QLineEdit* lineedit = new QLineEdit(c.name);
    this->addWidget(lineedit);
    spinbox = new QSpinBox();
    spinbox->setMinimum(0);
    spinbox->setMaximum(500);
    spinbox->setValue(c.width);
    this->addWidget(spinbox);
    stream = new QComboBox();
    for (int StreamKind=(int)Stream_General; StreamKind<(int)Stream_Max; StreamKind++)
    {
        stream->addItem(wstring2QString(C->StreamName((stream_t)StreamKind)),StreamKind);
    }
    stream->setCurrentIndex(c.stream);
    this->addWidget(stream);
    combobox = new QComboBox();
    combobox->addItem(c.key,c.key);
    this->addWidget(combobox);
    up = new QToolButton();
    up->setArrowType(Qt::UpArrow);
    //up->setSizePolicy(QSizePolicy::Minimum);
    this->addWidget(up);
    down = new QToolButton();
    down->setArrowType(Qt::DownArrow);
    down->setBaseSize(480,480);
    down->setMaximumHeight(spinbox->height());
    //qDebug(QString::number(spinbox->height()).toStdString().c_str());
    this->addWidget(down);
    minus = new QToolButton();
    minus->setText("-");
    this->addWidget(minus);

    connect(lineedit,SIGNAL(textChanged(QString)),SLOT(contentChanged()));
    connect(spinbox,SIGNAL(valueChanged(int)),SLOT(contentChanged()));
    connect(up,SIGNAL(clicked()),SLOT(upButton()));
    connect(down,SIGNAL(clicked()),SLOT(downButton()));
    connect(minus,SIGNAL(clicked()),SLOT(minusButton()));
    connect(stream,SIGNAL(currentIndexChanged(int)),SLOT(fillCombobox()));

    fillCombobox();

    posChanged(nb);
}

ColumnEditSheet::~ColumnEditSheet() {
    while (QLayoutItem* i = takeAt(0)) {
        delete i->widget();
    }
}

void ColumnEditSheet::fillCombobox() {
    combobox->clear();
    QString s = wstring2QString(C->Parameters());
    s.replace("\r\n","\n");
    s.remove(0,s.indexOf(stream->currentText()+"\n"));
    s.truncate((s.indexOf("\n\n")==-1?s.size():s.indexOf("\n\n")));
    QStringList sl = s.split("\n");
    sl.removeAt(0);
    sl.replaceInStrings(QRegExp(";(.*)"),"");
    for (int i=0; i<sl.size(); ++i)
        combobox->addItem(sl.at(i),sl.at(i));
    if(stream->itemData(stream->currentIndex()).toInt()==col.stream)
        combobox->setCurrentIndex(sl.indexOf(col.key));
}

void ColumnEditSheet::contentChanged() {
    emit somethingChanged();
}

void ColumnEditSheet::upButton() {
    emit moveMeUp(pos);
}

void ColumnEditSheet::downButton() {
    emit moveMeDown(pos);
}

void ColumnEditSheet::minusButton() {
    emit removeMe(pos);
}

void ColumnEditSheet::posSwitched(int p1, int p2, int nb) {
    if(pos==p1)
        pos=p2;
    else if(pos==p2)
        pos=p1;
    posChanged(nb);
}

void ColumnEditSheet::posRemoved(int p, int nb) {
    if(pos>p)
        pos--;
    posChanged(nb);
}

void ColumnEditSheet::posChanged(int nb) {
    up->setEnabled(pos!=0);
    down->setEnabled(pos!=(nb-1));
    minus->setEnabled(nb>1);
    emit somethingChanged();
}