#include "AddCuotaEfectiva.h"

// Network imports
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

// Json imports
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

// Other imports
#include <QMessageBox>


AddCuotaEfectiva::AddCuotaEfectiva(QWidget *parent): QDialog(parent) {
	ui.setupUi(this);
	setWindowTitle (QString::fromLatin1(("Añadir Cuota")));

	connect (ui.cancelButton, &QPushButton::clicked, this, &AddCuotaEfectiva::reject);
	connect (ui.addButton, &QPushButton::clicked, this, &AddCuotaEfectiva::onSaveClicked);

	connect (ui.pagoMonto, &QLineEdit::textChanged, this, &AddCuotaEfectiva::onMontoCuotaChanged);
	connect (ui.pagoCapital, &QLineEdit::textChanged, this, &AddCuotaEfectiva::onPagoCapitalChanged);
	connect (ui.pagoInteres, &QLineEdit::textChanged, this, &AddCuotaEfectiva::onPagoInteresChanged);

	connect (ui.pagoCapital, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);
	connect (ui.pagoInteres, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);
}

AddCuotaEfectiva::~AddCuotaEfectiva() {

}

void AddCuotaEfectiva::setWindowData (QString targetUrl, QString token, int cuota, QDate minDate, CasosPlanDePagos_enum caso, int parentID, int editingID, bool editing) {
	this->targetUrl = targetUrl;
	this->token = token;
	ui.numeroCuota->setValue (cuota);
	//ui.numeroCuota->setEnabled (false);
	ui.fechaPago->setMinimumDate (minDate);
	ui.fechaPago->setDate (minDate.addMonths (1));
	this->caso = caso;

	// default editing = false and editingID = 0		so default is new
	this->parentID = parentID;
	this->editing = editing;
	this->editingID = editingID;

	// Set window title by editing
	setWindowTitle (QString::fromLatin1 ((this->editing ? "Editar Cuota" : "Añadir Cuota")));
	ui.label->setText (QString::fromLatin1 ((this->editing ? "Editar Cuota" : "Añadir Cuota")));
	ui.addButton->setText (QString::fromLatin1 (this->editing ? "Actualizar" : "Registrar"));
}

void AddCuotaEfectiva::onMontoCuotaChanged (QString monto) {
	bool isNumber = false;
	double ammount = monto.toDouble (&isNumber);
	if (!isNumber) {
		monto.truncate (monto.length () - 1);
		ui.pagoMonto->setText (monto);
	}

	if (this->caso == CasosPlanDePagos_enum::CasoLeaseBack) {
		ui.pagoIva->setText (QString::number (0.13 * ammount, 'f', 2));
	}
}

void AddCuotaEfectiva::onPagoCapitalChanged (QString capital) {
	bool isNumber = false;
	double ammount = capital.toDouble (&isNumber);
	if (!isNumber) {
		capital.truncate (capital.length () - 1);
		ui.pagoCapital->setText (capital);
	}

	double total = ui.pagoMonto->text ().toDouble (),
		iva = ui.pagoIva->text ().toDouble ();

	ui.pagoInteres->setText (QString::number (total - ammount - iva, 'f', 2));
}

void AddCuotaEfectiva::onPagoInteresChanged (QString interes) {
	bool isNumber = false;
	double ammount = interes.toDouble (&isNumber);
	if (!isNumber) {
		interes.truncate (interes.length () - 1);
		ui.pagoInteres->setText (interes);
	}

	double total = ui.pagoMonto->text ().toDouble (),
		iva = ui.pagoIva->text ().toDouble ();

	ui.pagoCapital->setText (QString::number (total - ammount - iva, 'f', 2));
}

