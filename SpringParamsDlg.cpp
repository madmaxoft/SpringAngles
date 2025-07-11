#include "SpringParamsDlg.hpp"
#include "ui_SpringParamsDlg.h"
#include "SpringNet.hpp"





std::optional<SpringParams> SpringParamsDlg::ask(QWidget * aParent, double aIdealLength, double aForce)
{
	SpringParamsDlg dlg(aParent, aIdealLength, aForce);
	if (dlg.exec() == QDialog::Rejected)
	{
		return std::nullopt;
	}
	SpringParams sp;
	sp.mIdealLength = dlg.idealLength();
	sp.mForce = dlg.force();
	return sp;
}





SpringParamsDlg::SpringParamsDlg(QWidget * aParent, double aIdealLength, double aForce):
	Super(aParent),
	mUI(new Ui::SpringParamsDlg)
{
	mUI->setupUi(this);
	mUI->eLength->setText(QString::number(aIdealLength));
	mUI->eVerticalDifference->setText("0");
	mUI->eForce->setText(QString::number(aForce));
	mUI->eLength->selectAll();
}





SpringParamsDlg::~SpringParamsDlg()
{
	// Nothing explicit needed yet
}





double SpringParamsDlg::idealLength() const
{
	auto measuredLength = mUI->eLength->text().toDouble();
	auto verticalDiff = mUI->eVerticalDifference->text().toDouble();
	return Spring::projectLengthToFloor(measuredLength, verticalDiff);
}





double SpringParamsDlg::force() const
{
	return mUI->eForce->text().toDouble();
}
