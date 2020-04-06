/// @author Roberto Preghenella
/// @since  2020-04-01

#include "Framework/Task.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/ConfigParamSpec.h"
#include "Framework/ConfigParamRegistry.h"
#include "Framework/ControlService.h"
#include "Framework/CallbackService.h"
#include "Framework/ConcreteDataMatcher.h"
#include "Framework/RawDeviceService.h"
#include "Framework/DeviceSpec.h"
#include <fairmq/FairMQDevice.h>

using namespace o2::framework;

class InputProxyTask : public Task
{
 public:
  InputProxyTask() = default;
  ~InputProxyTask() override = default;
  virtual void init(InitContext &ic) = 0;
  virtual void run(ProcessingContext &pc) = 0;
  
 protected:
  void dumpPayload(const char *buffer, int payload);
  void sendPayload(ProcessingContext &pc, const char *buffer, int payload);
};

void
InputProxyTask::dumpPayload(const char *buffer, int payload)
{
  std::cout << " --- dump payload: " << payload << " bytes" << std::endl;
  auto word = reinterpret_cast<const uint32_t *>(buffer);
  for (int i = 0; i < payload / 4; ++i)
    printf(" 0x%08x \n", *word++);
  std::cout << " --- dump payload: done " << std::endl;
}

void
InputProxyTask::sendPayload(ProcessingContext &pc, const char *buffer, int payload)
{

  /** output **/
  auto device = pc.services().get<o2::framework::RawDeviceService>().device();
  auto outputRoutes = pc.services().get<o2::framework::RawDeviceService>().spec().outputs;
  auto fairMQChannel = outputRoutes.at(0).channel;  
  auto payloadMessage = device->NewMessage(payload);
  std::memcpy(payloadMessage->GetData(), buffer, payload);
  o2::header::DataHeader header("RAWDATA", "TOF", 0);
  header.payloadSize = payload;
  o2::framework::DataProcessingHeader dataProcessingHeader{0};
  o2::header::Stack headerStack{header, dataProcessingHeader};
  auto headerMessage = device->NewMessage(headerStack.size());
  std::memcpy(headerMessage->GetData(), headerStack.data(), headerStack.size());
  
  /** send **/
  FairMQParts parts;
  parts.AddPart(std::move(headerMessage));
  parts.AddPart(std::move(payloadMessage));
  device->Send(parts, fairMQChannel);

};
