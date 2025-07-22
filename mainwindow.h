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
  void updateFileList(const QList<FileInfo> &files_lst, size_t totalSize_i);
  void handleError(const QString &error_str);

  private:
  QLineEdit *_directoryEdit_po;
  QLineEdit *_quotaEdit_po;
  QTableWidget *_fileTable_po;
  QLabel *_statusLabel_po;
  MonitorThread *_monitorThread_po;
  size_t _quotaBytes_i;

  QString formatSize_str(size_t bytes_i);
};

#endif // MAINWINDOW_H
