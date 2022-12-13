// Description: Header interface for your application's updater. Implementation could differ by platform.

#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>

class QAction;

#ifdef __OBJC__
@class AppUpdaterDelegate;
#endif

class Updater : public QObject
{
    Q_OBJECT
    
public:
    // Caller example from a QMainWindow subclass:
    /*
     QAction *updaterAction = new QAction(tr("&Check for Updates…"), this);
     updaterAction->setMenuRole(QAction::ApplicationSpecificRole);
     
     QMenu *updaterMenu = menuBar()->addMenu("&Updater");
     updaterMenu->addAction(updaterAction);
    
     Updater *updater = new Updater(updaterAction);
     */
    // This constructor initializes the updater,
    // and takes care of connecting the check for updates action and updating its enabled state
    // Note: this must be called on the main thread
    Updater(QAction *checkForUpdatesAction);

private Q_SLOTS:
    void checkForUpdates();

private:
#ifdef __OBJC__
    // Expose ObjC type only to updater_sparkle.mm. This allows ARC to properly track its lifetime.
    AppUpdaterDelegate *_updaterDelegate;
#else
    void *_updaterDelegate;
#endif
};

#endif
