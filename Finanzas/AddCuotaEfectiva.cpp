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

	connect (ui.pagoCapital, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);
	connect (ui.pagoInteres, &QLineEdit::returnPressed, ui.addButton, &QPushButton::click);

	connect (ui.pagoMonto, &NumberInput::valueChanged, this, &AddCuotaEfectiva::capitalAutoFill);
}

AddCuotaEfectiva::~AddCuotaEfectiva() {

}

void AddCuotaEfectiva::setWindowData (QString targetUrl, QString token, int cuota, QDate minDate, OperacionesFinancieras::TiposDeOperacion caso, OperacionesFinancieras::FrecuenciaDePagos freq, int parentID, QDate currentDate, double total, double cap, double inte, double  iva, int editingID, bool editing) {
	this->targetUrl = targetUrl;
	this->token = token;
	ui.numeroCuota->setValue (cuota);
	//ui.numeroCuota->setEnabled (false);
	ui.fechaPago->setMinimumDate (minDate);

	int addedMonths = 0;
	switch (freq) {
	case OperacionesFinancieras::FrecuenciaDePagos::Mensual:
		addedMonths = 1;
		break;
	case OperacionesFinancieras::FrecuenciaDePagos::Bimensual:
		addedMonths = 2;
		break;
	case OperacionesFinancieras::FrecuenciaDePagos::Trimestral:
		addedMonths = 3;
		break;
	case OperacionesFinancieras::FrecuenciaDePagos::Semestral:
		addedMonths = 6;
		break;
	case OperacionesFinancieras::FrecuenciaDePagos::Anual:
		addedMonths = 12;
		break;
	case OperacionesFinancieras::FrecuenciaDePagos::NONE:
		break;
	}
	ui.fechaPago->setDate (minDate.addMonths (addedMonths));
	this->caso = caso;

	// enable iva editing for Leasing and Lease back, otherwise disable it
	if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
		ui.pagoIva->setEnabled (true);
	}
	else {
		ui.pagoIva->setEnabled (false);
	}

	if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales) {
		ui.pagoCapital->setEnabled (false);
	}
	else {
		ui.pagoCapital->setEnabled (true);
	}

	// default editing = false and editingID = 0		so default is new
	this->parentID = parentID;
	this->editing = editing;
	this->editingID = editingID;

	// Set window title by editing
	setWindowTitle (QString::fromLatin1 ((this->editing ? "Editar Cuota" : "Añadir Cuota")));
	ui.label->setText (QString::fromLatin1 ((this->editing ? "Editar Cuota" : "Añadir Cuota")));
	ui.addButton->setText (QString::fromLatin1 (this->editing ? "Actualizar" : "Registrar"));

	if(editing) {
		ui.fechaPago->setDate (currentDate);
		ui.pagoMonto->setValue (total);
		ui.pagoCapital->setValue (cap);
		ui.pagoInteres->setValue (inte);
		ui.pagoIva->setValue (iva);
	}

	connect (ui.pagoCapital, &NumberInput::valueChanged, this, &AddCuotaEfectiva::onPagoCapitalChanged);

	connect (ui.pagoIva, &NumberInput::valueChanged, this, &AddCuotaEfectiva::onIvaChanged);

	//connect (ui.pagoInteres, &NumberInput::valueChanged, this, &AddCuotaEfectiva::onPagoInteresChanged);

	ui.pagoMonto->setFocus ();
}

void AddCuotaEfectiva::capitalAutoFill (double monto) {
	if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales) {
		ui.pagoCapital->setValue (monto);
	}
}

void AddCuotaEfectiva::onPagoCapitalChanged (double capital) {
	if (ui.pagoCapital->hasFocus ()) {
		if (this->caso != OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales) {
			if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
				ui.pagoIva->setValue (0.13 * capital / 0.87);
			}
			ui.pagoInteres->setValue (ui.pagoMonto->getValue () - capital - ui.pagoIva->getValue ());
		}
	}
}

void AddCuotaEfectiva::onPagoInteresChanged (double interes) {
	//ui.pagoCapital->setValue (ui.pagoMonto->getValue () - interes - ui.pagoIva->getValue ());
}

void AddCuotaEfectiva::onIvaChanged (double iva) {
	if (ui.pagoIva->hasFocus ()) {
		if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
			ui.pagoInteres->setValue (ui.pagoMonto->getValue () - ui.pagoCapital->getValue () - iva);
		}
	}
}

void AddCuotaEfectiva::onSaveClicked () {
	ui.addButton->setEnabled (false);
	//bool total_ok = false;
	//double total = ui.pagoMonto->text ().toDouble (&total_ok);
	double total = ui.pagoMonto->getValue ();
	//bool capital_ok = false;
	//double capital = ui.pagoCapital->text ().toDouble (&capital_ok);
	double capital = ui.pagoCapital->getValue ();
	//bool interes_ok = false;
	//double interes = ui.pagoInteres->text ().toDouble (&interes_ok);
	double interes = ui.pagoInteres->getValue ();
	//bool iva_ok = false;
	//double iva = ui.pagoIva->text ().toDouble (&iva_ok);
	double iva = ui.pagoIva->getValue ();

	//if (!total_ok || total <= 0) {
	if (total <= 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El monto total de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	//if (!capital_ok || capital <= 0) {
	if (capital < 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El capital de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	//if (!interes_ok || interes <= 0) {
	if (this->caso != OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales && interes <= 0 || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoImpuestosNacionales && interes < 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 ("El interés de la cuota debe ser mayor a cero"));
		ui.addButton->setEnabled (true);
		return;
	}
	if ((this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing) && iva < 0) {
		QMessageBox::critical (this, "Error", QString::fromLatin1 (""));
		ui.addButton->setEnabled (true);
		return;
	}

	double diff = abs (total - (capital + interes + iva));

	if (diff > 1e-2) {
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
		if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
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
		if (this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeasing || this->caso == OperacionesFinancieras::TiposDeOperacion::CasoLeaseBack) {
			bodyContent.insert ("pagoDeIva", ui.pagoIva->text ().toDouble ());
		}
		bodyContent.insert ("parent", this->parentID);

		body.setObject (bodyContent);
		nam->put (request, body.toJson ());
	}
}
