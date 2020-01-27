#include "Login.h"
#include <QtWidgets/QApplication>
#include <QIcon>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Login w;
	w.show();

	QIcon icon (":/Resources/img/Logo-LPL.png");
	a.setApplicationDisplayName ("Operaciones Financieras LPL");
	a.setWindowIcon (icon);

	return a.exec();
}
