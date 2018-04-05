#include "optdialog.h"
#include "ui_optdialog.h"
#include <QColorDialog>

optDialog::optDialog(Options *options, QWidget *parent) :
	QDialog(parent), _options(options),
	ui(new Ui::optDialog)
{
	ui->setupUi(this);
#ifdef Q_WS_MAEMO_5
	this->setAttribute(Qt::WA_Maemo5StackedWindow);
	this->setWindowFlags(Qt::Window);
#endif
	this->showFullScreen();
	QString buttoncolor;
	ui->horizontalSlider_mapopacity->setValue(_options->mapOpacity);
	buttoncolor = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(_options->backgroundColor.name());
	_backgroundColor=_options->backgroundColor;
	ui->pushButton_bcolor_map->setStyleSheet(buttoncolor);
	ui->spinBox_size_waypoints->setMinimum(1);
	ui->spinBox_size_waypoints->setValue(_options->waypointSize);
	buttoncolor = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(_options->waypointColor.name());
	_waypointColor=_options->waypointColor;
	ui->pushButton_color_waypoints->setStyleSheet(buttoncolor);
	ui->spinBox_size_pois->setMinimum(1);
	ui->spinBox_size_pois->setValue(_options->poiSize);
	buttoncolor = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(_options->poiColor.name());
	_poiColor = _options->poiColor;
	ui->pushButton_color_pois->setStyleSheet(buttoncolor);
	_baseColor=_options->palette.color();
	buttoncolor = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(_options->palette.color().name());
	ui->pushButton_color_base->setStyleSheet(buttoncolor);
	ui->spinBox_path_trwidth->setValue(_options->trackWidth);
	ui->spinBox_path_trwidth->setMinimum(1);
	ui->comboBox_path_trstyle->setValue(_options->trackStyle);
	ui->spinBox_path_rwidth->setValue(_options->routeWidth);
	ui->spinBox_path_rwidth->setMinimum(1);
	ui->comboBox_path_rstyle->setValue(_options->routeStyle);
	ui->checkBox_path_antia->setChecked(_options->pathAntiAliasing);
	ui->spinBox_graph_lwidth->setValue(_options->graphWidth);
	ui->spinBox_graph_lwidth->setMinimum(1);
	ui->checkBox_graph_antia->setChecked(_options->graphAntiAliasing);
	ui->doubleSpinBox_poi_radius->setSingleStep(1);
	ui->doubleSpinBox_poi_radius->setDecimals(1);
	if (_options->units == Imperial) {
		ui->doubleSpinBox_poi_radius->setValue(_options->poiRadius / MIINM);
		ui->doubleSpinBox_poi_radius->setSuffix(UNIT_SPACE + tr("mi"));
	} else {
		ui->doubleSpinBox_poi_radius->setValue(_options->poiRadius / KMINM);
		ui->doubleSpinBox_poi_radius->setSuffix(UNIT_SPACE + tr("km"));
	}
	ui->checkBox_system_opengl->setChecked(_options->useOpenGL);
	ui->doubleSpinBox_color_pshift->setMinimum(0);
	ui->doubleSpinBox_color_pshift->setMaximum(1.0);
	ui->doubleSpinBox_color_pshift->setSingleStep(0.01);
	ui->doubleSpinBox_color_pshift->setValue(_options->palette.shift());

	ui->checkBox_exp_name->setChecked(_options->printName);
	ui->checkBox_exp_date->setChecked(_options->printDate);
	ui->checkBox_exp_distance->setChecked(_options->printDistance);
	ui->checkBox_exp_time->setChecked(_options->printTime);
	ui->checkBox_exp_movingtime->setChecked(_options->printMovingTime);
	ui->checkBox_exp_itemcount->setChecked(_options->printItemCount);
	ui->checkBox_exp_separate->setChecked(_options->separateGraphPage);

	QString filterToolTip = tr("Moving average window size");

	ui->spinBox_elevationFilter->setValue(_options->elevationFilter);
	ui->spinBox_elevationFilter->setToolTip(filterToolTip);
	ui->spinBox_speedFilter->setValue(_options->speedFilter);
	ui->spinBox_speedFilter->setToolTip(filterToolTip);
	ui->spinBox_heartRateFilter->setValue(_options->heartRateFilter);
	ui->spinBox_heartRateFilter->setToolTip(filterToolTip);
	ui->spinBox_cadenceFilter->setValue(_options->cadenceFilter);
	ui->spinBox_cadenceFilter->setToolTip(filterToolTip);
	ui->spinBox_powerFilter->setValue(_options->powerFilter);
	ui->spinBox_powerFilter->setToolTip(filterToolTip);

	ui->checkBox_outlierEliminate->setChecked(_options->outlierEliminate);

	ui->doubleSpinBox_pauseSpeed->setDecimals(1);
	ui->doubleSpinBox_pauseSpeed->setSingleStep(0.1);
	ui->doubleSpinBox_pauseSpeed->setMinimum(0.1);
	if (_options->units == Imperial) {
		ui->doubleSpinBox_pauseSpeed->setValue(_options->pauseSpeed * MS2MIH);
		ui->doubleSpinBox_pauseSpeed->setSuffix(UNIT_SPACE + tr("mi/h"));
	} else {
		ui->doubleSpinBox_pauseSpeed->setValue(_options->pauseSpeed * MS2KMH);
		ui->doubleSpinBox_pauseSpeed->setSuffix(UNIT_SPACE + tr("km/h"));
	}
	ui->spinBox_pauseInterval->setMinimum(1);
	ui->spinBox_pauseInterval->setSuffix(UNIT_SPACE + tr("s"));
	ui->spinBox_pauseInterval->setValue(_options->pauseInterval);
	ui->spinBox_pixmapCache->setMinimum(16);
	ui->spinBox_pixmapCache->setMaximum(1024);
	ui->spinBox_pixmapCache->setSuffix(UNIT_SPACE + tr("MB"));
	ui->spinBox_pixmapCache->setValue(_options->pixmapCache);

	if (_options->hiresPrint)
		ui->radioButton_print_highres->setChecked(true);
	else
		ui->radioButton_print_wysiwyg->setChecked(true);

}

