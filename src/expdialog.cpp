#include <QFileInfo>
#include <QMessageBox>
#include "units.h"
#include "expdialog.h"
#include "ui_expdialog.h"

expDialog::expDialog(Export *exp, QWidget *parent) :
	QDialog(parent), _export(exp),
	ui(new Ui::expDialog)
{
	ui->setupUi(this);
#ifdef Q_WS_MAEMO_5
	this->setAttribute(Qt::WA_Maemo5StackedWindow);
	this->setWindowFlags(Qt::Window);
#endif
	this->showFullScreen();
	int index;

	//ui->widget_fileselect = new FileSelectWidget();
	ui->widget_fileselect->setFilter(tr("PDF files (*.pdf);;All files (*)"));
	ui->widget_fileselect->setFile(_export->fileName);


	ui->comboBox_pagesize->addItem("A3", QPrinter::A3);
	ui->comboBox_pagesize->addItem("A4", QPrinter::A4);
	ui->comboBox_pagesize->addItem("A5", QPrinter::A5);
	ui->comboBox_pagesize->addItem("Tabloid", QPrinter::Tabloid);
	ui->comboBox_pagesize->addItem("Legal", QPrinter::Legal);
	ui->comboBox_pagesize->addItem("Letter", QPrinter::Letter);
	if ((index = ui->comboBox_pagesize->findData(_export->paperSize)) >= 0)
		ui->comboBox_pagesize->setCurrentIndex(index);

	ui->comboBox_resolution->addItem("300 DPI", 300);
	ui->comboBox_resolution->addItem("600 DPI", 600);
	ui->comboBox_resolution->addItem("1200 DPI", 1200);
	if ((index = ui->comboBox_resolution->findData(_export->resolution)) >= 0)
		ui->comboBox_resolution->setCurrentIndex(index);

	if (_export->orientation == QPrinter::Portrait)
		ui->radioButton_portrait->setChecked(true);
	else
		ui->radioButton_landsape->setChecked(true);

	QString us = (_export->units == Imperial) ? tr("in") : tr("mm");
	ui->doubleSpinBox_topmargin->setSuffix(UNIT_SPACE + us);
	ui->doubleSpinBox_bottommargin->setSuffix(UNIT_SPACE + us);
	ui->doubleSpinBox_leftmargin->setSuffix(UNIT_SPACE + us);
	ui->doubleSpinBox_rightmargin->setSuffix(UNIT_SPACE + us);
	if (_export->units == Imperial) {
		ui->doubleSpinBox_topmargin->setValue(_export->margins.top() * MM2IN);
		ui->doubleSpinBox_bottommargin->setValue(_export->margins.bottom() * MM2IN);
		ui->doubleSpinBox_leftmargin->setValue(_export->margins.left() * MM2IN);
		ui->doubleSpinBox_rightmargin->setValue(_export->margins.right() * MM2IN);
		ui->doubleSpinBox_topmargin->setSingleStep(0.1);
		ui->doubleSpinBox_bottommargin->setSingleStep(0.1);
		ui->doubleSpinBox_leftmargin->setSingleStep(0.1);
		ui->doubleSpinBox_rightmargin->setSingleStep(0.1);
	} else {
		ui->doubleSpinBox_topmargin->setValue(_export->margins.top());
		ui->doubleSpinBox_bottommargin->setValue(_export->margins.bottom());
		ui->doubleSpinBox_leftmargin->setValue(_export->margins.left());
		ui->doubleSpinBox_rightmargin->setValue(_export->margins.right());
	}
}

bool expDialog::checkFile()
{
	if (ui->widget_fileselect->file().isEmpty()) {
		QMessageBox::warning(this, tr("Error"), tr("No output file selected."));
		return false;
	}

	QFile file(ui->widget_fileselect->file());
	QFileInfo fi(file);
	bool exists = fi.exists();
	bool opened = false;

	if (exists && fi.isDir()) {
		QMessageBox::warning(this, tr("Error"), tr("%1 is a directory.")
		  .arg(file.fileName()));
		return false;
	} else if ((exists && !fi.isWritable())
	  || !(opened = file.open(QFile::Append))) {
		QMessageBox::warning(this, tr("Error"), tr("%1 is not writable.")
		  .arg(file.fileName()));
		return false;
	}
	if (opened) {
		file.close();
		if (!exists)
			file.remove();
	}

	return true;
}

void expDialog::accept()
{
	if (!checkFile())
		return;

	QPrinter::Orientation orientation = ui->radioButton_portrait->isChecked()
	  ? QPrinter::Portrait : QPrinter::Landscape;
	QPrinter::PaperSize paperSize = static_cast<QPrinter::PaperSize>
	  (ui->comboBox_pagesize->itemData(ui->comboBox_pagesize->currentIndex()).toInt());
	int resolution = ui->comboBox_resolution->itemData(ui->comboBox_resolution->currentIndex()).toInt();

	_export->fileName = ui->widget_fileselect->file();
	_export->paperSize = paperSize;
	_export->resolution = resolution;
	_export->orientation = orientation;
	if (_export->units == Imperial)
		_export->margins = MarginsF(ui->doubleSpinBox_leftmargin->value() / MM2IN,
		ui->doubleSpinBox_topmargin->value() / MM2IN, ui->doubleSpinBox_rightmargin->value() / MM2IN,
		ui->doubleSpinBox_bottommargin->value() / MM2IN);
	else
		_export->margins = MarginsF(ui->doubleSpinBox_leftmargin->value(), ui->doubleSpinBox_topmargin->value(),
		  ui->doubleSpinBox_rightmargin->value(), ui->doubleSpinBox_bottommargin->value());

	QDialog::accept();
}

expDialog::~expDialog()
{
	delete ui;
}
