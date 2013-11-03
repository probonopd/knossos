#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDir>

#include <curl/curl.h>

#include "knossos-global.h"
#include "widgets/mainwindow.h"
#include "skeletonizer.h"
#include "taskmanagementwidget.h"
#include "taskmanagementmaintab.h"

extern stateInfo *state;

TaskManagementWidget::TaskManagementWidget(TaskLoginWidget *loginWidget, QWidget *parent) :
    QWidget(parent)
{
    mainTab = new TaskManagementMainTab(loginWidget);
    detailsTab = new TaskManagementDetailsTab();
    tabs = new QTabWidget(this);
    tabs->addTab(mainTab, "General");
    tabs->addTab(detailsTab, "Description");

    QHBoxLayout *layout = new QHBoxLayout(); // add in layout, so that tabs resize, too
    layout->addWidget(tabs);

    setWindowTitle("Task Management");
    setLayout(layout);
    connect(mainTab, SIGNAL(hideSignal()), this, SLOT(hide()));
    connect(mainTab, SIGNAL(updateDescriptionSignal(QString, QString)), detailsTab, SLOT(updateDescriptionSlot(QString, QString)));
}

void TaskManagementWidget::closeEvent(QCloseEvent *) {
    this->hide();
}