void AddCuotaEfectiva::onSaveClicked () {
	ui.addButton->setEnabled (false);
	bool total_ok = false;
	double total = ui.pagoMonto->text ().toDouble (&total_ok);
	bool capital_ok = false;
	double capital = ui.pagoCapital->text ().toDouble (&capital_ok);
	bool interes_ok = false;
	double interes = ui.pagoInteres->text ().toDouble (&interes_ok);
	bool iva_ok = false;
	double iva = ui.pagoIva->text ().toDouble (&iva_ok);

	if (!total_ok || total <= 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto total de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	if (!capital_ok || capital <= 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El capital de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	if (!interes_ok || interes <= 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El interés de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	if ( this->caso == CasosPlanDePagos_enum::CasoLeaseBack && (!total_ok || total <= 0)) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 (""));
		ui.addButton->setEnabled (true);
		return;
	}

	if (total != (capital + interes + iva)) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("La suma del capital, interés e iva (cuando aplica) debe ser igual al monto de la cuota"));
		ui.addButton->setEnabled (true);
		return;
	}

	if (!editing) {
		// Network manager and request
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (this->targetUrl + "/cuotaEfectiva"));
		request.setRawHeader ("Content-Type", "application/json");
		request.setRawHeader ("token", this->token.toUtf8 ());

		// On response lambda
		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument jsonReply = QJsonDocument::fromJson (reply->readAll ());
			if (reply->error ()) {
				if (jsonReply.object ().value ("err").toObject ().contains ("message")) {
					// If there is a known error
					QMessageBox::critical (this, "Error", QString::fromLatin1 (jsonReply.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
				}
				else {
					if (reply->error () == QNetworkReply::ConnectionRefusedError) {
						QMessageBox::critical (this, QString::fromLatin1 ("Error de conexión"), QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
					}
					else {
						// If there is a server error
						QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (jsonReply.toJson ().toStdString ()));
					}
				}
			}
			else {
				emit this->accept ();
				ui.addButton->setEnabled (true);
				reply->deleteLater ();
			}
		});

		// Request body
		QJsonDocument body;
		QJsonObject bodyContent;

		bodyContent.insert ("numeroDeCuota", ui.numeroCuota->value ());
		bodyContent.insert ("fechaDePago", ui.fechaPago->dateTime ().toMSecsSinceEpoch ());
		bodyContent.insert ("montoTotalDelPago", ui.pagoMonto->text ().toDouble ());
		bodyContent.insert ("pagoDeCapital", ui.pagoCapital->text ().toDouble ());
		bodyContent.insert ("pagoDeInteres", ui.pagoInteres->text ().toDouble ());
		if (ui.pagoIva->text () != "") {
			bodyContent.insert ("pagoDeIva", ui.pagoIva->text ().toDouble ());
		}
		bodyContent.insert ("parent", this->parentID);

		body.setObject (bodyContent);
		nam->post (request, body.toJson ());
	}
	else {
		// Network manager and request
		QNetworkAccessManager* nam = new QNetworkAccessManager (this);
		QNetworkRequest request;
		request.setUrl (QUrl (this->targetUrl + "/cuotaEfectiva/" + QString::number(this->editingID)));
		request.setRawHeader ("Content-Type", "application/json");
		request.setRawHeader ("token", this->token.toUtf8 ());

		// On response lambda
		connect (nam, &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
			QJsonDocument jsonReply = QJsonDocument::fromJson (reply->readAll ());
			if (reply->error ()) {
				if (jsonReply.object ().value ("err").toObject ().contains ("message")) {
					// If there is a known error
					QMessageBox::critical (this, "Error", QString::fromLatin1 (jsonReply.object ().value ("err").toObject ().value ("message").toString ().toLatin1 ()));
				}
				else {
					if (reply->error () == QNetworkReply::ConnectionRefusedError) {
						QMessageBox::critical (this, QString::fromLatin1 ("Error de conexión"), QString::fromLatin1 ("No se pudo establecer conexión con el servidor"));
					}
					else {
						// If there is a server error
						QMessageBox::critical (this, "Error en base de datos", "Por favor enviar un reporte de error con una captura de pantalla de esta venta.\n" + QString::fromStdString (jsonReply.toJson ().toStdString ()));
					}
				}
			}
			else {
				QMessageBox::information (this, QString::fromLatin1 ("Éxito"), QString::fromLatin1 ("Cuota actualizada con éxito"));
				emit this->accept ();
				ui.addButton->setEnabled (true);
				reply->deleteLater ();
			}
			});

		// Request body
		QJsonDocument body;
		QJsonObject bodyContent;

		bodyContent.insert ("numeroDeCuota", ui.numeroCuota->value ());
		bodyContent.insert ("fechaDePago", ui.fechaPago->dateTime ().toMSecsSinceEpoch ());
		bodyContent.insert ("montoTotalDelPago", ui.pagoMonto->text ().toDouble ());
		bodyContent.insert ("pagoDeCapital", ui.pagoCapital->text ().toDouble ());
		bodyContent.insert ("pagoDeInteres", ui.pagoInteres->text ().toDouble ());
		if (ui.pagoIva->text () != "") {
			bodyContent.insert ("pagoDeIva", ui.pagoIva->text ().toDouble ());
		}
		bodyContent.insert ("parent", this->parentID);

		body.setObject (bodyContent);
		nam->put (request, body.toJson ());
	}
}
