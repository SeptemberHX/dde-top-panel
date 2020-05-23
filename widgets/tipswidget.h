#ifndef TIPSWIDGET_H
#define TIPSWIDGET_H

#include <QFrame>

class TipsWidget : public QFrame
{
    Q_OBJECT
public:
    explicit TipsWidget(QWidget *parent = nullptr);

    const QString& text(){return m_text;}
    void setText(const QString &text);
    void refreshFont();
    
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

#endif // TIPSWIDGET_H
