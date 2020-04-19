#pragma once
#include "BasicLogger.hpp"
#include <qobject.h>

class QLogger : public QObject, public anl::BasicLogger
{
   Q_OBJECT;
public:
   explicit QLogger(const std::string& loggingFormat)
      : BasicLogger(loggingFormat)
   {
   }

   QLogger(const std::string& fileName, const std::string& loggingFormat)
      : BasicLogger(fileName, loggingFormat)
   {
   }

   void log(const std::string& msg, const Level& level) const override
   {
      QString html;
      switch(level)
      {
         case Level::ERR:
            html = "<font color=\"Red\">";
            break;
         case Level::TRACE:
            html = "<font color=\"Green\">";
            break;
         case Level::WARNING:
            html = "<font color=\"Yellow\">";
            break;
         case Level::DEBUG:
            html = "<font color=\"Orange\">";
            break;
         case Level::INFO:
            html = "<font color=\"Black\">";
            break;
      }
      //log.replace(" ", "&nbsp;");
      //log = html + log + "</font>";
      emit logSig(html + "<pre>" + QString::fromStdString(msg) + "</pre></font>");
   }

signals:
   void logSig(const QString& msg) const;
};

