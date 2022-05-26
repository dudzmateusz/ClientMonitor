#ifndef FIRSTAPP_H
#define FIRSTAPP_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class firstApp; }
QT_END_NAMESPACE

class firstApp : public QMainWindow
{
    Q_OBJECT

public:
    firstApp(QWidget *parent = nullptr);
    ~firstApp();

private:
    Ui::firstApp *ui;


protected slots:
    void sendKeyEvent();
    void sendKeyEventPacjenci();
    void sendClickEvent();
    void testConnect(const char *zapytanie);
    void fillTablePacjenci(const char *zapytanie);
    void fillTableBadania(const char *zapytanie);
    void fillTableLekarze(const char *zapytanie);
    void searchData(std::string tekst_wyszukiwarka, int rodzaj_filtru, int numer_tabeli, int id_wyszukiwarka);
    int connector(int val_filter);
    void completer(QStringList l);
    void fillCompleter(int rodzaj_filtru, QString tekst);
    void filter_managment(int i);


};
#endif // FIRSTAPP_H
