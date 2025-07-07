#include "SpringParamsDlg.hpp"
#include "ui_SpringParamsDlg.h"





std::optional<SpringParams> SpringParamsDlg::ask(QWidget * aParent, bool aIsFixedLength, double aLength)
{
	SpringParamsDlg dlg(aParent, aIsFixedLength, aLength);
	if (dlg.exec() == QDialog::Rejected)
	{
		return std::nullopt;
	}
	SpringParams sp;
	sp.mLength = dlg.length();
	sp.mForce = dlg.force();
	return sp;
}





SpringParamsDlg::SpringParamsDlg(QWidget * aParent, bool aIsFixedLength, double aLength):
	Super(aParent),
	mUI(new Ui::SpringParamsDlg)
{
	mUI->setupUi(this);
	mUI->eLength->setEnabled(!aIsFixedLength);
	mUI->eLength->setText(QString::number(aLength));
	mUI->eForce->setText("1");
}





SpringParamsDlg::~SpringParamsDlg()
{
	// Nothing explicit needed yet
}





double SpringParamsDlg::length() const
{
	return mUI->eLength->text().toDouble();
}





double SpringParamsDlg::force() const
{
	return mUI->eForce->text().toDouble();
}
