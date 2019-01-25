#include "command.h"

pfr::Command::Command(std::string const& name): name(name) {

}

pfr::Command::~Command() {

}

auto pfr::Command::setId(std::size_t id) -> void {
  this->id = id;
}

auto pfr::Command::getId() const -> std::size_t {
  return this->id;
}

auto pfr::Command::setStatus(pfr::Status const& status) -> void {
        this->status = status;
}

auto pfr::Command::getStatus() const -> pfr::Status {
  return this->status;
}

auto pfr::Command::getTransmitData() const -> dev::TransmitData {
  return this->transmitData;
}

auto pfr::Command::response(dev::TransmitData const& transmitData) -> void {
  switch ( this->status ) {
    case pfr::Status::SPIRIT:
      return;
      break;
    case pfr::Status::NEW:
      this->status = pfr::Status::MIRROR;
      break;
    case pfr::Status::MIRROR:
      this->status = pfr::Status::FLOW;
      break;
    case pfr::Status::FLOW:
      break;
    case pfr::Status::COMPLETION:
      break;
    case pfr::Status::DONE:
      break;
    default:
      break;
  }
  responseProcessing(transmitData);
}
