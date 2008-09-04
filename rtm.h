 // Here we avoid loading the header multiple times
#ifndef RTM_HEADER
#define RTM_HEADER
// We need the Plasma Applet headers
#include <KIcon>

#include <Plasma/Applet>
#include <Plasma/Svg>

#include "ui_widj.h"

class QSizeF;
class QGraphicsLinearLayout;
namespace Plasma { class PushButton; class Label; }
// namespace Ui { class MainWidget; }
// class KUrlLabel;

// Define our plasma Applet
class Rtm : public Plasma::Applet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        Rtm(QObject *parent, const QVariantList &args);
        ~Rtm();
 
        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
        void init();
 
    private:
        Plasma::Label *m_label;
        Plasma::PushButton *m_authButton;
        QGraphicsLinearLayout *m_layout;
        QString m_a;
        Plasma::Svg m_svg;
        KIcon m_icon;
        Ui::MainWidget *m_ui;
        QWidget *w;
};
 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(rtm, Rtm)
#endif
