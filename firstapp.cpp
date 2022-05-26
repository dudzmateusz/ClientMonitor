#include "firstapp.h"
#include "ui_firstapp.h"
#include <QKeyEvent>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QIntValidator>
#include <QVariant>
#include <sstream>
#include <math.h>
#include <list>
#include <iostream>
#include <string>
#include <cstdlib>
#include <QCompleter>
#include <QComboBox>
#include <QDebug>
#include <QStringListModel>




static const char keyPropKey[] = "key";
int count_start = 0;
QString sprawdz_pierwsza_litere;




firstApp::firstApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::firstApp)
{

    ui->setupUi(this);
    ui->filtr_pacjenci->addItem("-",QVariant(0));
    ui->filtr_pacjenci->addItem("Imię",QVariant(1));
    ui->filtr_pacjenci->addItem("Nazwisko",QVariant(2));
    ui->filtr_pacjenci->addItem("Pesel",QVariant(3));
    ui->filtr_pacjenci->addItem("ID",QVariant(4));
    ui->lineEdit_2->setEnabled(0);
    ui->lineEdit_2->setStyleSheet("background-color:lightgrey;");
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->filtr_pacjenci, SIGNAL(currentIndexChanged(int)), this, SLOT(filter_managment(int)));
    ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &firstApp::sendKeyEvent);
    ui->lineEdit->setProperty(keyPropKey, (int)Qt::Key_Any);
    //connect(ui->filtr_pacjenci, SIGNAL(currentIndexChanged(int)), this, SLOT(connector(int index)));
    //connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &firstApp::sendKeyEvent2);
    ui->lineEdit_2->setFocusPolicy(Qt::StrongFocus);
    connect(ui->lineEdit_2, &QLineEdit::textEdited, this, &firstApp::sendKeyEventPacjenci);
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, this, &firstApp::sendKeyEventPacjenci);
    //if (cellitem.){

    //}
    ui->lineEdit_2->setProperty(keyPropKey, (int)Qt::Key_Any);
    //connect(ui->lineEdit_2, SIGNAL(textChanged()), this, SLOT(connector(int)));
    //testConnect("SELECT * FROM Pacjenci");
    //test2("SELECT * FROM Pacjenci;");


}

firstApp::~firstApp()
{
    delete ui;
}

void firstApp::filter_managment(int i){

    if(i!=0){
        ui->lineEdit_2->setEnabled(1);
        ui->lineEdit_2->setStyleSheet("background-color:white;");

    } else {
        ui->lineEdit_2->setEnabled(0);
        ui->lineEdit_2->setStyleSheet("background-color:lightgrey;");

    }

    if (i==0){
      ui->lineEdit_2->setPlaceholderText("Brak możliwości wyszukiwania. Wybierz filtr.");
    } else {
        ui->lineEdit_2->setPlaceholderText("Szukaj...");
    }


}
void firstApp::completer(QStringList tmp){

    QCompleter *completer = new QCompleter(this);
    QStringListModel *model;

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(completer->PopupCompletion);

    ui->lineEdit_2->setCompleter(completer);
    ui->lineEdit_2->completer()->complete(QRect(0,4,completer->widget()->width(),completer->widget()->height()));

    model = (QStringListModel*)(completer->model());
    if(model==NULL){
        model = new QStringListModel();
    }
    model->setStringList(tmp);
    completer->setModel(model);
    completer->popup()->setStyleSheet("background-color: lightyellow; color:blue; border-radius: 10pt; ");

}

void firstApp::sendKeyEvent(){

QLineEdit *Edit = (QLineEdit *) sender();
QString lineVal = Edit->text();

auto key = sender()->property(keyPropKey);
if (key.isValid()){
    QKeyEvent event(QEvent::KeyRelease, key.toInt(), Qt::NoModifier);
    QApplication::sendEvent(this, &event);
    searchData("'"+lineVal.toStdString()+"%'", 2, 1,false);
}
}

