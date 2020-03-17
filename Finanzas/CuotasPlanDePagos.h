#pragma once

#include <QObject>

#include <QDate>
#include <QDateTime>


enum class DueValidationError {
	SERVER_SIDE_ERROR,
	NO_ERROR
};

class CuotasPlanDePagos : public QObject {
	Q_OBJECT

public:
	CuotasPlanDePagos(QObject *parent);
	~CuotasPlanDePagos();

	void setID (int id);
	void setDueNumber (int dueNumber);
	void setDueDate (QDate date);
	void setTotal (double ammount);
	void setCapital (double ammount);
	void setInterest (double ammount);
	void setIva (double ammount);
	void setParentID (int parent);

	int getID ();
	int getDueNumber ();
	QDate getDueDate ();
	double getTotal ();
	double getCapital ();
	double getInterest ();
	double getIva ();
	int getParentID ();

	void save (QString targetUrl, QString token);
	void update (QString targetUrl, QString token);

	static bool deleteRes (QString targetURL, QString token, int id, QObject *parent= nullptr);

signals:
	void notifyValidationStatus (DueValidationError errorCode, QString errorMessage = "");

private:
	int id;
	int dueNumber;
	QDate dueDate;
	double totalAmmount;
	double capitalDue;
	double interestDue;
	double ivaDue;

	int parent_ID;
};
