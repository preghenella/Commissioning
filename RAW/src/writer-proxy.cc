/// @author Roberto Preghenella
/// @since  2020-04-01

#include "Framework/Task.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/ConfigParamSpec.h"
#include "Framework/ConfigParamRegistry.h"
#include "Framework/ControlService.h"
#include "Framework/CallbackService.h"
#include "Framework/ConcreteDataMatcher.h"
#include <fairmq/FairMQDevice.h>

using namespace o2::framework;

class WriterProxyTask : public Task
{
 public:
  WriterProxyTask() = default;
  ~WriterProxyTask() override = default;

  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;
  
 private:

  bool mStatus = false;
  std::ofstream mFile;

};

void
WriterProxyTask::init(InitContext& ic)
{
  auto filename = ic.options().get<std::string>("filename");

  /** open output file **/
  std::cout << " --- Opening output file: " << filename << std::endl;
  mFile.open(filename, std::fstream::out | std::fstream::binary);
  if (!mFile.is_open()) {
    std::cout << " --- Cannot open output file: " << strerror(errno) << std::endl;
    mStatus = true;
  }

};

void
WriterProxyTask::run(ProcessingContext& pc)
{

  /** check status **/
  if (mStatus) {
    if (mFile.is_open())
      mFile.close();
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }

  /** receive payload **/
  for (auto& input : pc.inputs()) {

    /** input **/
    auto headerIn = DataRefUtils::getHeader<o2::header::DataHeader*>(input);
    auto dataProcessingHeaderIn = DataRefUtils::getHeader<o2::framework::DataProcessingHeader*>(input);
    auto buffer = input.payload;
    auto payload = headerIn->payloadSize;

    /** write **/
    mFile.write(buffer, payload);
    
  }

};


// add workflow options, note that customization needs to be declared before
// including Framework/runDataProcessing
void customize(std::vector<ConfigParamSpec>& workflowOptions)
{
  auto inputSpec = ConfigParamSpec{"input-spec", VariantType::String, "x:TOF/RAWDATA", {"Input spec string"}};
  workflowOptions.push_back(inputSpec);
}

#include "Framework/runDataProcessing.h" // the main driver

/// This function hooks up the the workflow specifications into the DPL driver.
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  auto inputSpec = cfgc.options().get<std::string>("input-spec");
  
  return WorkflowSpec {
    DataProcessorSpec {"writer-proxy",
	select(inputSpec.c_str()),
	Outputs{},
	AlgorithmSpec(adaptFromTask<WriterProxyTask>()),
	Options{
	  {"filename", VariantType::String, "", {"Output file name"}}}
    }
  };
}
