#include "mainwindow.h"
#include "QHeaderView"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  QWidget *centralWidget_po = new QWidget(this);
  QVBoxLayout *mainLayout_po = new QVBoxLayout(centralWidget_po);

  _quotaBytes_i = 100 * 1024 * 1024;

  // Directory selection
  QHBoxLayout *dirLayout_po = new QHBoxLayout();
  _directoryEdit_po = new QLineEdit(this);
  QPushButton *browseButton_po = new QPushButton("Browse...", this);
  dirLayout_po->addWidget(_directoryEdit_po);
  dirLayout_po->addWidget(browseButton_po);

  // Quota settings
  QHBoxLayout *quotaLayout_po = new QHBoxLayout();
  QLabel *quotaLabel_po = new QLabel("Quota (MB):", this);
  _quotaEdit_po = new QLineEdit("100", this);
  QPushButton *acceptQuota_po = new QPushButton("Accept Quota", this);
  quotaLayout_po->addWidget(quotaLabel_po);
  quotaLayout_po->addWidget(_quotaEdit_po);
  quotaLayout_po->addWidget(acceptQuota_po);

  // File table
  _fileTable_po = new QTableWidget(0, 3, this);
  _fileTable_po->setHorizontalHeaderLabels(QStringList{"Path", "Size", "Last Modified"});
  _fileTable_po->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

  // Status bar
  _statusLabel_po = new QLabel("Total: 0 MB, Quota: 100 MB", this);

  // Layout setup
  mainLayout_po->addLayout(dirLayout_po);
  mainLayout_po->addLayout(quotaLayout_po);
  mainLayout_po->addWidget(_fileTable_po);
  mainLayout_po->addWidget(_statusLabel_po);

  setCentralWidget(centralWidget_po);
  resize(800, 600);

  // Connections
  connect(browseButton_po, &QPushButton::clicked, this, &MainWindow::browseDirectory);
  connect(acceptQuota_po, &QPushButton::clicked, this, &MainWindow::updateQuota);
  // connect(_quotaEdit_po, &QLineEdit::textChanged, this, &MainWindow::updateQuota);


  // Monitor thread
  _monitorThread_po = new MonitorThread(this);
  connect(_monitorThread_po, &MonitorThread::fileListUpdated, this, &MainWindow::updateFileList);
  connect(_monitorThread_po, &MonitorThread::errorOccurred, this, &MainWindow::handleError);

  _monitorThread_po->start();
}

MainWindow::~MainWindow() {
  _monitorThread_po->stopWatch();
  _monitorThread_po->wait();
  delete _monitorThread_po;
}

void MainWindow::browseDirectory() {
  QString dir_str = QFileDialog::getExistingDirectory(this, "Select Directory");
  if (!dir_str.isEmpty()) {
    _directoryEdit_po->setText(dir_str);
    _monitorThread_po->setDirectory(dir_str);
  }
}

void MainWindow::updateQuota() {
  bool ok_b;
  double mb_d = _quotaEdit_po->text().toDouble(&ok_b);
  if (ok_b && mb_d > 0) {
    _quotaBytes_i = static_cast<size_t>(mb_d * 1024 * 1024);
    _monitorThread_po->setQuota(_quotaBytes_i);
    _statusLabel_po->setText(QString("Total: - MB, Quota: %1 MB").arg(mb_d, 0, 'f', 2));
  }
}

void MainWindow::updateFileList(const QList<FileInfo> &files_lst, size_t totalSize_i) {
  _fileTable_po->setRowCount(files_lst.size());

  for (int i = 0; i < files_lst.size(); ++i) {
    const FileInfo &fi_ro = files_lst[i];
    _fileTable_po->setItem(i, 0, new QTableWidgetItem(fi_ro.filePath_str));
    _fileTable_po->setItem(i, 1, new QTableWidgetItem(formatSize_str(fi_ro.size_i)));
    _fileTable_po->setItem(i, 2, new QTableWidgetItem(fi_ro.lastModified_o.toString("yyyy-MM-dd hh:mm:ss")));
  }

  double totalMB_d = totalSize_i / (1024.0 * 1024.0);
  double quotaMB_d = _quotaBytes_i / (1024.0 * 1024.0);
  _statusLabel_po->setText(QString("Total: %1 MB, Quota: %2 MB").arg(totalMB_d, 0, 'f', 2).arg(quotaMB_d, 0, 'f', 2));
}

void MainWindow::handleError(const QString &error_str) {
  QMessageBox::critical(this, "Error", error_str);
}

QString MainWindow::formatSize_str(size_t bytes_i) {
  constexpr size_t KB_i = 1024;
  constexpr size_t MB_i = KB_i * 1024;
  constexpr size_t GB_i = MB_i * 1024;

  if (bytes_i >= GB_i) return QString("%1 GB").arg(bytes_i / static_cast<double>(GB_i), 0, 'f', 2);
  if (bytes_i >= MB_i) return QString("%1 MB").arg(bytes_i / static_cast<double>(MB_i), 0, 'f', 2);
  if (bytes_i >= KB_i) return QString("%1 KB").arg(bytes_i / static_cast<double>(KB_i), 0, 'f', 2);
  return QString("%1 bytes").arg(bytes_i);
}

