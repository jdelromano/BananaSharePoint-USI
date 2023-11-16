#include <QtWidgets>

#include "mainwindow.h"
#include "mdichild.h"

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateMenus);

    createActions();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("TextEditor-Sharepoint"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }
    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    MdiChild *child = createMdiChild();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
        child->show();
    else
        child->close();
    MainWindow::prependToRecentFiles(fileName);
    return succeeded;
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    MdiChild *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}
#endif

void MainWindow::about()
{
    QMessageBox::about(
            this, tr("About %1").arg(this->windowTitle()),
            QString("<h2>") + this->windowTitle() + QString("</h2>") + "<p>"
                    + tr("This is a R&D project.") + "</p>"
                    + "<p>" + tr("The objective is to create a small desktop application "
                        "that reads and writes text files, with the capabilities "
                        "to connect directly with Teams or sharepoint.")
                    + "</p>");
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != nullptr);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    pasteAct->setEnabled(hasMdiChild);
#endif
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
#endif
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

#ifndef QT_NO_CLIPBOARD
    connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif

    return child;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    //Convenience button to start the login process
    loginAct = new QAction("do login");
    fileToolBar->addAction(loginAct);
    connect(loginAct, &QAction::triggered, this, &MainWindow::startLoginProcess);

    saveOnline = new QAction("Save online", this);
    saveOnline->setStatusTip(tr("Save the document online"));
    fileToolBar->addAction(saveOnline);
    connect(saveOnline, &QAction::triggered, [this](){
        this->auth->checkVersion(this->current_open_file.site_id,
                                 this->current_open_file.item_id,
                                 this->current_open_file.version);
    });


    fileMenu->addSeparator();


    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

    fileMenu->addSeparator();

//! [0]
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::quit);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
//! [0]

#ifndef QT_NO_CLIPBOARD
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);
#endif



    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    }

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    else
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
}

/*!
 * \brief MainWindow::startLoginProcess sets the file folder in which to store the files and the json file
 * containing informations about the files, and starts the login process.
 */
void MainWindow::startLoginProcess()
{
    QString current_path = QCoreApplication::applicationDirPath();
    QString params_path = current_path + "/../../TextEditor/params.json";
    QFile file(params_path);
    QJsonDocument document;
    file.open(QIODeviceBase::ReadOnly);
    if(file.isOpen()){
        QByteArray json_bytes = file.readAll();
        document = QJsonDocument::fromJson(json_bytes);
        file.close();
    }
    QJsonObject obj = document.object();
    QString filesPath = current_path + obj["files_path"].toString();
    QString filesJsonPath = filesPath + "/files_params.json";
    if (!QFile::exists(filesPath)){
        QDir().mkdir(filesPath);
    }
    if (!QFile::exists(filesJsonPath)){
        QFile new_json_file(filesJsonPath);
        if (new_json_file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&new_json_file);
            out << "[]";
        }
        else {
            qDebug() << "Cannot open file";
        }
    }
    qDebug() << "start login";
    this->auth = new Authenticator(this, false);
    connect(this->auth, &AbstractAuthenticator::loggedIn, this, &MainWindow::onLoggedIn);
    this->auth->startLogin();
    this->auth->files_path = filesPath;
}

/*!
 * \brief MainWindow::onLoggedIn after a successful login, it triggers the process to get the teams list
 */
void MainWindow::onLoggedIn()
{
    loginAct->setText("You are in");
    //add right widget to access teams
    this->dockWidget = new QDockWidget(tr("Dock Widget"), this);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    QWidget* multiWidget = new QWidget();
    dockWidgetlayout = new QVBoxLayout();
    dockWidgetlayout->setAlignment(Qt::AlignTop);
    multiWidget->setLayout(dockWidgetlayout);
    dockWidget->setWidget(multiWidget);
    connect(this->auth, &AbstractAuthenticator::teamsListReceived, this, &MainWindow::addTeams);
    connect(this->auth, &AbstractAuthenticator::versionChecked, this, &MainWindow::sendFile);
    this->auth->getTeamsList();
}

