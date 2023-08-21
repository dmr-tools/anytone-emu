#include "device.hh"
#include "request.hh"
#include "response.hh"
#include "model.hh"

#include <QIODevice>
#include "logger.hh"


Device::Device(QIODevice *interface, Model* model, QObject *parent)
  : QObject{parent}, _state(State::Initial), _interface(interface), _model(model),
    _in_buffer(), _out_buffer()
{
  _interface->setParent(this);
  connect(_interface, SIGNAL(readyRead()), this, SLOT(onBytesAvailable()));
  connect(_interface, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));

  if (! _interface->open(QIODevice::ReadWrite)) {
    logError() << "Cannot open interface: " << _interface->errorString();
    disconnect(_interface, &QIODevice::readyRead, this, &Device::onBytesAvailable);
    disconnect(_interface, &QIODevice::bytesWritten, this, &Device::onBytesWritten);
  }

  if (nullptr != _model) {
    _model->setParent(this);
    connect(this, &Device::startProgram, _model, &Model::startProgram);
    connect(this, &Device::endProgram, _model, &Model::endProgram);
  }

  _elementDescriptions = {
    {0x00800000,
     {"Channel bank 0",
      "A bank of 32 channels."}},
    {0x00840000,
     {"Channel bank 1",
      "A bank of 32 channels."}},
    {0x00880000,
     {"Channel bank 2",
      "A bank of 32 channels."}},
    {0x008c0000,
     {"Channel bank 3",
      "A bank of 32 channels."}},
    {0x00900000,
     {"Channel bank 4",
      "A bank of 32 channels."}},
    {0x00940000,
     {"Channel bank 5",
      "A bank of 32 channels."}},
    {0x00980000,
     {"Channel bank 6",
      "A bank of 32 channels."}},
    {0x009c0000,
     {"Channel bank 7",
      "A bank of 32 channels."}},
    {0x00a00000,
     {"Channel bank 8",
      "A bank of 32 channels."}},
    {0x00a40000,
     {"Channel bank 9",
      "A bank of 32 channels."}},
    {0x00a80000,
     {"Channel bank 10",
      "A bank of 32 channels."}},
    {0x00ac0000,
     {"Channel bank 11",
      "A bank of 32 channels."}},
    {0x00b00000,
     {"Channel bank 12",
      "A bank of 32 channels."}},
    {0x00b40000,
     {"Channel bank 13",
      "A bank of 32 channels."}},
    {0x00b80000,
     {"Channel bank 14",
      "A bank of 32 channels."}},
    {0x00bc0000,
     {"Channel bank 15",
      "A bank of 32 channels."}},
    {0x00c00000,
     {"Channel bank 16",
      "A bank of 32 channels."}},
    {0x00c40000,
     {"Channel bank 17",
      "A bank of 32 channels."}},
    {0x00c80000,
     {"Channel bank 18",
      "A bank of 32 channels."}},
    {0x00cc0000,
     {"Channel bank 19",
      "A bank of 32 channels."}},
    {0x00d00000,
     {"Channel bank 20",
      "A bank of 32 channels."}},
    {0x00d40000,
     {"Channel bank 21",
      "A bank of 32 channels."}},
    {0x00d80000,
     {"Channel bank 22",
      "A bank of 32 channels."}},
    {0x00dc0000,
     {"Channel bank 23",
      "A bank of 32 channels."}},
    {0x00e00000,
     {"Channel bank 24",
      "A bank of 32 channels."}},
    {0x00e40000,
     {"Channel bank 25",
      "A bank of 32 channels."}},
    {0x00e80000,
     {"Channel bank 26",
      "A bank of 32 channels."}},
    {0x00ec0000,
     {"Channel bank 27",
      "A bank of 32 channels."}},
    {0x00f00000,
     {"Channel bank 28",
      "A bank of 32 channels."}},
    {0x00f40000,
     {"Channel bank 29",
      "A bank of 32 channels."}},
    {0x00f80000,
     {"Channel bank 30",
      "A bank of 32 channels."}},
    {0x00fc0000,
     {"Channel bank 31",
      "A bank of 32 channels."}},
    {0x00fc0800,
     {"VFO Channels",
      "The two VFO channels (A & B)"}},
    {0x01000000,
     {"Zone channel lists.",
      "A list of 250 lists of 250 channel indices each."}},
    {0x01080000,
     {"Scan lists bank 0",
      "A bank of up to 16 scan lists."}},
    {0x010c0000,
     {"Scan lists bank 1",
      "A bank of up to 16 scan lists."}},
    {0x024c1300,
     {"Zone bitmap",
      "Encodes valid zones, using one bit per zone."}},
    {0x024c1500,
     {"Channel bitmap",
      "Encodes valid channels, using one bit per channel."}},
    {0x025c0b10,
     {"Group list bitmap",
      "Inidcates which of the 250 RX group lists are set."}},
    {0x024C1340,
     {"Scan list bitmap",
      "Indicates, which of the 250 scan lists are set."}},
    {0x02540000,
     {"Zone names",
      "A list of 250 zone names, 16 ASCII chars and 0-padded to 32b each."}},
    {0x02600000,
     {"Contact indices",
      "Index list of up to 10000 contacts."}},
    {0x02640000,
     {"Contact bitmap",
      "Inverted bitmap, one bit for each of the possible 10000 contacts."}},
    {0x02680000,
     {"Contact bank 0",
      "A bank containing 1000 contacts."}},
    {0x026c0000,
     {"Contact bank 1",
      "A bank containing 1000 contacts."}},
    {0x02700000,
     {"Contact bank 2",
      "A bank containing 1000 contacts."}},
    {0x02740000,
     {"Contact bank 3",
      "A bank containing 1000 contacts."}},
    {0x02780000,
     {"Contact bank 4",
      "A bank containing 1000 contacts."}},
    {0x027c0000,
     {"Contact bank 5",
      "A bank containing 1000 contacts."}},
    {0x02800000,
     {"Contact bank 6",
      "A bank containing 1000 contacts."}},
    {0x02840000,
     {"Contact bank 7",
      "A bank containing 1000 contacts."}},
    {0x02880000,
     {"Contact bank 8",
      "A bank containing 1000 contacts."}},
    {0x028c0000,
     {"Contact bank 9",
      "A bank containing 1000 contacts."}},
    {0x02900000,
     {"DTMF contact indices",
      "Index list of valid analog contacts."}},
    {0x02900100,
     {"DTMF contact byte map",
      "A byte map, indicating which DTMF contacts are set."}},
    {0x02940000,
     {"DTMF contacts",
      "Up to 128 DMTF contacts."}},
    {0x02980000,
     {"Group lists",
      "Up to 250 RX group lists."}},
    {0x04340000,
     {"Contact look-up table",
      "Maps DMR IDs to contacts."}}
  };
}


