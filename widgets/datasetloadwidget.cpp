#include "datasetloadwidget.h"

#include <stdexcept>

#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "GuiConstants.h"
#include "knossos.h"
#include "knossos-global.h"
#include "loader.h"
#include "mainwindow.h"
#include "network.h"
#include "viewer.h"

void DatasetLoadWidget::MessageBoxInformation(QString message) {
    QMessageBox info;
    info.setWindowFlags(Qt::WindowStaysOnTopHint);
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("Information");
    info.setText(message);
    info.addButton(QMessageBox::Ok);
    info.exec();
}

DatasetLoadWidget::DatasetLoadWidget(QWidget *parent) : QDialog(parent) {

    cubeEdgeSpin.setRange(1, 256);
    cubeEdgeSpin.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    supercubeEdgeSpin = new QSpinBox;
    supercubeEdgeSpin->setMinimum(3);
    supercubeEdgeSpin->setSingleStep(2);
    supercubeEdgeSpin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    supercubeSizeLabel = new QLabel();

    cancelButton = new QPushButton("Cancel");
    processButton = new QPushButton("Use");

    QFrame* line0 = new QFrame();
    line0->setFrameShape(QFrame::HLine);
    line0->setFrameShadow(QFrame::Sunken);
    auto hLayoutLine0 = new QHBoxLayout;
    hLayoutLine0->addWidget(line0);

    auto hLayoutDatasetInfo = new QGridLayout;
    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(3);

    tableWidget->resizeRowsToContents();
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);
    hLayoutDatasetInfo->addWidget(tableWidget,0,0);

    infolabel = new QLabel();
    auto str = QString("");
    infolabel->setText(str);

    scrollarea = new QScrollArea();
    scrollarea->setWidgetResizable(true);
    scrollarea->setWidget(infolabel);

    hLayoutDatasetInfo->addWidget(scrollarea, 0, 1);

    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    auto hLayoutLine1 = new QHBoxLayout;
    hLayoutLine1->addWidget(line1);

    auto hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(supercubeEdgeSpin);
    supercubeEdgeSpin->setAlignment(Qt::AlignLeft);
    hLayout2->addWidget(supercubeSizeLabel);

    auto hLayoutCubeSize = new QHBoxLayout;
    hLayoutCubeSize->addWidget(&cubeEdgeSpin);
    hLayoutCubeSize->addWidget(&cubeEdgeLabel);

    auto hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(processButton);
    hLayout3->addWidget(cancelButton);

    auto localLayout = new QVBoxLayout();

    localLayout->addLayout(hLayoutLine0);
    localLayout->addLayout(hLayoutDatasetInfo);
    localLayout->addLayout(hLayoutLine1);
    localLayout->addLayout(hLayout2);
    localLayout->addWidget(&segmentationOverlayCheckbox);
    localLayout->addLayout(hLayout3);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addLayout(localLayout);
    setLayout(mainLayout);

    QObject::connect(&cubeEdgeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DatasetLoadWidget::adaptMemoryConsumption);
    QObject::connect(supercubeEdgeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DatasetLoadWidget::adaptMemoryConsumption);
    QObject::connect(&segmentationOverlayCheckbox, &QCheckBox::stateChanged, this, &DatasetLoadWidget::adaptMemoryConsumption);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &DatasetLoadWidget::cancelButtonClicked);
    QObject::connect(processButton, &QPushButton::clicked, this, &DatasetLoadWidget::processButtonClicked);
    QObject::connect(tableWidget, &QTableWidget::clicked, this, &DatasetLoadWidget::processTableWidgetClicked);
    QObject::connect(tableWidget, &QTableWidget::itemSelectionChanged, this, &DatasetLoadWidget::processTableWidgetClicked);

    this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

void DatasetLoadWidget::addRow(int row, int col) {
    if(col == 0 && row == tableWidget->rowCount() - 1) {
        if(tableWidget->item(row, 0)->text() != "") {
            QPushButton *delDs = new QPushButton("Del");
            delDs->setMaximumWidth(40);
            delDs->setEnabled(true);
            tableWidget->setCellWidget(tableWidget->rowCount()-1, 2, delDs);
            QObject::connect(delDs, &QPushButton::clicked, this, &DatasetLoadWidget::delClicked);

            tableWidget->insertRow(tableWidget->rowCount());

            QPushButton *addDs = new QPushButton("…");
            addDs->setMaximumWidth(20);
            QPushButton *fdelDs = new QPushButton("Del");
            delDs->setMaximumWidth(40);
            fdelDs->setEnabled(false);
            QTableWidgetItem *t = new QTableWidgetItem("");
            tableWidget->setItem(tableWidget->rowCount() - 1, 0, t);
            tableWidget->setCellWidget(tableWidget->rowCount() - 1, 1, addDs);
            tableWidget->setCellWidget(tableWidget->rowCount() - 1, 2, fdelDs);

            QObject::connect(addDs, &QPushButton::clicked, this, &DatasetLoadWidget::addClicked);
        }
    }
}

