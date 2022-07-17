#ifndef TCNOARROWCOMBOBOX_H
#define TCNOARROWCOMBOBOX_H

#include <QObject>
#include <QComboBox>
#include <QPainter>
#include <QComboBox>
#include <QStyleOptionComboBox>

///////////////////////////////////////////////////////////////////////////////////////////////////
/// This class is a custom QComboBox which does NOT display the down arrow. The down arrow takes
/// a lot of real estate when you're trying to make them narrow. So much real estate that you can't
/// see short lines of text such as "CH 1" without the digit cut off. The only thing that this
/// custom widget does is to override the paint function. The new paint function draws the combo
/// box (using all style sheet info) without the down arrow.
///////////////////////////////////////////////////////////////////////////////////////////////////

class tcNoArrowComboBox : public QComboBox
{
	Q_OBJECT
public:
	tcNoArrowComboBox(QWidget *parent) : QComboBox(parent) {}
	void paintEvent(QPaintEvent *) override
	{
		QPainter p;
		p.begin(this);
		QStyleOptionComboBox opt;
		opt.initFrom(this);
		style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
		style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, &p, this);
		style()->drawItemText(&p, rect(), Qt::AlignCenter, palette(), isEnabled(), currentText());
		p.end();
	}
};

#endif // TCNOARROWCOMBOBOX_H
