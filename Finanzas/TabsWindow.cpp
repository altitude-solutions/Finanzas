#include "TabsWindow.h"



TabsWindow::TabsWindow(QWidget *parent): QMainWindow(parent) {
	ui.setupUi(this);
	// ==========================================
	// Linea de credito tab configuration
	// ==========================================
	firstTab = new QWidget (this);
	firtsTabHeader = new AppHeader(this);
	firtsTabHeader->setAppName ( QString::fromLatin1( "Líneas de Crédito") );
	lineaDeCredito = new LineaDeCredito (this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget (firtsTabHeader, 1);
	layout->addWidget (lineaDeCredito, 8);
	layout->setMargin (0);
	firstTab->setLayout (layout);
	ui.tabWidget->addTab (firstTab, QString::fromLatin1("Líneas de Crédito") );
	connect (firtsTabHeader, &AppHeader::logoutButton, this, &TabsWindow::logout);
	connect (this, &TabsWindow::lineasDeCreditoSelected, lineaDeCredito, &LineaDeCredito::onTabSelected);

	// ==========================================
	// Plan de pagos tab configuration
	// ==========================================
	secondTab = new QWidget (this);
	secondTabHeader = new AppHeader (this);
	secondTabHeader->setAppName ( QString::fromLatin1("Planes de Pago") );
	planDePagos = new PlanDePagos (this);
	QVBoxLayout *secondLayout = new QVBoxLayout (this);
	secondLayout->addWidget (secondTabHeader, 1);
	secondLayout->addWidget (planDePagos, 8);
	secondLayout->setMargin (0);
	secondTab->setLayout (secondLayout);
	ui.tabWidget->addTab (secondTab, QString::fromLatin1 ("Plan de Pagos"));
	connect (secondTabHeader, &AppHeader::logoutButton, this, &TabsWindow::logout);
	connect (this, &TabsWindow::planesDePagoSelected, planDePagos, &PlanDePagos::onTabSelected);

	// ==========================================
	// Plan de pagos tab configuration
	// ==========================================
	thirdTab = new QWidget (this);
	thirdTabHeader = new AppHeader (this);
	thirdTabHeader->setAppName ( QString::fromLatin1("Pagos Efectivos") );
	pagosEfectivos = new PagosEfectivos (this);
	QVBoxLayout *thirdLayout = new QVBoxLayout (this);
	thirdLayout->addWidget (thirdTabHeader, 1);
	thirdLayout->addWidget (pagosEfectivos, 8);
	thirdLayout->setMargin (0);
	thirdLayout->setSpacing (0); // remove space between header and body
	thirdTab->setLayout (thirdLayout);
	ui.tabWidget->addTab (thirdTab, QString::fromLatin1 ("Pagos Efectivos"));
	connect (thirdTabHeader, &AppHeader::logoutButton, this, &TabsWindow::logout);
	connect (this, &TabsWindow::pagosEfectivosSelected, pagosEfectivos, &PagosEfectivos::onTabSelected);


	this->setWindowTitle (QString::fromLatin1 ("Líneas de Crédito"));
	// ==========================================
	// Set Window title by the selected tab
	// ==========================================
	connect (ui.tabWidget, &QTabWidget::currentChanged, this, [&](int index) {
		switch (index) {
		case 0:
			// linaes de credito
			this->setWindowTitle (QString::fromLatin1("Líneas de Crédito"));
			emit lineasDeCreditoSelected ();
			break;
		case 1:
			// plan de pagos
			this->setWindowTitle (QString::fromLatin1("PLanes de Pago"));
			emit planesDePagoSelected ();
			break;
		case 2:
			// pagos efectivos
			this->setWindowTitle (QString::fromLatin1 ("Pagos Efectivos"));
			emit pagosEfectivosSelected ();
			break;
		}
	});
}

TabsWindow::~TabsWindow() {
	// Window destructor
}

void TabsWindow::onLoginSuccess (QString address, QString userName, QString realName, QString token) {
	// ==========================================
	// First tab auth data
	// ==========================================
	lineaDeCredito->setAuthData (address, token, userName);
	firtsTabHeader->setUserName (realName);
	// ==========================================
	// Second tab auth data
	// ==========================================
	planDePagos->setAuthData(address, token, userName);
	secondTabHeader->setUserName (realName);
	// ==========================================
	// Third tab auth data
	// ==========================================
	pagosEfectivos->setAuthData (address, token, userName);
	thirdTabHeader->setUserName (realName);
}