#pragma once
#include <QPlainTextEdit>
#include <QWidget>
class CodeWidget;
class LineNumbers : public QWidget {
	Q_OBJECT
private:
	CodeWidget *m_code;
protected:
	void paintEvent(QPaintEvent *event) override;
public:
	LineNumbers(CodeWidget *code);
	~LineNumbers();
	QSize sizeHint() const override;
};
class CodeWidget : public QPlainTextEdit {
	Q_OBJECT
private:
	LineNumbers *m_line_numbers;
protected:
	void resizeEvent(QResizeEvent *event) override;
public:
	CodeWidget(QWidget *parent = nullptr);
	~CodeWidget();
	void lineNumbersPaintEvent(QPaintEvent *event);
	int lineNumberWidth();
	QString getSource() const;
private slots:
	void setLineNumberWidth();
	void highlightCurrentLine();
};