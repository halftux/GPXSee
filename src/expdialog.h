#ifndef EXPDIALOG_H
#define EXPDIALOG_H

#include <QDialog>
#include <QPrinter>
#include "margins.h"
#include "units.h"

struct Export {
	QString fileName;
	QPrinter::PaperSize paperSize;
	QPrinter::Orientation orientation;
	MarginsF margins;
	Units units;
};

namespace Ui {
class expDialog;
}

class expDialog : public QDialog
{
	Q_OBJECT

public:
	explicit expDialog(Export *exp, QWidget *parent = 0);
	~expDialog();

public slots:
	void accept();

private:
	bool checkFile();
	Export *_export;
	Ui::expDialog *ui;
};

#endif // EXPDIALOG_H