void DatasetLoadWidget::addClicked(){
    for(int row = 0; row < tableWidget->rowCount(); ++row) {
        for(int col = 0; col < tableWidget->columnCount(); ++col) {
            if(sender() == tableWidget->cellWidget(row, col)) {
                //open dialog
                state->viewerState->renderInterval = SLOW;
                QApplication::processEvents();
                QString selectFile = QFileDialog::getOpenFileName(this, "Select a KNOSSOS dataset", QDir::homePath(), "*.conf");

                if(selectFile != "") {
                    qDebug() << selectFile;
                    QTableWidgetItem *t = new QTableWidgetItem(selectFile);
                    tableWidget->setItem(row, 0, t);
                }

                state->viewerState->renderInterval = FAST;
            }
        }
    }
}

void DatasetLoadWidget::delClicked(){
    for(int row = 0; row < tableWidget->rowCount(); ++row) {
        for(int col = 0; col < tableWidget->columnCount(); ++col) {
            if(sender() == tableWidget->cellWidget(row, col)) {
                //delete row
                tableWidget->removeRow(row);
            }
        }
    }
}

void DatasetLoadWidget::processTableWidgetClicked() {
    if(tableWidget->selectedItems().count() == 0) return;

    QTableWidgetItem * itemClicked = tableWidget->selectedItems().front();
    if(itemClicked->text() == "") return;

    datasetinfo = getConfigFileInfo(itemClicked->text().toStdString().c_str());

    QString infotext;

    if(datasetinfo.remote) {
        infotext = QString("<b>Remote Dataset</b><br>URL: %0%1<br>Boundary (x y z): %2 %3 %4<br>Compression: %5<br>cubeEdgeLength: %6<br>Magnification: %7<br>Scale (x y z): %8 %9 %10")
                .arg(datasetinfo.ftphostname.c_str())
                .arg(datasetinfo.ftpbasepath.c_str())
                .arg(datasetinfo.boundary.x).arg(datasetinfo.boundary.y).arg(datasetinfo.boundary.z)
                .arg(datasetinfo.compressionRatio)
                .arg(datasetinfo.cubeEdgeLength)
                .arg(datasetinfo.magnification)
                .arg(datasetinfo.scale.x)
                .arg(datasetinfo.scale.y)
                .arg(datasetinfo.scale.z);
    } else {
        infotext = QString("<b>Locale Dataset</b><br>Boundary (x y z): %0 %1 %2<br>Compression: %3<br>cubeEdgeLength: %4<br>Magnification: %5<br>Scale (x y z): %6 %7 %8")
                .arg(datasetinfo.boundary.x).arg(datasetinfo.boundary.y).arg(datasetinfo.boundary.z)
                .arg(datasetinfo.compressionRatio)
                .arg(datasetinfo.cubeEdgeLength)
                .arg(datasetinfo.magnification)
                .arg(datasetinfo.scale.x)
                .arg(datasetinfo.scale.y)
                .arg(datasetinfo.scale.z);
    }

    infolabel->setText(infotext);
}

QStringList DatasetLoadWidget::getRecentPathItems() {
    QStringList recentPaths;

    for(int row = 0; row < tableWidget->rowCount() - 1; ++row) {
        if(tableWidget->item(row, 0)->text() != "") {
            recentPaths.append(tableWidget->item(row, 0)->text());
        }
    }

    return recentPaths;
}

void DatasetLoadWidget::adaptMemoryConsumption() {
    const auto cubeEdge = cubeEdgeSpin.value();
    const auto superCubeEdge = supercubeEdgeSpin->value();
    auto mibibytes = std::pow(cubeEdge, 3) * std::pow(superCubeEdge, 3) / std::pow(1024, 2);
    mibibytes += segmentationOverlayCheckbox.isChecked() * OBJID_BYTES * mibibytes;
    const auto fov = cubeEdge * (superCubeEdge - 1);
    auto text = QString("Data cache cube edge length (%1 MiB RAM) – FOV %2 pixel per dimension").arg(mibibytes).arg(fov);
    supercubeSizeLabel->setText(text);
    const auto maxsupercubeedge = TEXTURE_EDGE_LEN / cubeEdge;
    //make sure it’s an odd number
    supercubeEdgeSpin->setMaximum(maxsupercubeedge - (maxsupercubeedge % 2 == 0 ? 1 : 0));
}