void
Device::onBytesAvailable() {
  _in_buffer.append(_interface->readAll());

  while (Request *req = Request::fromBuffer(_in_buffer)) {
    Response *resp = this->handle(req);
    delete req;
    if (resp) {
      if (resp->serialize(_out_buffer))
        onBytesWritten();
      delete resp;
    }
  }
}


void
Device::onBytesWritten() {
  if (0 == _out_buffer.size())
    return;

  qint64 nbytes = _interface->write(_out_buffer);
  _out_buffer.remove(0, nbytes);
}


Response *
Device::handle(Request *request) {
  if (request->is<ProgramRequest>()) {
    if (State::Initial == _state)
      emit startProgram();
    logDebug() << "Enter progam mode.";
    _state = State::Program;
    return new ProgramResponse();
  } else if ((State::Program == _state) && request->is<DeviceInfoRequest>()) {
    logDebug() << "Get device info.";
    return new DeviceInfoResponse(this->model(), this->hwVersion());
  } else if ((State::Program == _state) && request->is<ReadRequest>()) {
    ReadRequest *rreq = request->as<ReadRequest>();
    logDebug() << "Read " << (int)rreq->length() << "b from " << Qt::hex << rreq->address() << "h.";
    QByteArray payload; payload.reserve(rreq->length());
    if (! this->read(rreq->address(), rreq->length(), payload))
      return nullptr;
    return new ReadResponse(rreq->address(), payload);
  } else if ((State::Program == _state) && request->is<WriteRequest>()) {
    WriteRequest *wreq = request->as<WriteRequest>();
    logDebug() << "Write " << (int)wreq->payload().size() << "b to " << Qt::hex << wreq->address() << "h.";
    if (! this->write(wreq->address(), wreq->payload()))
      return nullptr;
    return new WriteResponse(wreq->address(), wreq->payload().size());
  } else if (request->is<EndRequest>()) {
    if (State::Program == _state)
      emit endProgram();
    logDebug() << "Done.";
    _state = State::Initial;
    return nullptr;
  }

  logWarn() << "Uknown request.";
  return nullptr;
}


bool
Device::read(uint32_t address, uint8_t len, QByteArray &buffer) {
  int n = std::min(16u, std::min((uint)len, (uint)buffer.capacity()));

  if (0x02fa0000 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x01\x01\x01\x00\x00\x01\x01\x20\x20\x20\x20\xff", n));
  else if (0x02fa0010 == address)
    buffer.append(QByteArray::fromRawData("\x44\x38\x37\x38\x55\x56\x00\x01\x00\xff\xff\xff\xff\xff\xff\xff", n));
  else if (0x02fa0020 == address)
    buffer.append(QByteArray::fromRawData("\xff\xff\xff\xff\x00\x00\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0030 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0040 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0050 == address)
    buffer.append(QByteArray::fromRawData("\x31\x32\x33\x34\x35\x36\x37\x38\xff\xff\xff\xff\xff\xff\xff\xff", n));
  else if (0x02fa0060 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0070 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0080 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0090 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00a0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00b0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00c0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00d0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00e0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00f0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (nullptr != _model)
    return _model->read(address, len, buffer);
  else
    return false;

  return true;
}


bool
Device::write(uint32_t addr, const QByteArray &data) {
  if (nullptr != _model)
    return _model->write(addr, data);
  return false;
}

bool
Device::elementKnown(uint32_t address) const {
  return _elementDescriptions.contains(address);
}

QString
Device::elementName(uint32_t address) const {
  if (elementKnown(address))
    return _elementDescriptions[address].first;
  return "Unkown element";
}

QString
Device::elementDescription(uint32_t address) const {
  if (elementKnown(address))
    return _elementDescriptions[address].second;
  return "";
}
