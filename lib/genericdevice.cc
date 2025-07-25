#include "genericdevice.hh"

#include "logger.hh"


/* ******************************************************************************************** *
 * GenericRequest
 * ******************************************************************************************** */
GenericRequest::GenericRequest()
{
  // pass...
}

GenericRequest::~GenericRequest()
{
  // pass...
}


/* ******************************************************************************************** *
 * GenericResponse
 * ******************************************************************************************** */
GenericResponse::GenericResponse()
{
  // pass...
}

GenericResponse::~GenericResponse()
{
  // pass...
}



/* ******************************************************************************************** *
 * GenericDevice
 * ******************************************************************************************** */
GenericDevice::GenericDevice(
   QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
 : Device{pattern, handler, parent}, _interface(interface), _in_buffer(), _out_buffer()
{
 _interface->setParent(this);
 connect(_interface, &QIODevice::readyRead, this, &GenericDevice::onBytesAvailable);
 connect(_interface, &QIODevice::bytesWritten, this, &GenericDevice::onBytesWritten);

 if (! _interface->open(QIODevice::ReadWrite)) {
   logError() << "Cannot open interface: " << _interface->errorString();
   disconnect(_interface, &QIODevice::readyRead, this, &GenericDevice::onBytesAvailable);
   disconnect(_interface, &QIODevice::bytesWritten, this, &GenericDevice::onBytesWritten);
 }
}


void
GenericDevice::onBytesAvailable() {
 _in_buffer.append(_interface->readAll());

 bool ok = true;
 ErrorStack err;
 while (auto req = this->parse(_in_buffer, ok, err)) {
   auto resp = this->handle(req);
   delete req;
   if (resp) {
     if (resp->serialize(_out_buffer)) {
       onBytesWritten();
     }
     delete resp;
   }
 }

 if (! ok) {
   logError() << "Cannot receive/handle request: " << err.format();
   return;
 }
}


void
GenericDevice::onBytesWritten() {
 if (0 == _out_buffer.size())
   return;

 qint64 nbytes = _interface->write(_out_buffer);
 _out_buffer.remove(0, nbytes);
}