void firstApp::sendKeyEventPacjenci(){

QLineEdit *Edit = (QLineEdit *)sender();
QString lineVal = Edit->text();
int tryb_filtracji = ui->filtr_pacjenci->itemData(ui->filtr_pacjenci->currentIndex()).toInt();


auto key = sender()->property(keyPropKey);
if (key.isValid()){

    QKeyEvent event(QEvent::KeyRelease, key.toInt(), Qt::NoModifier);
    QApplication::sendEvent(this, &event);

    ui->lineEdit_2->setEnabled(1);
    if (lineVal.isEmpty()){
        ui->tableWidget_2->setRowCount(0);
        ui->badania->setRowCount(0);
        ui->lekarze->setRowCount(0);

    }
    else if (lineVal.length()>=3 || tryb_filtracji==4){
            connector(tryb_filtracji);
            connect(ui->tableWidget_2->selectionModel(), &QItemSelectionModel::selectionChanged, this , &firstApp::sendClickEvent);

    }
   if (lineVal.length()==1){
       if (sprawdz_pierwsza_litere!=lineVal){
       fillCompleter(tryb_filtracji,lineVal);
   }
       sprawdz_pierwsza_litere = lineVal;
   }
}
}
void firstApp::sendClickEvent(){

    int row = ui->tableWidget_2->rowCount();
    int le = ui->lineEdit_2->text().length();
    if (row>0 || le>0){
        int i = ui->tableWidget_2->selectionModel()->currentIndex().row();
        int s = ui->tableWidget_2->item(i,0)->text().toInt();

        searchData("", 4, 3, s);
        searchData("", 4, 4, s);
}
    ui->tableWidget_2->clearSelection();
}

void firstApp::searchData(std::string tekst_wyszukiwarka, int rodzaj_filtru, int numer_tabeli, int id_wyszukiwarka)
{
   std::string sql;
   std::string sqlBadania;
   std::string sqlLekarze;
   std::string wybor_from;
   //QList actuallySelectedRow = ui->tableWidget_2->selectedItems();
   //int s = ui->tableWidget_2->currentRow();
   //std::cout<<"WIERSZ:"<<"\t"<<s<<std::endl;


   //std::cout<<s<<std::endl;
   switch(rodzaj_filtru){
          //case 0:
               //sql = ("SELECT ID,Imie,Nazwisko,Pesel FROM Pacjenci;");
               //ui->lineEdit_2->setDisabled(1);
               //break;
          case 1:
            wybor_from = "Imie";
           break;
          case 2:
            wybor_from = "Nazwisko";
            break;
          case 3:
            wybor_from = "Pesel";
           break;
          case 4:
             wybor_from = "ID";
            break;
   }

   if (rodzaj_filtru!=4){
   sql = ("SELECT * FROM Pacjenci WHERE "+wybor_from+" LIKE "+ tekst_wyszukiwarka+";");
   } else {
       sql = ("SELECT * FROM Pacjenci WHERE "+wybor_from+"="+std::to_string(id_wyszukiwarka)+";");
       sqlBadania = ("SELECT Pacjenci.ID, Badania.Termin, Badania.Id_lekarz, Badania.Opis FROM Pacjenci JOIN Badania ON Pacjenci.ID = Badania.ID_pacjent WHERE Pacjenci.ID ="+std::to_string(id_wyszukiwarka)+";");
       sqlLekarze = ("SELECT DISTINCT Lekarze.ID, Lekarze.Imie, Lekarze.Nazwisko, Lekarze.Telefon, Lekarze.Room, Lekarze.HoursWorking, Lekarze.Available, Lekarze.Urlop FROM Lekarze JOIN Badania ON Lekarze.ID = Badania.Id_lekarz WHERE Badania.ID_pacjent = "+std::to_string(id_wyszukiwarka)+";");
   }
   const char * sql_converted_to_char = sql.c_str();
   const char * sql_converted_to_char_badania = sqlBadania.c_str();
   const char * sql_converted_to_char_lekarze = sqlLekarze.c_str();
   switch(numer_tabeli){
           case 1:
                testConnect(sql_converted_to_char);
                break;
           case 2:
                fillTablePacjenci(sql_converted_to_char);
                break;
           case 3:
                fillTableBadania(sql_converted_to_char_badania);
                break;

           case 4:
                 fillTableLekarze(sql_converted_to_char_lekarze);
                break;


   }

   }

