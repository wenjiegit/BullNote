#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

namespace Ui {
class MainWindow;
}

class NoteItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_addNote_clicked();

    void onNoteSave();
    void onNoteRemove();

    void onShow();
    void onNewNote();
    void onForceTop();
    void onCancelTop();
    void onQuit();

private:
    void saveNotesData();
    void removeNoteItem(NoteItem *item);
    void restoreNotes();

private:
    Ui::MainWindow *ui;
    QList<NoteItem*> m_items;
};

#endif // MAINWINDOW_H
