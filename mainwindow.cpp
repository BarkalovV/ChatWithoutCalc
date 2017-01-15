/// Работа с сетью
/// ==============
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QTime>

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  ui->setupUi(this);
  socket = NULL;
  on_enterChatButton_clicked();
}

MainWindow::~MainWindow() {
  delete ui;
}

// Создание UDP-чата
void MainWindow::UdpChat(QString nick, int port) { 
  if(socket != NULL) {
    socket->close();
    delete socket;
    socket = NULL;
  }

  log(QString("Чат создан: port %1").arg(port));
  socket = new QUdpSocket(this);

  if(socket->bind(QHostAddress::Any,port)) {
    connect(socket, SIGNAL(readyRead()), this, SLOT(read()));
  }
  else {
    qDebug() << "Port " << port << " in use. Change port!";
    log(QString("порт %1 занят. давай другой").arg(port));
    return;
  }

  send(nick + " - в чате", USUAL_MESSAGE); 
  QTime now = QTime::currentTime();
  QString nowStr = now.toString("hh:mm:ss");
  QString str = nowStr + " " +ui->nicknameEdit->text();
  send(str,PERSON_ONLINE);

  ui->onlineList->addItem(str);

}
// Нажимаем на кнопку "Войти в чат"
void MainWindow::on_enterChatButton_clicked() {
  QString nick = ui->nicknameEdit->text();
  UdpChat(nick,
          ui->portNumEdit->text().toInt());
  // Разрешаем отправлять сообщения только когда уже в чате
  ui->sendButton->setEnabled(true);
  ui->nicknameEdit->setEnabled(false);
  ui->messageEdit->setEnabled(true);
}
// Запрещаем писать во время смены ника, меняем ник
 void MainWindow::on_changeButton_clicked(){
     ui->nicknameEdit->setEnabled(true);
     ui->messageEdit->setEnabled(false);

 }



/// Отправка сообщения в сеть
///-->
void MainWindow::send(QString str, MessageType type) {
  if(type == USUAL_MESSAGE)
    log(QString("send: %1 %2").arg(str).arg(type));  
  QByteArray data; // Массив данных для отправки
  // Последовательно выводим в него байты
  QDataStream out(&data, QIODevice::WriteOnly);
  out << qint8(type); // Тип сообщения
  out << str; // Само сообщение

  // Отправляем полученный массив данных всем в локальный сети
  // на порт указанный в интерфейсе
  socket->writeDatagram(data,QHostAddress::Broadcast,
                        ui->portNumEdit->text().toInt());
}

void MainWindow::saveToLogFile(QString str) {
  QFile file("log.txt");
  file.open(QIODevice::WriteOnly | QIODevice::Text
            | QIODevice::Append );
  QTextStream log(&file);
  log.setCodec("UTF-8");
  log << str << endl;
  file.close();
}

void MainWindow::read() {
  while (socket->hasPendingDatagrams()) {
    log("read>>");
    // Массив (буфер) для полученных данных
    QByteArray buf;
    // Устанавливаем массиву размер
    // соответствующий размеру полученного пакета данных
    buf.resize(socket->pendingDatagramSize());
    QHostAddress* address = new QHostAddress();
    // Принимаем данные пришедшие по сети
    socket->readDatagram(buf.data(), buf.size(), address);   
    QDataStream in(&buf, QIODevice::ReadOnly);
    // Получаем тип пакета
    qint8 type = 0;
    in >> type;

    QString str;
    in >> str;
    log(QString("read>> %1 %2").arg(str).arg(type));

    if(str.length() == 0)
      return;

    if (type == USUAL_MESSAGE) {
      // Записываем входящие сообщения в файл
      saveToLogFile(str);

      // Отображаем строчку в интерфейсе
      ui->plainTextEdit->appendPlainText(str);
      }
    else if (type == PERSON_ONLINE) {
        ui->onlineList->addItem(str);
        }
    else if (type == WHO_IS_ONLINE) {
      log(QString("Отвечаем своим ником: %1").arg(ui->nicknameEdit->text()));
      QTime now = QTime::currentTime();
      QString nowStr = now.toString("hh:mm:ss");
      send(nowStr + " " +
           ui->nicknameEdit->text(),
           PERSON_ONLINE);
    }
  }
}

// Нажимаем на кнопку "Отправить сообщение"

void MainWindow::on_sendButton_clicked() {
  // Вся строка сообщения: "ник: сообщение"
  send(ui->nicknameEdit->text() + ": " +
       ui->messageEdit->text(),
       USUAL_MESSAGE);

  ui->messageEdit->clear();
  QTime now = QTime::currentTime();
  QString nowStr = now.toString("hh:mm:ss");
  send(nowStr + " " +
  ui->nicknameEdit->text(),PERSON_ONLINE);

}


void MainWindow::on_messageEdit_returnPressed() {
  on_sendButton_clicked();
}

// Запись в log-окно
void MainWindow::log(QString s) {
  ui->log->append(s);
}