int firstApp:: connector(int index){
        switch(index){
         //case 0:
            //searchData("",0,2);
            //break;
         case 1:
            ui->lineEdit_2->setDisabled(0);
            searchData("'"+ui->lineEdit_2->text().toStdString()+"%'",1,2,false);
            break;
         case 2:
            ui->lineEdit_2->setDisabled(0);
            searchData("'"+ui->lineEdit_2->text().toStdString()+"%'",2,2,false);
            break;
         case 3:
            ui->lineEdit_2->setDisabled(0);
            searchData("'"+ui->lineEdit_2->text().toStdString()+"%'",3,2,false);
            break;
         case 4:
            ui->lineEdit_2->setDisabled(0);
            searchData("",4,2,ui->lineEdit_2->text().toInt());
            searchData("",4,3,ui->lineEdit_2->text().toInt());
            searchData("",4,4,ui->lineEdit_2->text().toInt());
            break;
        }
      return 0;
}

void firstApp::fillTablePacjenci(const char *zapytanie)
{

        sqlite3 *db;
        sqlite3_stmt *stmt;
        ui->tableWidget_2->setRowCount(0);
        int i = 0;
        int x = 0;

        sqlite3_open("/home/programista3/sqlite/test.db", &db);

        if (db == NULL)
        {
            printf("Failed to open DB\n");

        }

        sqlite3_prepare_v2(db, zapytanie, -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW) {

            int num_cols = sqlite3_column_count(stmt);


                for (i = 0; i < num_cols; i++){
                    if (i==0){
                    x = ui->tableWidget_2->rowCount();
                    ui->tableWidget_2->insertRow(x);
                    const unsigned char* result = sqlite3_column_text(stmt, i);
                    std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                    QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                    ui->tableWidget_2->setItem(x,i,ID);
                    ui->tableWidget_2->item(x,i)->setText(test.c_str());
                    } else {
                    const unsigned char* result = sqlite3_column_text(stmt, i);

                    std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                    QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                    ui->tableWidget_2->setItem(x,i,ID);
                    ui->tableWidget_2->item(x,i)->setText(test.c_str());
                    }

                }
        }


        sqlite3_finalize(stmt);
        sqlite3_close(db);
}

void firstApp::fillTableBadania(const char *zapytanie){
    sqlite3 *db;
    sqlite3_stmt *stmt;
    ui->badania->setRowCount(0);


    int i = 0;
    int x = 0;

    sqlite3_open("/home/programista3/sqlite/test.db", &db);

    if (db == NULL)
    {
        printf("Failed to open DB\n");

    }

    sqlite3_prepare_v2(db, zapytanie, -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int num_cols = sqlite3_column_count(stmt);


            for (i = 0; i < num_cols; i++){
                if (i==0){
                x = ui->badania->rowCount();
                ui->badania->insertRow(x);
                const unsigned char* result = sqlite3_column_text(stmt, i);
                std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                ui->badania->setItem(x,i,ID);
                ui->badania->item(x,i)->setText(test.c_str());


                } else {
                const unsigned char* result = sqlite3_column_text(stmt, i);

                std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                ui->badania->setItem(x,i,ID);
                ui->badania->item(x,i)->setText(test.c_str());

                }

            }
    }


    sqlite3_finalize(stmt);
    sqlite3_close(db);

}

