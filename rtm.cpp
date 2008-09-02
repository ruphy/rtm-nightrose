 
#include "rtm.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
 
#include <plasma/svg.h>
#include <plasma/theme.h>

#include "rtmapi.h"
// #include "rtm/rtmapi.c"

Rtm::Rtm(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_svg(this),
    m_icon("plasma")
{
    // this will get us the standard applet background, for free!
    setBackgroundHints(Plasma::Applet::StandardBackground);
    resize(200, 200);
}
 
 
Rtm::~Rtm()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}
 
void Rtm::init()
{
    rtm_api *rtm = rtm_api_init("4375efc5a88a6917485c7864736d5eef", "68fc073a483defd7");


    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, "No world to say hello");
    }
} 
 
 
void Rtm::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);
 
    // Now we draw the applet, starting with our svg
    m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
    m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());
 
    // We place the icon and text
    p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(),(int)contentsRect.width()-14));
    p->save();
    p->setPen(Qt::white);
    p->drawText(contentsRect,
                Qt::AlignBottom | Qt::AlignHCenter,
                "Hello Plasmoid!");
    p->restore();
}
 
#include "rtm.moc"
