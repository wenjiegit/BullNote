#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QWindow>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSystemTrayIcon>
#include <QMenu>

#include <QDebug>

#include "NoteItem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (true) {
        QString basePath = qApp->applicationDirPath();
        QIcon pix(basePath + "/res/logo.png");

        QSystemTrayIcon *icon = new QSystemTrayIcon(pix, this);
        QMenu *m = new QMenu(this);
        m->addAction(QString(QStringLiteral("显示便笺")), this, SLOT(onShow()));
        m->addAction(QString(QStringLiteral("新建便笺")), this, SLOT(onNewNote()));
        m->addAction(QString(QStringLiteral("置顶便笺")), this, SLOT(onForceTop()));
        m->addAction(QString(QStringLiteral("取消便笺置顶")), this, SLOT(onCancelTop()));
        m->addAction(QString(QStringLiteral("退出")), this, SLOT(onQuit()));

        icon->setContextMenu(m);
        icon->show();
    }

    restoreNotes();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addNote_clicked()
{
    NoteItem *item = new NoteItem();
    connect(item, SIGNAL(saveRequired()), this, SLOT(onNoteSave()));
    connect(item, SIGNAL(removeRequired()), this, SLOT(onNoteRemove()));

    item->show();

    m_items << item;

    saveNotesData();
}

void MainWindow::onNoteSave()
{
    saveNotesData();
}

void MainWindow::onNoteRemove()
{
    NoteItem *item = dynamic_cast<NoteItem*> (sender());
    if (!item) {
        return;
    }

    removeNoteItem(item);
}

void MainWindow::onShow()
{
    for (int i = 0; i < m_items.size(); ++i) {
        NoteItem *item =m_items.at(i);
        item->raise();
        item->show();
    }
}

void MainWindow::onNewNote()
{
    on_addNote_clicked();
}

void MainWindow::onForceTop()
{
    for (int i = 0; i < m_items.size(); ++i) {
        NoteItem *item =m_items.at(i);
        item->setWindowFlags(item->windowFlags() | Qt::WindowStaysOnTopHint);
        item->raise();
        item->show();
    }
}

void MainWindow::onCancelTop()
{
    for (int i = 0; i < m_items.size(); ++i) {
        NoteItem *item =m_items.at(i);
        item->setWindowFlags(item->windowFlags() & ~Qt::WindowStaysOnTopHint);
        item->raise();
        item->show();
    }
}

void MainWindow::onQuit()
{
    qApp->quit();
}

void MainWindow::saveNotesData()
{
    QJsonArray arr;
    for (int i = 0; i < m_items.size(); ++i) {
        NoteItem *item = m_items.at(i);
        arr.append(item->getData());
    }

    QJsonDocument doc(arr);
    QByteArray jsonStr = doc.toJson();

    QString outFilePath = qApp->applicationDirPath() + "/data.json";
    QFile outFile(outFilePath);
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(jsonStr);
        outFile.close();
    }
}

void MainWindow::removeNoteItem(NoteItem *item)
{
    m_items.removeAll(item);
    item->deleteLater();
    saveNotesData();
}

void MainWindow::restoreNotes()
{
    QString inFilePath = qApp->applicationDirPath() + "/data.json";
    QFile inFile(inFilePath);
    if (inFile.open(QIODevice::ReadOnly)) {
        QByteArray jsonStr = inFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr);
        QJsonArray arr = doc.array();

        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr.at(i).toObject();
            NoteItem *item = new NoteItem;
            item->setData(obj);
            item->show();

            m_items << item;

            connect(item, SIGNAL(saveRequired()), this, SLOT(onNoteSave()));
            connect(item, SIGNAL(removeRequired()), this, SLOT(onNoteRemove()));
        }
    }
}
