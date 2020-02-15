#include <core/command.h>

dev::Command::Command(std::uint8_t const& devId, std::string const& name) noexcept
  : devId(devId),
    name(name) 
{ }

dev::Command::Command(std::uint8_t const& devId, std::string const& name, std::size_t const& index, std::size_t const& subIndex) noexcept
  : devId(devId),
    name(name),
    index(index),
    subIndex(subIndex)
{ }

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

auto dev::Command::response(dev::TransmitData&& transmitData) -> void {
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
      responseProcessing(std::move(transmitData));
      break;
    case dev::Status::DONE:
      //добавить функцию отмены.
      break;
    default:
      break;
  }
}