void DatasetLoadWidget::cancelButtonClicked() {
    this->hide();
}

void DatasetLoadWidget::processButtonClicked() {
    if(tableWidget->currentItem()->text().isEmpty()) {
        MessageBoxInformation("No path selected");

        return;
    }

    if(loadDataset(true, tableWidget->currentItem()->text())) {
        this->hide(); //hide datasetloadwidget only if we could successfully load a widget
    }
}

/* dataset can be selected in three ways:
 * 1. by selecting the folder containing a k.conf (for multires datasets it's a "magX" folder)
 * 2. for multires datasets: by selecting the dataset folder (the folder containing the "magX" subfolders)
 * 3. by specifying a .conf directly.
 */
bool DatasetLoadWidget::loadDataset(bool isGUI, QString path) {
    QFile confFile;
    QString filePath; // for holding the whole path to a .conf file
    QFileInfo pathInfo;

    if(path.isEmpty()) {
        path = lastused;
    } else {
        lastused = path;
    }

    //check if we have a remote conf
    if(path.startsWith("http", Qt::CaseInsensitive)) {
        std::string tmp = downloadRemoteConfFile(path);
        path = QString::fromStdString(tmp);
        if(path.isEmpty()) return false;
    }

    pathInfo.setFile(path);

    if(pathInfo.isFile()) { // .conf file selected (case 3)
        filePath = path;
        confFile.setFileName(filePath);
    }  else { // folder selected
        if(path.endsWith('/') == false && path.endsWith('\\') == false) {
            // qFileInfo only recognizes paths with trailing slash as directories.
            path.append('/');
            pathInfo.setFile(path);
        }
        QDir directory(path);
        QStringList dirContent = directory.entryList(QStringList("*.conf"));
        if(dirContent.empty()) { // apparently the base dataset folder (case 2) was selected
            // find the magnification subfolders and look for a .conf file starting at lowest mag
            bool foundConf = false;
            dirContent = directory.entryList(QStringList("*mag*"), QDir::Dirs, QDir::Name);
            for(const auto magPath : dirContent) {
                QDir magDir(QString("%1/%2").arg(directory.absolutePath()).arg(magPath));
                QStringList subDirContent = magDir.entryList(QStringList("*.conf"), QDir::Files);
                if(subDirContent.empty() == false) {
                    filePath = QString("%1/%2/%3").arg(directory.absolutePath()).arg(magPath).arg(subDirContent.front());
                    confFile.setFileName(filePath);
                    QFile::copy(filePath, QString("%1/%2.k.conf").arg(directory.absolutePath()).arg(directory.dirName()));
                    foundConf = true;
                    break;
                }
            }
            if(foundConf == false) {
                if (isGUI) {
                    MessageBoxInformation("Could not find a dataset file (*.conf)");
                }
                return false;
            }
        }
        else {
            filePath = QString("%1/%2").arg(directory.absolutePath()).arg(dirContent.front());
            if(QRegExp(".*mag[0-9]+").exactMatch(directory.absolutePath())) {
                // apparently the magnification folder was selected (case 1)
                directory.cdUp();
                QFile::copy(filePath, QString("%1/%2.k.conf").arg(directory.absolutePath()).arg(directory.dirName()));
            }
        }
    }

    // Note:
    // We clear the skeleton *before* reading the new config. In case we fail later, the skeleton would be nevertheless be gone.
    // This is a gamble we take, in order to not have possible bugs where the skeleton depends on old configuration values.
    if (isGUI) {
        emit clearSkeletonSignalGUI();
    } else {
        emit clearSkeletonSignalNoGUI();
    }

    emit breakLoaderSignal();

    if(false == Knossos::readConfigFile(filePath.toStdString().c_str())) {
        MessageBoxInformation(QString("Failed to read config from %1").arg(filePath));
        return false;
    }

    // we want state->path to hold the path to the dataset folder
    // instead of a path to a subfolder of a specific magnification
    QDir datasetDir(pathInfo.absolutePath());
    if(QRegExp(".*mag[0-9]+").exactMatch(datasetDir.absolutePath())) {
        datasetDir.cdUp();
    }
    strcpy(state->path, datasetDir.absolutePath().toStdString().c_str());

    knossos->commonInitStates();

    // check if a fundamental geometry variable has changed. If so, the loader requires reinitialization
    state->cubeEdgeLength = cubeEdgeSpin.text().toInt();
    state->M = supercubeEdgeSpin->value();
    state->overlay = segmentationOverlayCheckbox.isChecked();

    if(state->M * state->cubeEdgeLength >= TEXTURE_EDGE_LEN) {
        qDebug() << "Please choose smaller values for M or N. Your choice exceeds the KNOSSOS texture size!";
        throw std::runtime_error("Please choose smaller values for M or N. Your choice exceeds the KNOSSOS texture size!");
    }

    applyGeometrySettings();

    emit datasetSwitchZoomDefaults();

    // reset skeleton viewport
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_RESET;
    }

    emit startLoaderSignal();

    emit updateDatasetCompression();

    // ...beginning with loading the middle of dataset
    Coordinate range(state->boundary.x/2, state->boundary.y/2, state->boundary.z/2);
    state->viewerState->currentPosition = range;

    emit datasetChanged(Coordinate(0, 0, 0), state->boundary, segmentationOverlayCheckbox.isChecked());

    emit userMoveSignal(0, 0, 0, USERMOVE_NEUTRAL, VIEWPORT_UNDEFINED);

    return true;
}

