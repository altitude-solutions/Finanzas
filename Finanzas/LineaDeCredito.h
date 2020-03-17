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

public slots:
	void onTabSelected ();

private slots:
	void onSaveClicked ();
	void onCancelClicked ();
	void onEditClicked ();
	void onDeleteClicked ();

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

	// setup table clipboard
	void setupTableClipboard ();


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

	// Edit flag
	bool editing;
	int editingID;
};
