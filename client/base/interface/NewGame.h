#ifndef NEWGAME_H
#define NEWGAME_H

#include <QDialog>

namespace Ui {
class NewGame;
}

class NewGame : public QDialog
{
    Q_OBJECT

public:
    explicit NewGame(const QString &skinPath, const QStringList &forcedSkin, QWidget *parent = 0);
    ~NewGame();
    bool haveTheInformation();
    QString pseudo();
    QString skin();
    bool haveSkin();
    void updateSkin();
private slots:
    void on_ok_clicked();
    void on_pseudo_textChanged(const QString &arg1);
    void on_pseudo_returnPressed();
    void on_nextSkin_clicked();
    void on_previousSkin_clicked();
private:
    QStringList forcedSkin;
    Ui::NewGame *ui;
    bool ok;
    bool skinLoaded;
    QStringList skinList;
    int currentSkin;
    QString skinPath;
    bool okCanBeEnabled();
};

#endif // NEWGAME_H