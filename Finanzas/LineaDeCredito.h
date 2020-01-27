#pragma once

#include <QWidget>
#include "ui_LineaDeCredito.h"
#include <QHash>

class LineaDeCredito : public QWidget {
	Q_OBJECT

public:
	LineaDeCredito(QWidget *parent = Q_NULLPTR);
	~LineaDeCredito();
	// Auth data setter
	void setAuthData (QString address, QString token, QString userName);

private slots:
	void onSaveClicked ();

private:
	void loadEntidadesFinancieras ();
	void loadTiposDeEntidad ();
	void loadEmpresasGrupo ();
	void loadLineasDeCredito ();

	// table operations
	void setTableHeaders ();
	void refreshTable (QJsonDocument data);

	// validator functions
	void resetValidarors ();
	bool check ();

	// reset fields
	void clearFields ();


	Ui::LineaDeCredito ui;

	// Auth data
	QString token;
	QString userName;
	QString targetAddress;

	// App data
	QHash <QString, QHash<QString, QString>> listaEntidades;
	QHash <QString, QString> listaTiposEntidades;
	QHash <QString, QString> listaEmpresas;

	QHash <int, QJsonDocument> lineasDeCredito;

	// Validation flags
	bool *dataIsCorrect;
};
