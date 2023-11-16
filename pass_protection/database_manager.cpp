#include "database_manager.hpp"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <qurl.h>

DatabaseManager::DatabaseManager(const QString &path) {
  database = QSqlDatabase::addDatabase("QSQLITE");
  database.setDatabaseName(path);

  if (!database.open()) {
    qDebug() << "Error: connection with database";
  } else {
    qDebug() << "Database: connection ok";
  }
}

bool DatabaseManager::createUser(
    const QString &username, const QString &pass, const QString &lastName,
    const QString &firstName, const QString &patronymic, const QDate &birthDate,
    const QString &birthPlace, const QString &phone) {
  bool success = false;

  QSqlQuery selectQ;
  selectQ.prepare(
      R"(SELECT "username" FROM "main"."users" WHERE username = ?)");
  selectQ.addBindValue(username);
  selectQ.exec();
  if (!selectQ.exec()) {
    qDebug() << "selectUser error:" << selectQ.lastError();
    return false;
  } else {
    qDebug() << "selectUser status:" << "ok";
    if (selectQ.next()) {
      qDebug() << "selectUser collision";
      return false;
    }
  }

  QSqlQuery insertQ;
  insertQ.prepare(
      R"(INSERT INTO "main"."users" ("username", "password", "last_name", "first_name", "patronymic", "birthdate", "birthplace", "phone") VALUES (?, ?, ?, ?, ?, ?, ?, ?);)");

  insertQ.addBindValue(username);
  insertQ.addBindValue(pass);
  insertQ.addBindValue(lastName);
  insertQ.addBindValue(firstName);
  insertQ.addBindValue(patronymic);
  insertQ.addBindValue(birthDate.toString());
  insertQ.addBindValue(birthPlace);
  insertQ.addBindValue(phone);

  if (insertQ.exec()) {
    success = true;
  } else {
    qDebug() << "createUser error:" << insertQ.lastError();
  }

  return success;
}

bool DatabaseManager::changePass(const QString &username, const QString &pass,
                                 const QString &controlPass) {
  bool result = false;

  if (login(username, pass)) {
    QSqlQuery updateQ;
    updateQ.prepare(
        R"(UPDATE "main"."users" SET password = ? WHERE username = ?)");
    updateQ.addBindValue(controlPass);
    updateQ.addBindValue(username);
    if (!updateQ.exec()) {
      qDebug() << "updateUser error:" << updateQ.lastError();
    } else {
      qDebug() << "updateUser status:" << "ok";
      result = true;
    }
  }

  return result;
}

bool DatabaseManager::login(const QString &username, const QString &pass) {
  QSqlQuery selectQ;
  selectQ.prepare(
      R"(SELECT 1 FROM "main"."users" WHERE username = ? AND password = ?)");
  selectQ.addBindValue(username);
  selectQ.addBindValue(pass);

  bool result = false;
  if (!selectQ.exec()) {
    qDebug() << "selectUser error:" << selectQ.lastError();
  } else {
    qDebug() << "selectUser status:" << "ok";
    if (selectQ.next()) {
      qDebug() << "selectUser collision";
      result = true;
    }
  }
  return result;
}