optDialog::~optDialog()
{
	delete ui;
}
void optDialog::accept()
{
	_options->palette.setColor(_baseColor);
	_options->mapOpacity = ui->horizontalSlider_mapopacity->value();
	_options->backgroundColor=_backgroundColor;
	_options->waypointSize=ui->spinBox_size_waypoints->value();
	_options->waypointColor=_waypointColor;
	_options->poiSize=ui->spinBox_size_pois->value();
	_options->poiColor=_poiColor;

	_options->palette.setShift(ui->doubleSpinBox_color_pshift->value());
	_options->trackWidth = ui->spinBox_path_trwidth->value();
	_options->trackStyle = (Qt::PenStyle) ui->comboBox_path_trstyle->itemData(
	  ui->comboBox_path_trstyle->currentIndex()).toInt();
	_options->routeWidth = ui->spinBox_path_rwidth->value();
	_options->routeStyle = (Qt::PenStyle) ui->comboBox_path_rstyle->itemData(
	  ui->comboBox_path_rstyle->currentIndex()).toInt();
	_options->pathAntiAliasing = ui->checkBox_path_antia->isChecked();
	_options->graphWidth = ui->spinBox_graph_lwidth->value();
	_options->graphAntiAliasing = ui->checkBox_graph_antia->isChecked();

	_options->elevationFilter = ui->spinBox_elevationFilter->value();
	_options->speedFilter = ui->spinBox_speedFilter->value();
	_options->heartRateFilter = ui->spinBox_heartRateFilter->value();
	_options->cadenceFilter = ui->spinBox_cadenceFilter->value();
	_options->powerFilter = ui->spinBox_powerFilter->value();
	_options->outlierEliminate = ui->checkBox_outlierEliminate->isChecked();
	_options->pauseSpeed = (_options->units == Imperial)
		? ui->doubleSpinBox_pauseSpeed->value() / MS2MIH : ui->doubleSpinBox_pauseSpeed->value() / MS2KMH;
	_options->pauseInterval = ui->spinBox_pauseInterval->value();

	/*if (_options->units == Imperial)
		_options->poiRadius = ui->doubleSpinBox_poi_radius->value() * MIINM;
	else
		_options->poiRadius = ui->doubleSpinBox_poi_radius->value() * KMINM;*/
	_options->poiRadius = (_options->units == Imperial)
		? ui->doubleSpinBox_poi_radius->value() * MIINM :  ui->doubleSpinBox_poi_radius->value() * KMINM;

	_options->useOpenGL = ui->checkBox_system_opengl->isChecked();
	_options->pixmapCache = ui->spinBox_pixmapCache->value();

	_options->printName = ui->checkBox_exp_name->isChecked();
	_options->printDate = ui->checkBox_exp_date->isChecked();
	_options->printDistance = ui->checkBox_exp_distance->isChecked();
	_options->printTime = ui->checkBox_exp_time->isChecked();
	_options->printMovingTime = ui->checkBox_exp_movingtime->isChecked();
	_options->printItemCount = ui->checkBox_exp_itemcount->isChecked();
	_options->separateGraphPage = ui->checkBox_exp_separate->isChecked();
	_options->hiresPrint = ui->radioButton_print_highres->isChecked();

	QDialog::accept();
}

void optDialog::on_pushButton_color_base_clicked()
{
	QColorDialog cdialog;
			cdialog.setOption( QColorDialog::ShowAlphaChannel, false );
		QColor color = cdialog.getColor( _baseColor, this );

	//QColor color = QColorDialog::getColor(_baseColor, this, QString(), QColorDialog::ShowAlphaChannel);
	if (color.isValid()) {
		QString bgc = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(color.name());
		ui->pushButton_color_base->setStyleSheet(bgc);
		_baseColor=color;
		//emit colorChanged(_color);
	}
}

void optDialog::on_pushButton_color_waypoints_clicked()
{
	QColorDialog cdialog;
	cdialog.setOption( QColorDialog::ShowAlphaChannel, false );
	QColor color = cdialog.getColor( _waypointColor, this );

	if (color.isValid())
	{
		QString bgc = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(color.name());
		ui->pushButton_color_waypoints->setStyleSheet(bgc);
		_waypointColor=color;
	}
}

void optDialog::on_pushButton_color_pois_clicked()
{
	QColorDialog cdialog;
	cdialog.setOption( QColorDialog::ShowAlphaChannel, false );
	QColor color = cdialog.getColor( _poiColor, this );

	if (color.isValid())
	{
		QString bgc = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(color.name());
		ui->pushButton_color_pois->setStyleSheet(bgc);
		_poiColor=color;
	}
}

void optDialog::on_pushButton_bcolor_map_clicked()
{
	QColorDialog cdialog;
	cdialog.setOption( QColorDialog::ShowAlphaChannel, false );
	QColor color = cdialog.getColor( _backgroundColor, this );

	if (color.isValid())
	{
		QString bgc = QString("QPushButton { background-color: %1; border: 1px solid white; }").arg(color.name());
		ui->pushButton_bcolor_map->setStyleSheet(bgc);
		_backgroundColor=color;
	}
}
