#include "CodeWidget.hpp"
LineNumbers::LineNumbers(CodeWidget *code) : QWidget(code), m_code(code) {}
LineNumbers::~LineNumbers() {}
void LineNumbers::paintEvent(QPaintEvent * event) {
	m_code->lineNumbersPaintEvent(event);
}
QSize LineNumbers::sizeHint() const {
	return QSize(m_code->lineNumberWidth(), 0);
}
CodeWidget::CodeWidget(QWidget *parent)	: QPlainTextEdit(parent) {
	m_line_numbers = new LineNumbers(this);

	connect(this->document(), &QTextDocument::blockCountChanged, this, &CodeWidget::setLineNumberWidth);
	connect(this, &CodeWidget::updateRequest, [this](QRect const& rect, int dy) {
		if (dy) m_line_numbers->scroll(0, dy);
		else m_line_numbers->update(0, rect.y(), m_line_numbers->width(), rect.height());
		if (rect.contains(viewport()->rect())) setLineNumberWidth();
	});
	connect(this, &CodeWidget::cursorPositionChanged, this, &CodeWidget::highlightCurrentLine);

	setFont(QFont("Consolas", 18));

	setLineNumberWidth();
	highlightCurrentLine();
}
CodeWidget::~CodeWidget() { delete m_line_numbers; }
void CodeWidget::resizeEvent(QResizeEvent *event) {
	QPlainTextEdit::resizeEvent(event);
	auto cr = contentsRect();
	m_line_numbers->setGeometry(QRect(cr.left(), cr.top(), lineNumberWidth(), cr.height()));
}
int CodeWidget::lineNumberWidth() {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	return 3 + fontMetrics().width(QLatin1Char('9')) * digits;
}
void CodeWidget::highlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> extraSelections;
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(245, 245, 245);
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}
	setExtraSelections(extraSelections);
}
void CodeWidget::setLineNumberWidth() {
	setViewportMargins(lineNumberWidth(), 0, 0, 0);
}
#include <QPainter>
#include <QTextBlock>
void CodeWidget::lineNumbersPaintEvent(QPaintEvent *event) {
	QPainter painter(m_line_numbers);
	painter.fillRect(event->rect(), QColor(240, 240, 240));

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_line_numbers->width(), fontMetrics().height(),
							 Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}