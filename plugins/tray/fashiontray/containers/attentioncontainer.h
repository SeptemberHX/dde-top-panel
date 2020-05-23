#ifndef ATTENTIONCONTAINER_H
#define ATTENTIONCONTAINER_H

#include "abstractcontainer.h"

class AttentionContainer : public AbstractContainer
{
    Q_OBJECT
public:
    explicit AttentionContainer(TrayPlugin *trayPlugin, QWidget *parent = nullptr);

    FashionTrayWidgetWrapper *takeAttentionWrapper();

    // AbstractContainer interface
public:
    bool acceptWrapper(FashionTrayWidgetWrapper *wrapper) Q_DECL_OVERRIDE;
    void refreshVisible() Q_DECL_OVERRIDE;
    void addWrapper(FashionTrayWidgetWrapper *wrapper) Q_DECL_OVERRIDE;
};

#endif // ATTENTIONCONTAINER_H
