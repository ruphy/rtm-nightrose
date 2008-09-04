 
#include "rtm.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QPushButton>
#include <QGraphicsLinearLayout>

#include <KUrlLabel>
 
#include <plasma/svg.h>
#include <plasma/theme.h>
#include <Plasma/PushButton>
#include <Plasma/Label>

#include "rtmapi.h"
// #include "rtm/rtmapi.c"

Rtm::Rtm(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_svg(this),
    m_icon("plasma"),
    m_label(0),
    m_authButton(0),
    m_layout(0)
//     m_ui(0)
{
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    
    w = new QWidget();
    QGraphicsProxyWidget *p = new QGraphicsProxyWidget(this);
    p->setWidget(w);
    m_ui = new Ui::MainWidget;
    m_ui->setupUi(w);
    m_layout->addItem(p);
    
    // this will get us the standard applet background, for free!
    setBackgroundHints(Plasma::Applet::StandardBackground);
    resize(200, 200);
    
    setLayout(m_layout);
}
 
 
Rtm::~Rtm()
{
    delete w;
    delete m_ui;
//     delete m_label;
//     delete m_authButton;
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}
 
void Rtm::init()
{
//    RtmApi rtm("4375efc5a88a6917485c7864736d5eef", "68fc073a483defd7");
//    m_label = new Plasma::Label(); 
//    m_label->setText("<qt><a href="+rtm.getAuthUrl()+"+>Authenticate!</a></qt>");
//    m_label->setText("Authenticate!");
   
//    m_authButton = new Plasma::PushButton(this);
//    m_authButton->setText("I authenticated... continue");
   
//    m_layout->addItem(m_label);
//    m_layout->addItem(m_authButton);

    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, "No world to say hello");
    }
} 
 
 
void Rtm::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
//     p->setRenderHint(QPainter::SmoothPixmapTransform);
//     p->setRenderHint(QPainter::Antialiasing);
//  
//     // Now we draw the applet, starting with our svg
//     m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
//     m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());
//  
//     // We place the icon and text
//     p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(),(int)contentsRect.width()-14));
//     p->save();
//     p->setPen(Qt::white);
//     p->drawText(contentsRect,
//                 Qt::AlignBottom | Qt::AlignHCenter,
//                 m_a);
//     p->restore();
}
 
#include "rtm.moc"
