#include <QCoreApplication>
#include <QtCore>
#include <QtXml>
#include <QDebug>
#include <map>
#include <string>
#include <QString>
#include <iostream>
#include <vector>

using std::map;
using std::multimap;
using std::string;
using std::cout;
using std::endl;
using std::vector;


struct edge{    //data from route

    QString source;
    QString sourceID;
    QString destinationID;
    QString destination;
    size_t stops;
    size_t cost;
    size_t fTime;
    size_t distance; //km
};


struct airline
{
    QString name;
    QString iata;
    QString icao;
    bool active;
};

struct airport
{
    QString airportid;
    QString airportName;
    QString cityName;
    QString country;
    QString iata;
    QString icao;
    double lati;
    double lon;
    std::vector<edge> edges;
};



double getDistance(double latA_rad, double lonA_rad, double latB_rad, double lonB_rad)
{
    double latA = latA_rad * 3.1415926535897 / 180;
    double lonA = lonA_rad * 3.1415926535897 / 180;
    double latB = latB_rad * 3.1415926535897 / 180;
    double lonB = lonB_rad * 3.1415926535897 / 180;

    const double R = 3959;
    const double Rk = 6371;

    // could change from returning Km to Miles
    bool Km_or_Miles = true;

    float cAOB = cos(latA) * cos(latB) * cos(lonB-lonA)+ sin(latA) * sin(latB);

    // angle between the two vector OA and OB
    float AOB = acos(cAOB);
    if(Km_or_Miles)
        return Rk * AOB;
    else
        return R * AOB;
}

void readFromXml(QDomDocument &doc, const QString &filename)
{
    QFile file(filename);
    cout<<"Reading "<<filename.toStdString()<<endl;
    if (!file.open(QIODevice::ReadOnly) )
    {
        qDebug()<<"Cannot open "<<filename<<"!";
        return;
    }
    if ( !doc.setContent(&file))
    {
        qDebug()<<"Cannot parse "<<filename<<"!";
           return;
     }
    cout<<"Finished reading "<<filename.toStdString()<<endl;
}

void loadAirportMap(map<string,airport> &airportMap, QDomDocument &airports)
{
    QDomNodeList elements = airports.documentElement().elementsByTagName( "airport" );
    airport port;
    for(size_t i = 0; i < elements.size(); ++i)
    {
        QDomNode curr = elements.item(i);

        QString id = curr.firstChildElement("AirportID").text();
        port.airportName = curr.firstChildElement("Name").text(),
        port.iata = curr.firstChildElement("IATA_FAA").text(),
        port.icao = curr.firstChildElement("ICAO").text(),
        port.country = curr.firstChildElement("Country").text(),
        port.cityName = curr.firstChildElement("City").text(),
        port.lati = curr.firstChildElement("Latitude").text().toDouble(),
        port.lon = (curr.firstChildElement("Longitude").text().toDouble());
//        qDebug()<<"lat: "<<curr.firstChildElement("Latitude").text()
//               <<"\nlon: "<<curr.firstChildElement("Longitude").text().toDouble()<<endl;
//        cout<<"port: latI: "<<port.lati<<endl;
//        cout<<"port: lon: "<<port.lon<<endl;
        airportMap[id.toStdString()] = port;
    }

    cout<<"size: "<<airportMap.size()<<endl;
}


void loadLineMap(map<string,airline> &airlineMap, QDomDocument &airlines)
{
    QDomNodeList elements = airlines.documentElement().elementsByTagName( "airline" );
    airline port;
    for(size_t i = 0; i < elements.size(); ++i)
    {

        QDomNode curr = elements.item(i);
        if(curr.firstChildElement("Active").text() == "N"||(curr.firstChildElement("ICAO").text().size() == 0 && curr.firstChildElement("ICAO").text().size() == 0))
        {
            continue;
        }
        QString id = curr.firstChildElement("AirlineID").text();
        port.name = curr.firstChildElement("Name").text(),
        port.iata = curr.firstChildElement("IATA").text(),
        port.icao = curr.firstChildElement("ICAO").text(),
        port.active = curr.firstChildElement("Country").text() == "N" ? 0:1;
        airlineMap[id.toStdString()] = port;
    }
    cout<<"size: "<<airlineMap.size()<<endl;
}

