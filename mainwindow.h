#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "monitorthread.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
  void browseDirectory();
  void updateQuota();
  void updateFileList(const QList<FileInfo> &files, qint64 totalSize);
  void handleError(const QString &error);

  private:
  void start_find();
  void stop_find();
  QLineEdit *m_directoryEdit;
  QLineEdit *m_quotaEdit;
  QTableWidget *m_fileTable;
  QLabel *m_statusLabel;
  MonitorThread *m_monitorThread;
  qint64 m_quotaBytes = 100 * 1024 * 1024;

  QString formatSize(qint64 bytes);
};

#endif // MAINWINDOW_H
