#ifndef NORMALCONTAINER_H
#define NORMALCONTAINER_H

#include "abstractcontainer.h"

class NormalContainer : public AbstractContainer
{
    Q_OBJECT
public:
    explicit NormalContainer(TrayPlugin *trayPlugin, QWidget *parent = nullptr);

    // AbstractContainer interface
public:
    bool acceptWrapper(FashionTrayWidgetWrapper *wrapper) Q_DECL_OVERRIDE;
    void addWrapper(FashionTrayWidgetWrapper *wrapper) Q_DECL_OVERRIDE;
    void refreshVisible() Q_DECL_OVERRIDE;
    void setExpand(const bool expand) Q_DECL_OVERRIDE;
    int itemCount() override;
    QSize sizeHint() const override;
    void updateSize();

protected:
    int whereToInsert(FashionTrayWidgetWrapper *wrapper) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) override;

private:
    int whereToInsertByDefault(FashionTrayWidgetWrapper *wrapper) const;
    int whereToInsertAppTrayByDefault(FashionTrayWidgetWrapper *wrapper) const;
    int whereToInsertSystemTrayByDefault(FashionTrayWidgetWrapper *wrapper) const;
    void compositeChanged();
    void adjustMaxSize(const QSize size);

private:
    mutable QVariantAnimation *m_sizeAnimation;
};

#endif // NORMALCONTAINER_H