/*!
 * \brief MainWindow::addTeams adds the teams on the interface
 * \param list_id_name list containing all the informations about the retrieved teams
 */
void MainWindow::addTeams(QList<QPair<QString, QString>> list_id_name){
    QComboBox * selectTeam = new QComboBox(this);
    this->dockWidgetlayout->addWidget(selectTeam);
    selectTeam->setPlaceholderText(QStringLiteral("--Select Team--"));
    selectTeam->setCurrentIndex(-1);
    for (auto i = list_id_name.begin(); i != list_id_name.end(); i++){
        QString team_name = i->second;
        selectTeam->addItem(team_name);
    }
    connect(selectTeam, &QComboBox::activated, [list_id_name, this](int index){
        QString team_id = list_id_name.at(index).first;
        qDebug() << "clicked item with index: " << index << " and id: " << team_id;
        this->auth->getChannelsList(team_id);
    });
    connect(this->auth, &AbstractAuthenticator::channelsListReceived, this, &MainWindow::addChannels);
}

/*!
 * \brief MainWindow::addChannels adds the channels on the interface
 * \param channels map containing all informations about the retrieved channels
 * \param team_id the id of the team previously selected
 */
void MainWindow::addChannels(QMap<QString, QString> channels, QString team_id){
    QComboBox * selectChannel = new QComboBox(this);
    this->dockWidgetlayout->addWidget(selectChannel);
    selectChannel->setPlaceholderText(QStringLiteral("--Select Channel--"));
    selectChannel->setCurrentIndex(-1);
    QMap<int, QString> ids;
    int i=0;
    for (auto it = channels.begin(); it != channels.end(); ++it){
        QString channel_name = it.value();
        selectChannel->addItem(channel_name);
        ids.insert(i, it.key());
        ++i;
    }
    connect(selectChannel, &QComboBox::activated, [this, ids, team_id](int index){
        QString channel_id = ids.value(index);
        this->auth->getFilesFolder(team_id, channel_id);
    });
    connect(this->auth, &AbstractAuthenticator::filesListReceived, this, &MainWindow::addFiles);
}

/*!
 * \brief MainWindow::addFiles adds all the retrieved file on the interface
 * \param list_file_infos list containing all the informations about the retrieved files
 */
void MainWindow::addFiles(QList<fileInfos> list_file_infos){
    connect(this->auth, &AbstractAuthenticator::openFile, this, &MainWindow::openCurrentFile);
    for (auto it = list_file_infos.begin(); it != list_file_infos.end(); ++it){
        QPushButton * button = new QPushButton();
        QString fname = it->file_name;
        QString sid = it->site_id;
        QString tid = it->item_id;
        button->setText(it->file_name);
        connect(button, &QPushButton::clicked, [this, sid, tid, fname](){
            this->auth->getFileContent(sid, tid, fname, true);
        });
        this->dockWidgetlayout->addWidget(button);
    }
}

/*!
 * \brief MainWindow::openCurrentFile triggers the opening of a certain file and saves important infos about it
 * \param fileName the name of the file to open
 * \param site_id site id of the file
 * \param item_id item id (file id)
 * \param version version of the file
 */
void MainWindow::openCurrentFile(QString fileName, QString site_id, QString item_id, QString version){
    this->current_open_file = {fileName, site_id, item_id, version};
    QString file_path = "../../../files/" + fileName;
    this->openFile(file_path);
}

/*!
 * \brief MainWindow::sendFile triggers the update of the file online, if the local version is the same as the online version
 * \param res boolean result from function AbstractAuthenticator::checkVersion
 */
void MainWindow::sendFile(bool res){
    if (res){
        QString new_text = activeMdiChild()->getText();
        this->auth->updateFileContent(new_text.toUtf8(), &(this->current_open_file));
    }
    else{
        //display message: file was already modified
        QMessageBox::warning(
                this,
                tr("Text Editor"),
                tr("This file has been modified"));
    }
}

