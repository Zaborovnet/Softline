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

/**
  @brief Основной класс дял отображения UI
*/
class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
  /**
   * @brief Конструктор
   */
  MainWindow(QWidget *parent = nullptr);

  /**
   * @brief Деструктор
   */
  ~MainWindow();

  private slots:
  /** @brief Обработчик вызова файлового менеджера*/
  void browseDirectory();
  /** @brief Обновление квоты при нажатие на кнопку Accept Quota*/
  void updateQuota();
  /** @brief Обновление таблицы при получение нового списка файлов в целевой директории
   * @param[in] files_lst Лист с файлами
   * @param[in] totalSize_i Общий объём всех файлов
   */
  void updateFileList(const QList<FileInfo> &files_lst, size_t totalSize_i);
  /** @brief Обработчик ошибок при их получение
   * @param[in] error_str ошибка
   */
  void handleError(const QString &error_str);

  private:
  QLineEdit *_directoryEdit_po;           ///< @brief Текстовое поле в котором указывается путь до целевой директории
  QLineEdit *_quotaEdit_po;               ///< @brief Текстовое поле в котором указывается квота
  QTableWidget *_fileTable_po;            ///< @brief Таблица в которую выводятся данные по найденным файлам
  QLabel *_statusLabel_po;                ///< @brief Лэйбл в котором записывеься вес всей директоии и размер квоты
  MonitorThread *_monitorThread_po;       ///< @brief Мониторинг файлов
  size_t _quotaBytes_i;                   ///< @brief Размер квоты

  QString formatSize_str(size_t bytes_i); ///< @brief Перевод в понятные названия размеров файлов
};

#endif // MAINWINDOW_H
