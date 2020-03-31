/// @author Roberto Preghenella
/// @since  2020-03-31

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

#include "Headers/RAWDataHeader.h"
#include "DataFormatsTOF/RawDataFormat.h"

extern "C" {
#include "tofbuf.h"
};

//#define STAGING_RAM_BYTES MAXEVSIZE_B   
//#define STAGING_RAM_WORDS (STAGING_RAM_BYTES/4)

using namespace o2::framework;

class SclProxyTask : public Task
{
 public:
  SclProxyTask() = default;
  ~SclProxyTask() override = default;

  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;
  
 private:

  bool mStatus = false;
  bool mDumpData = false;
  char mBuffer[0x1000000];
};

void
SclProxyTask::init(InitContext& ic)
{

  mDumpData = ic.options().get<bool>("dump-data");

  //  tofbufRule(TOFBUF_NOWAIT);
  tofbufRule(TOFBUF_BLOCKING);
  tofbufMap();

};

void
SclProxyTask::run(ProcessingContext& pc)
{

  /** check status **/
  if (mStatus) {
    pc.services().get<ControlService>().endOfStream();
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }

  /** read data **/
  int bufferPayload = 0;
  int link = tofbufPop(&bufferPayload, reinterpret_cast<unsigned int *>(mBuffer));
  if (link < 0) {
    sleep(1);
    return;
  }
  std::cout << " --- got data buffer from link #" << link << ": " << bufferPayload << " bytes" << std::endl;
  /** end of transmission **/
  if (tofbufCheckEOT(reinterpret_cast<unsigned int *>(mBuffer))) {
    std::cout << " --- end of transmission detected: so long, and thanks for all the bytes" << std::endl;
    mStatus = true;
  }
  /** no payload **/
  if (bufferPayload == 0) 
    return;

  /** scan buffer to find DRM chunks **/
  char *eob = mBuffer + bufferPayload;
  char *pointer = mBuffer;
  while (pointer < eob) {
    uint32_t *word = reinterpret_cast<uint32_t *>(pointer);
    if (!(*word & 0x40000000)) {
      printf(" --- unrecognised word: %08x \n ", *word);
      pointer += 4;
      continue;
    }

    auto tofDataHeader = reinterpret_cast<const o2::tof::raw::TOFDataHeader_t*>(pointer);
    auto payload = tofDataHeader->bytePayload + 4;

    if (mDumpData) {
      std::cout << " --- dump data: " << payload << " bytes" << std::endl;
      word = reinterpret_cast<uint32_t *>(pointer);
      for (int i = 0; i < payload / 4; ++i) {
	printf(" 0x%08x \n", *word);
	word++;
      }
      std::cout << " --- end of dump data " << std::endl;
    }
    
    /** output **/
    auto device = pc.services().get<o2::framework::RawDeviceService>().device();
    auto outputRoutes = pc.services().get<o2::framework::RawDeviceService>().spec().outputs;
    auto fairMQChannel = outputRoutes.at(0).channel;  
    auto payloadMessage = device->NewMessage(payload);
    std::memcpy(payloadMessage->GetData(), pointer, payload);
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
   
    pointer += payload;
  }
};


// add workflow options, note that customization needs to be declared before
// including Framework/runDataProcessing
void customize(std::vector<ConfigParamSpec>& workflowOptions)
{}

#include "Framework/runDataProcessing.h" // the main driver

/// This function hooks up the the workflow specifications into the DPL driver.
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec {
    DataProcessorSpec {"scl-proxy",
	Inputs{},
	Outputs{OutputSpec(ConcreteDataTypeMatcher{"TOF", "RAWDATA"})},
	AlgorithmSpec(adaptFromTask<SclProxyTask>()),
	Options{
	  {"dump-data", VariantType::Bool, false, {"Dump data"}}}
    }
  };
}
