#include "PointCoordsDlg.hpp"
#include "ui_PointCoordsDlg.h"





std::optional<QPointF> PointCoordsDlg::ask(
	QWidget * aParent,
	QPointF aInitialCoords
)
{
	PointCoordsDlg dlg(aParent, aInitialCoords);
	if (dlg.exec() == QDialog::Rejected)
	{
		return std::nullopt;
	}
	else
	{
		return dlg.coords();
	}
}





PointCoordsDlg::PointCoordsDlg(QWidget * aParent, QPointF aInitialCoords):
	Super(aParent),
	mUI(new Ui::PointCoordsDlg)
{
	mUI->setupUi(this);
	mUI->eX->setText(QString::number(aInitialCoords.x()));
	mUI->eY->setText(QString::number(aInitialCoords.y()));
}





PointCoordsDlg::~PointCoordsDlg()
{
	// Nothing explicit needed yet
}





QPointF PointCoordsDlg::coords() const
{
	auto x = mUI->eX->text().toDouble();
	auto y = mUI->eY->text().toDouble();
	return {x, y};
}
