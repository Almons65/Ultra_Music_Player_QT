#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

// Custom QLabel that emits a signal when clicked
class ClickableLabel : public QLabel {
    Q_OBJECT  // Enables Qt's signal-slot mechanism

public:
    // Constructor to initialize the label
    explicit ClickableLabel(QWidget *parent = nullptr) : QLabel(parent) {
        setMouseTracking(true);  // Enable tracking mouse movement
        setCursor(Qt::PointingHandCursor);  // Change cursor to hand icon
    }

signals:
    void clicked();  // Signal emitted when the label is clicked

protected:
    // Handles mouse press events
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {  // Check for left mouse button click
            emit clicked();  // Emit the clicked signal
        }
        QLabel::mousePressEvent(event);  // Call the base class event handler
    }
};

#endif // CLICKABLELABEL_H
