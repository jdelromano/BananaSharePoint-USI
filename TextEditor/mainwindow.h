#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include "QBoxLayout"
#include "helperstructures.h"
#include "authenticator.h"

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum { MaxRecentFiles = 5 };
    QMdiArea *mdiArea;

    QMenu *windowMenu;
    QAction *newAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *windowMenuSeparatorAct;

public:
    MainWindow();

    bool openFile(const QString &fileName);
    void sendFile(bool res);
    struct openFile current_open_file;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void updateRecentFileActions();
    void openRecentFile();
    void cut();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void switchLayoutDirection();

private:

    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    MdiChild *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;
    QAction* loginAct;
    Authenticator * auth;
    QDockWidget * dockWidget;
    QVBoxLayout* dockWidgetlayout;
    QAction * saveOnline;

private slots:
    void startLoginProcess();
    void onLoggedIn();
    void addTeams(QList<QPair<QString, QString>> list_id_name);
    void addChannels(QMap<QString, QString> channels, QString team_id);
    void addFiles(QList<fileInfos> list_file_infos);
    void openCurrentFile(QString fileName, QString site_id, QString item_id, QString version);
};

#endif