void loadRoutesMap(map<string,airport> &airportMap, QDomDocument &routes)
{
    QDomNodeList elements = routes.documentElement().elementsByTagName( "route" );
    edge port;
    for(size_t i = 0; i < elements.size(); ++i)
    {
        QDomNode curr = elements.item(i);
        QString id = curr.firstChildElement("AirlineID").text();
        //qDebug()<<id<<endl;
        port.source = curr.firstChildElement("Name").text(),
        port.sourceID = curr.firstChildElement("SourceAirportID").text(),
        port.destinationID = curr.firstChildElement("DestinationAirportID").text(),
        port.destination = curr.firstChildElement("IATA_FAA").text(),
        port.stops = curr.firstChildElement("ICAO").text().toInt(),
        port.cost = 0 ,
        port.fTime = 0,
        port.distance = getDistance(airportMap[port.sourceID.toStdString()].lati,
                                    airportMap[port.sourceID.toStdString()].lon,
                                    airportMap[port.destinationID.toStdString()].lati,
                                    airportMap[port.destinationID.toStdString()].lon);
//                double d = getDistance(airportMap[port.sourceID.toStdString()].lati,
//                        airportMap[port.sourceID.toStdString()].lon,
//                        airportMap[port.destinationID.toStdString()].lati,
//                        airportMap[port.destinationID.toStdString()].lon);
//                cout<<"sta1: "<<airportMap[port.sourceID.toStdString()].lati<<" \nst2: "<<airportMap[port.sourceID.toStdString()].lon
//                   <<" \ne1: "<<airportMap[port.destinationID.toStdString()].lati
//                    <<"\nne2: "<<airportMap[port.destinationID.toStdString()].lon<<endl;
//                cout<<port.distance<<" d: "<<d<<endl;

    //edge a = (edgedata.find("4210"))->second;
    //qDebug()<<a.Destination<<endl;
}
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDomDocument airports, routes, airlines;
    map<string,airport> airportMap;
    map<string,airline> airlineMap;
    multimap<string,edge> edgedata;
    readFromXml(airports,"airports.xml");
    readFromXml(routes,"routes.xml");
    readFromXml(airlines,"airlines.xml");
    loadAirportMap(airportMap, airports);
    loadRoutesMap(airportMap,routes);
    loadLineMap(airlineMap,airlines);
    return 0;
}

/*
 *
    QCoreApplication a(argc, argv);
        multimap <string, airport> data;

        multimap <string, edge> edgeData;



    QFile *xmlFile = new QFile("routes.xml");
            if (!xmlFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
                    qDebug() << "Cannot open file!"<<endl;
                    return -1;
            }
            else
            {
                qDebug()<<"file opened"<<endl;
            }

    QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlFile);
    qDebug()<<"file opened"<<endl;

    //Parse the XML until we reach end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            // Read next element
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            //If token is just StartDocument - go to next
            if(token == QXmlStreamReader::StartDocument) {
                    continue;
            }
            qDebug()<<"file opened"<<token<<endl;
            //If token is StartElement - read it
            if(token == QXmlStreamReader::StartElement) {
                    if(xmlReader->name() == "Airline" || "AirlineID") {
                            continue;
                    }

                    if(xmlReader->name() == "SourceAirport")
                    {

                    }
                    if(xmlReader->name() == "SourceAirportID")
                    {

                    }
                    if(xmlReader->name() == "DestinationAirport")
                    {

                    }
                    if(xmlReader->name() == "DestinationAirportID")
                    {

                    }

                    if(xmlReader->name() == "Stops")
                    {

                        qDebug()<<xmlReader->readElementText().toInt();
                    }
                    if(xmlReader->name() == "Active") {
                        qDebug() << xmlReader->readElementText();
                    }
                    if(xmlReader->name() == "")
                    {

                    }
                    qDebug()<<"file opened"<<endl;
            }
    }

    if(xmlReader->hasError()) {
            qDebug()<<"error"<<endl;
            return -1;
    }

    //close reader and flush file
    xmlReader->clear();
    xmlFile->close();



 * */