DatasetLoadWidget::Datasetinfo DatasetLoadWidget::getConfigFileInfo(const char *path) {
    Datasetinfo info;
    QString qpath{path};

    if(qpath.startsWith("http", Qt::CaseInsensitive)) {
        std::string tmp = downloadRemoteConfFile(qpath);
        qpath = QString::fromStdString(tmp);

        if(qpath == "") return info;
    }

    QFile file(qpath);

    if(!file.open(QIODevice::ReadOnly)) {
        qDebug("Error reading config file at path:%s", qpath.toStdString().c_str());
        return info;
    }

    QTextStream stream(&file);
    while(!stream.atEnd()) {
        QString line = stream.readLine();
        if(line.isEmpty())
            continue;

        QStringList tokenList = line.split(
            QRegExp("[ ;]"),
            QString::SkipEmptyParts
        );

        QString token = tokenList.at(0);

        if(token == "experiment") {
            token = tokenList.at(2);
            QStringList experimentTokenList = token.split(
                        QRegExp("[\"]"),
                        QString::SkipEmptyParts);
            info.experimentname = experimentTokenList.at(0).toStdString();

        } else if(token == "scale") {
            token = tokenList.at(1);
            if(token == "x") {
                info.scale.x = tokenList.at(2).toFloat();
            } else if(token == "y") {
                info.scale.y = tokenList.at(2).toFloat();
            } else if(token == "z") {
                info.scale.z = tokenList.at(2).toFloat();
            }
        } else if(token == "boundary") {
            token = tokenList.at(1);
            if(token == "x") {
                info.boundary.x = tokenList.at(2).toFloat();
            } else if(token == "y") {
                info.boundary.y = tokenList.at(2).toFloat();
            } else if(token == "z") {
                info.boundary.z = tokenList.at(2).toFloat();
            }
        } else if(token == "magnification") {
            info.magnification = tokenList.at(1).toInt();
        } else if(token == "cube_edge_length") {
            info.cubeEdgeLength = tokenList.at(1).toInt();
        } else if(token == "ftp_mode") {
            info.remote = true;

            info.ftphostname = tokenList.at(1).toStdString();
            info.ftpbasepath = tokenList.at(2).toStdString();

        } else if (token == "compression_ratio") {
            info.compressionRatio = tokenList.at(1).toInt();
        }
    }

    return info;
}

void DatasetLoadWidget::saveSettings() {
    QSettings settings;
    settings.beginGroup(DATASET_WIDGET);

    settings.setValue(DATASET_LAST_USED, lastused);

    settings.setValue(DATASET_MRU, getRecentPathItems());

    settings.setValue(DATASET_SUPERCUBE_EDGE, state->M);
    settings.setValue(DATASET_OVERLAY, state->overlay);

    settings.endGroup();
}

