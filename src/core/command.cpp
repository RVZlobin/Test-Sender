#include "command.h"

dev::Command::Command(std::string const& name): name(name) {

}

dev::Command::~Command() {

}

auto dev::Command::setId(std::size_t id) -> void {
  this->id = id;
}

auto dev::Command::getId() const -> std::size_t {
  return this->id;
}

auto dev::Command::setStatus(dev::Status const& status) -> void {
        this->status = status;
}

auto dev::Command::getStatus() const -> dev::Status {
  return this->status;
}

auto dev::Command::getTransmitData() const -> dev::TransmitData {
  return this->transmitData;
}

auto dev::Command::response(dev::TransmitData const& transmitData) -> void {
  switch ( this->status ) {
    case dev::Status::SPIRIT:
      return;
      break;
    case dev::Status::NEW:
      this->status = dev::Status::MIRROR;
      break;
    case dev::Status::MIRROR:
      this->status = dev::Status::FLOW;
      break;
    case dev::Status::FLOW:
      break;
    case dev::Status::COMPLETION:
      break;
    case dev::Status::DONE:
      break;
    default:
      break;
  }
  responseProcessing(transmitData);
}
