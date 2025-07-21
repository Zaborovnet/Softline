#include "mainwindow.h"
#include "QHeaderView"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // Directory selection
  QHBoxLayout *dirLayout = new QHBoxLayout();
  m_directoryEdit = new QLineEdit(this);
  QPushButton *browseButton = new QPushButton("Browse...", this);
  dirLayout->addWidget(m_directoryEdit);
  dirLayout->addWidget(browseButton);

  // Quota settings
  QHBoxLayout *ButtonLayout = new QHBoxLayout();
  QPushButton *startButton = new QPushButton("Start", this);
  QPushButton *stopButton = new QPushButton("Stop", this);
  ButtonLayout->addWidget(startButton);
  ButtonLayout->addWidget(stopButton);

  // Buttons
  QHBoxLayout *quotaLayout = new QHBoxLayout();
  QLabel *quotaLabel = new QLabel("Quota (MB):", this);
  m_quotaEdit = new QLineEdit("100", this);
  quotaLayout->addWidget(quotaLabel);
  quotaLayout->addWidget(m_quotaEdit);

  // File table
  m_fileTable = new QTableWidget(0, 3, this);
  m_fileTable->setHorizontalHeaderLabels(QStringList{"Path", "Size", "Last Modified"});
  m_fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

  // Status bar
  m_statusLabel = new QLabel("Total: 0 MB, Quota: 100 MB", this);

  // Layout setup
  mainLayout->addLayout(dirLayout);
  mainLayout->addLayout(quotaLayout);
  mainLayout->addLayout(ButtonLayout);
  mainLayout->addWidget(m_fileTable);
  mainLayout->addWidget(m_statusLabel);

  setCentralWidget(centralWidget);
  resize(800, 600);

  // Connections
  connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseDirectory);
  connect(m_quotaEdit, &QLineEdit::textChanged, this, &MainWindow::updateQuota);

  connect(startButton, &QPushButton::clicked, this, &MainWindow::start_find);
  connect(stopButton, &QPushButton::clicked, this, &MainWindow::stop_find);

  // Monitor thread
  m_monitorThread = new MonitorThread(this);
  connect(m_monitorThread, &MonitorThread::fileListUpdated, this, &MainWindow::updateFileList);
  connect(m_monitorThread, &MonitorThread::errorOccurred, this, &MainWindow::handleError);
  m_monitorThread->start();

}

MainWindow::~MainWindow() {
  m_monitorThread->stop();
  m_monitorThread->wait();
  delete m_monitorThread;
}

void MainWindow::browseDirectory() {
  QString dir = QFileDialog::getExistingDirectory(this, "Select Directory");
  if (!dir.isEmpty()) {
    m_directoryEdit->setText(dir);
    m_monitorThread->setDirectory(dir);
  }
}

void MainWindow::updateQuota() {
  bool ok;
  double mb = m_quotaEdit->text().toDouble(&ok);
  if (ok && mb > 0) {
    m_quotaBytes = static_cast<qint64>(mb * 1024 * 1024);
    m_monitorThread->setQuota(m_quotaBytes);
    m_statusLabel->setText(QString("Total: - MB, Quota: %1 MB").arg(mb, 0, 'f', 2));
  }
}

void MainWindow::updateFileList(const QList<FileInfo> &files, qint64 totalSize) {
  m_fileTable->setRowCount(files.size());

  for (int i = 0; i < files.size(); ++i) {
    const FileInfo &fi = files[i];
    m_fileTable->setItem(i, 0, new QTableWidgetItem(fi.filePath_str));
    m_fileTable->setItem(i, 1, new QTableWidgetItem(formatSize(fi.size_i)));
    m_fileTable->setItem(i, 2, new QTableWidgetItem(fi.lastModified_o.toString("yyyy-MM-dd hh:mm:ss")));
  }

  double totalMB = totalSize / (1024.0 * 1024.0);
  double quotaMB = m_quotaBytes / (1024.0 * 1024.0);
  m_statusLabel->setText(QString("Total: %1 MB, Quota: %2 MB").arg(totalMB, 0, 'f', 2).arg(quotaMB, 0, 'f', 2));
}

void MainWindow::handleError(const QString &error) {
  QMessageBox::critical(this, "Error", error);
}

QString MainWindow::formatSize(qint64 bytes) {
  constexpr qint64 KB = 1024;
  constexpr qint64 MB = KB * 1024;
  constexpr qint64 GB = MB * 1024;

  if (bytes >= GB) return QString("%1 GB").arg(bytes / static_cast<double>(GB), 0, 'f', 2);
  if (bytes >= MB) return QString("%1 MB").arg(bytes / static_cast<double>(MB), 0, 'f', 2);
  if (bytes >= KB) return QString("%1 KB").arg(bytes / static_cast<double>(KB), 0, 'f', 2);
  return QString("%1 bytes").arg(bytes);
}

void MainWindow::start_find()
{
  m_monitorThread->startWatch();
}

void MainWindow::stop_find()
{
  m_monitorThread->stop();
}