void firstApp::fillTableLekarze(const char *zapytanie){

    sqlite3 *db;
    sqlite3_stmt *stmt;
    ui->lekarze->setRowCount(0);
    int i = 0;
    int x = 0;

    sqlite3_open("/home/programista3/sqlite/test.db", &db);

    if (db == NULL)
    {
        printf("Failed to open DB\n");

    }

    sqlite3_prepare_v2(db, zapytanie, -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int num_cols = sqlite3_column_count(stmt);


            for (i = 0; i < num_cols; i++){
                if (i==0){
                x = ui->lekarze->rowCount();
                ui->lekarze->insertRow(x);
                const unsigned char* result = sqlite3_column_text(stmt, i);
                std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                ui->lekarze->setItem(x,i,ID);
                ui->lekarze->item(x,i)->setText(test.c_str());
                } else {
                const unsigned char* result = sqlite3_column_text(stmt, i);

                std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                ui->lekarze->setItem(x,i,ID);
                ui->lekarze->item(x,i)->setText(test.c_str());
                }

            }
    }


    sqlite3_finalize(stmt);
    sqlite3_close(db);

}

void firstApp::fillCompleter(int filter, QString tekst)
{

        sqlite3 *db;
        sqlite3_stmt *stmt;
        QStringList tmp;
        int i = 0;
        std::string zapytanie;
        std::string t = "'"+tekst.toStdString()+"%'";
        std::string wybor_from;



        sqlite3_open("/home/programista3/sqlite/test.db", &db);

        if (db == NULL)
        {
            printf("Failed to open DB\n");

        }
        switch(filter){
               //case 0:
                    //sql = ("SELECT ID,Imie,Nazwisko,Pesel FROM Pacjenci;");
                    //break;
               case 1:
                wybor_from = "Imie";
                break;
               case 2:
                 wybor_from = "Nazwisko";
                 break;
               case 3:
                wybor_from = "Pesel";
                break;
               case 4:
                wybor_from = "ID";
                break;
                             }


        zapytanie = ("SELECT ID,Imie,Nazwisko,Pesel FROM Pacjenci WHERE "+ wybor_from +" LIKE "+ t+";");
        const char * sql_converted_to_char = zapytanie.c_str();


        sqlite3_prepare_v2(db,sql_converted_to_char, -1, &stmt, NULL);


        while (sqlite3_step(stmt) == SQLITE_ROW) {

            int num_cols = sqlite3_column_count(stmt);
            for (i = 0; i < num_cols; i++){
                   if (i==filter){
                    const unsigned char* result = sqlite3_column_text(stmt, i);
                    std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));

                    tmp<<test.c_str();
                    }
                }
}
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        completer(tmp);
}
void firstApp::testConnect(const char *zapytanie)
{

        sqlite3 *db;
        sqlite3_stmt *stmt;
        ui->tableWidget->setRowCount(0);
        int x = 0;
        int i = 0;


        sqlite3_open("/home/programista3/sqlite/test.db", &db);

        if (db == NULL)
        {
            printf("Failed to open DB\n");

        }

        sqlite3_prepare_v2(db, zapytanie, -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW) {

            int num_cols = sqlite3_column_count(stmt);

                for (i = 0; i < num_cols; i++){
                    if (i==0){
                    x = ui->tableWidget->rowCount();
                    ui->tableWidget->insertRow(x);
                    const unsigned char* result = sqlite3_column_text(stmt, i);
                    std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                    QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                    ui->tableWidget->setItem(x,i,ID);
                    } else {
                    const unsigned char* result = sqlite3_column_text(stmt, i);

                    std::string test = std::string(result, result + std::strlen(reinterpret_cast<const char*>(result)));
                    QTableWidgetItem* ID = new QTableWidgetItem(test.c_str());
                    ui->tableWidget->setItem(x,i,ID);
                    }
                }
            printf("\n");

        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

}
