
#ifndef APPSPREVIEWPROVIDER_H
#define APPSPREVIEWPROVIDER_H

#include "previewcontainer.h"

static PreviewContainer *PreviewWindow(const WindowInfoMap &infos, const WindowList &allowClose, const Dock::Position dockPos)
{
    static PreviewContainer *preview;
    if (!preview) {
        preview = new PreviewContainer;
    }

    preview->disconnect();
    preview->setWindowInfos(infos, allowClose);
    preview->updateSnapshots();
    preview->updateLayoutDirection(dockPos);

    return preview;
}

#endif /* APPSPREVIEWPROVIDER_H */