void DatasetLoadWidget::applyGeometrySettings() {
    //settings depending on supercube and cube size
    state->cubeSliceArea = state->cubeEdgeLength * state->cubeEdgeLength;
    state->cubeBytes = state->cubeEdgeLength * state->cubeEdgeLength * state->cubeEdgeLength;
    state->cubeSetElements = state->M * state->M * state->M;
    state->cubeSetBytes = state->cubeSetElements * state->cubeBytes;

    for(uint i = 0; i <  Viewport::numberViewports; i++) {
        state->viewerState->vpConfigs[i].texture.texUnitsPerDataPx /= static_cast<float>(state->magnification);
        state->viewerState->vpConfigs[i].texture.usedTexLengthDc = state->M;
    }

    if(state->M * state->cubeEdgeLength >= TEXTURE_EDGE_LEN) {
        qDebug() << "Please choose smaller values for M or N. Your choice exceeds the KNOSSOS texture size!";
        throw std::runtime_error("Please choose smaller values for M or N. Your choice exceeds the KNOSSOS texture size!");
    }
}

void DatasetLoadWidget::loadSettings() {
    QSettings settings;
    settings.beginGroup(DATASET_WIDGET);

    lastused = settings.value(DATASET_LAST_USED).toString();

    //add datasets from file
    for(const auto & mru : settings.value(DATASET_MRU).toStringList()) {
        tableWidget->insertRow(tableWidget->rowCount());

        QPushButton *addDs = new QPushButton("…");
        addDs->setMaximumWidth(20);
        QPushButton *delDs = new QPushButton("Del");
        delDs->setMaximumWidth(40);
        QObject::connect(addDs, &QPushButton::clicked, this, &DatasetLoadWidget::addClicked);
        QObject::connect(delDs, &QPushButton::clicked, this, &DatasetLoadWidget::delClicked);

        QTableWidgetItem * t = new QTableWidgetItem(mru);
        tableWidget->setItem(tableWidget->rowCount() - 1, 0, t);
        tableWidget->setCellWidget(tableWidget->rowCount() - 1, 1, addDs);
        tableWidget->setCellWidget(tableWidget->rowCount() - 1, 2, delDs);
    }

    //add public datasets
    auto datasetsDir = QDir(":/resources/datasets");
    for (const auto & dataset : datasetsDir.entryInfoList()) {
        if (tableWidget->findItems(dataset.absoluteFilePath(), Qt::MatchExactly).empty()) {
            tableWidget->insertRow(tableWidget->rowCount());

            QPushButton *addDs = new QPushButton("…");
            addDs->setMaximumWidth(20);
            QPushButton *delDs = new QPushButton("Del");
            delDs->setMaximumWidth(40);
            QObject::connect(addDs, &QPushButton::clicked, this, &DatasetLoadWidget::addClicked);
            QObject::connect(delDs, &QPushButton::clicked, this, &DatasetLoadWidget::delClicked);

            QTableWidgetItem *t = new QTableWidgetItem(dataset.absoluteFilePath());
            tableWidget->setItem(tableWidget->rowCount() - 1, 0, t);
            tableWidget->setCellWidget(tableWidget->rowCount() - 1, 1, addDs);
            tableWidget->setCellWidget(tableWidget->rowCount() - 1, 2, delDs);
        }
    }

    //add Empty row at the end
    tableWidget->insertRow(tableWidget->rowCount());
    QPushButton *addDs = new QPushButton("…");
    addDs->setMaximumWidth(20);
    QPushButton *delDs = new QPushButton("Del");
    delDs->setMaximumWidth(40);
    QObject::connect(addDs, &QPushButton::clicked, this, &DatasetLoadWidget::addClicked);
    delDs->setEnabled(false);
    QTableWidgetItem *t = new QTableWidgetItem("");
    tableWidget->setItem(tableWidget->rowCount() - 1, 0, t);
    tableWidget->setCellWidget(tableWidget->rowCount() - 1, 1, addDs);
    tableWidget->setCellWidget(tableWidget->rowCount() - 1, 2, delDs);

    if (QApplication::arguments().filter("supercube-edge").empty()) {//if not provided by cmdline
        state->M = settings.value(DATASET_SUPERCUBE_EDGE, 3).toInt();
    }
    if (QApplication::arguments().filter("overlay").empty()) {//if not provided by cmdline
        state->overlay = settings.value(DATASET_OVERLAY, false).toBool();
    }

    cubeEdgeSpin.setValue(state->cubeEdgeLength);
    supercubeEdgeSpin->setValue(state->M);
    segmentationOverlayCheckbox.setCheckState(state->overlay ? Qt::Checked : Qt::Unchecked);
    adaptMemoryConsumption();

    settings.endGroup();

    applyGeometrySettings();

    QObject::connect(tableWidget, &QTableWidget::cellChanged, this, &DatasetLoadWidget::addRow);
    tableWidget->resizeColumnsToContents();
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}
