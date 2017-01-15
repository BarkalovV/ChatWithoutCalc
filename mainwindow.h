#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpServer>

enum MessageType {
  USUAL_MESSAGE, // Обычный текст
  PERSON_ONLINE, // Сообщение "Я-онлайн"
  WHO_IS_ONLINE, // Запрос о статусе пользователей
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

  void saveToLogFile(QString str);
 private slots:
  void on_enterChatButton_clicked();
  void on_sendButton_clicked();
  void on_messageEdit_returnPressed();
  void on_changeButton_clicked();

 private:
  Ui::MainWindow* ui;


  QUdpSocket* socket;

  // Открываем соединение
  // и отправляем первое сообщение
  void UdpChat(QString nick, int port);

  // Отправить сообщение
  void send(QString str, MessageType type);

  // Вывод в отладочную консоль
  void log(QString s);

  private slots:
  // Получаем сообщения
  void read();


};

#endif // MAINWINDOW_H
