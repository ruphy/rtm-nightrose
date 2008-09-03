
class QString;
class QStringList;

class RtmApi {
    
    class Private;

    public:
        RtmApi(const QString &api_key, const QString &secret);
        ~RtmApi();

        QString getAuthUrl();

        void addParam(QString param, QString value, bool reset = false);
        QString parameters();
        QString getSignature();
        QString getUrl(QString method = "", QString base = "http://www.rememberthemilk.com/services/rest/");
        
        void setToken();
    
    public Q_SLOTS:
        bool authenticate();

        // only api key, shared secret and token will be stored.
        QString call(QString method, QString url = "http://www.rememberthemilk.com/services/rest/");
        
        void reset();

    signals:
        void authenticated();

    private:
        RtmApi::Private *d;
};



