#include "NoteItem.h"
#include "ui_NoteItem.h"

#include <QPainter>
#include <QCoreApplication>
#include <QToolButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

#include "NcFramelessHelper.h"
#include "utils.h"

NoteItem::NoteItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteItem)
{
    ui->setupUi(this);

    resize(QSize(240, 280));

    // default bg color
    m_bgColor = "#f0d65c";

    ui->titleWidget->setVisible(false);
    ui->closeBtn->setPixmap(QPixmap(qApp->applicationDirPath() + "/res/close.png"));

    NcFramelessHelper *helper = new NcFramelessHelper(this);
    helper->setWidgetMovable(true);
    helper->activateOn(this);

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(ui->closeBtn, SIGNAL(clicked(bool)), this, SLOT(onRemoveClicked()));

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    setWindowTitle("");

    initColorBtn();
}

NoteItem::~NoteItem()
{
    delete ui;
}

QJsonObject NoteItem::getData()
{
    QJsonObject obj;
    QRect r = geometry();

    obj.insert("x", r.x());
    obj.insert("y", r.y());
    obj.insert("width", r.width());
    obj.insert("height", r.height());

    QJsonObject clrObj;
    clrObj.insert("r", m_bgColor.red());
    clrObj.insert("g", m_bgColor.green());
    clrObj.insert("b", m_bgColor.blue());
    clrObj.insert("a", m_bgColor.alpha());

    obj.insert("bg_color", clrObj);
    obj.insert("text", ui->textEdit->toHtml());

    return obj;
}

void NoteItem::setData(const QJsonObject &obj)
{
    int x = obj.value("x").toInt();
    int y = obj.value("y").toInt();
    int width = obj.value("width").toInt();
    int height = obj.value("height").toInt();

    QRect r;
    r.setX(x);
    r.setY(y);
    r.setWidth(width);
    r.setHeight(height);

    QRect desktopGeo = QRect(QPoint(0, 0), QGuiApplication::primaryScreen()->availableSize());

    static int sx = 0;
    static int sy = 0;
    if (!desktopGeo.intersects(r)) {
        r.moveTo(sx, sy);
        sx += 10;
        sy += 10;
    }
    setGeometry(r);

    QString text = obj.value("text").toString();
    ui->textEdit->setHtml(text);

    // get color
    QJsonObject clrObj = obj.value("bg_color").toObject();
    if (!clrObj.isEmpty()) {
        int red     = clrObj.value("r").toInt();
        int green   = clrObj.value("g").toInt();
        int blue    = clrObj.value("b").toInt();
        int alpha   = clrObj.value("a").toInt();
        m_bgColor = QColor(red, green, blue, alpha);
    }

    update();
}

void NoteItem::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);

    p.setPen(QColor("#c0c1c1"));
    p.setBrush(QColor(m_bgColor));

    QRect r(0, 0, width()-1, height()-1);
    p.drawRect(r);
}

bool NoteItem::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        emit saveRequired();
    } else if (event->type() == QEvent::WindowActivate) {
        ui->titleWidget->setVisible(true);
    } else if (event->type() == QEvent::WindowDeactivate) {
        ui->titleWidget->setVisible(false);
    }

    return QWidget::event(event);
}

void NoteItem::onTextChanged()
{
    emit saveRequired();
}

void NoteItem::onRemoveClicked()
{
    emit removeRequired();
}

void NoteItem::onColorChanged()
{
    QObject *sd = sender();
    if (!sd) {
        return;
    }

    QColor clr = sd->property("color").value<QColor>();
    m_bgColor = clr;

    update();

    emit saveRequired();
}

void NoteItem::initColorBtn()
{
    ui->horizontalLayout->removeItem(ui->horizontalSpacer);
    ui->horizontalLayout->removeWidget(ui->closeBtn);

    QString path = qApp->applicationDirPath() + "/bg_color.json";
    QByteArray data = readFile(path);
    QJsonArray arr = QJsonDocument::fromJson(data).array();
    for (int i = 0; i < arr.size(); ++i) {
        QJsonObject clr = arr.at(i).toObject();
        int r = clr.value("r").toInt();
        int g = clr.value("g").toInt();
        int b = clr.value("b").toInt();
        int a = clr.value("a").toInt();

        QColor c(r, g, b, a);
        addColorBtn(c);
    }

    ui->horizontalLayout->addItem(ui->horizontalSpacer);
    ui->horizontalLayout->addWidget(ui->closeBtn);
}

void NoteItem::addColorBtn(const QColor &clr)
{
    QImage img(16*4, 16, QImage::Format_RGBA8888);
    img.fill(clr);

    ToolButton *btn = new ToolButton;
    btn->setPixmap(QPixmap::fromImage(img));
    btn->setProperty("color", clr);

    connect(btn, &ToolButton::clicked, this, &NoteItem::onColorChanged);

    ui->horizontalLayout->addWidget(btn);
}
