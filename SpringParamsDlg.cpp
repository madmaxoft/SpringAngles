#include "SpringParamsDlg.hpp"
#include "ui_SpringParamsDlg.h"





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
	mUI->eForce->setText(QString::number(aForce));
}





SpringParamsDlg::~SpringParamsDlg()
{
	// Nothing explicit needed yet
}





double SpringParamsDlg::idealLength() const
{
	return mUI->eLength->text().toDouble();
}





double SpringParamsDlg::force() const
{
	return mUI->eForce->text().toDouble();
}
