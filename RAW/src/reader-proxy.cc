/// @author Roberto Preghenella
/// @since  2020-03-08

#include "input-proxy.hh"

#include "Headers/RAWDataHeader.h"
#include "DataFormatsTOF/RawDataFormat.h"

class ReaderProxyTask : public InputProxyTask
{
 public:
  ReaderProxyTask() = default;
  ~ReaderProxyTask() override = default;

  void init(InitContext& ic) final;
  void run(ProcessingContext& pc) final;
  
 private:

  long readFLP();
  long readCONET();

  bool mStatus = false;
  bool mCONET = false;
  bool mDumpPayload = false;
  std::ifstream mFile;
  char mBuffer[1048576];
};

void
ReaderProxyTask::init(InitContext& ic)
{
  auto filename = ic.options().get<std::string>("filename");
  mCONET = ic.options().get<bool>("CONET");
  mDumpPayload = ic.options().get<bool>("dump-payload");

  /** open input file **/
  std::cout << " --- Opening input file: " << filename << std::endl;
  mFile.open(filename, std::fstream::in | std::fstream::binary);
  if (!mFile.is_open()) {
    std::cout << " --- Cannot open input file: " << strerror(errno) << std::endl;
    mStatus = true;
  }

  if (mCONET) {
    std::cout << " --- CONET mode " << std::endl; 
  }

};

long
ReaderProxyTask::readFLP()
{
  /** read input file **/
  char *pointer = mBuffer;
  std::cout << " --- trying to read 64 bytes " << std::endl;
  if (!mFile.read(pointer, 64)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  long payload = 64;
  auto rdh = reinterpret_cast<o2::header::RAWDataHeader*>(pointer);
  while (!rdh->stop) {
    std::cout << " --- trying to read " << rdh->offsetToNext - rdh->headerSize << " bytes " << std::endl;
    if (!mFile.read(pointer + rdh->headerSize, rdh->offsetToNext - rdh->headerSize)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
    payload += (rdh->offsetToNext - rdh->headerSize);
    pointer += rdh->offsetToNext;
    std::cout << " --- trying to read 64 bytes " << std::endl;
    if (!mFile.read(pointer, 64)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
    payload += 64;
    rdh = reinterpret_cast<o2::header::RAWDataHeader*>(pointer);
  }
  
  return payload;
}

long
ReaderProxyTask::readCONET()
{

  char *pointer = mBuffer;
  const uint32_t *word;

  /** read TOF data header **/
  if (!mFile.read(pointer, 4)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  word = reinterpret_cast<const uint32_t *>(pointer);
  if (*word == 0x00080000) {
    printf(" --- unrecognised word: %08x \n ", *word);
    if (!mFile.read(pointer, 4)) {
      std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
      mStatus = true;
      return 0;      
    }
  }
  auto tofDataHeader = reinterpret_cast<const o2::tof::raw::TOFDataHeader_t*>(pointer);
  auto bytePayload = tofDataHeader->bytePayload;

  /** read payload **/
  if (!mFile.read(pointer + 4, bytePayload)) {
    std::cout << " --- Cannot read input file: " << strerror(errno) << std::endl;
    mStatus = true;
    return 0;      
  }
  return bytePayload + 4;
}

void
ReaderProxyTask::run(ProcessingContext& pc)
{

  /** check status **/
  if (mStatus) {
    mFile.close();
    pc.services().get<ControlService>().endOfStream();
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }

  /** read data **/
  int payload = 0;
  if (mCONET) payload = readCONET();
  else payload = readFLP();
  if (payload == 0) return;

  /** dump payload **/
  if (mDumpPayload)
    dumpPayload(mBuffer, payload);

  /** send payload **/
  sendPayload(pc, mBuffer, payload);

  /** check end of file **/
  if (mFile.eof()) {
    std::cout << " --- End of file " << std::endl;
    mStatus = true;
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
    DataProcessorSpec {"reader-proxy",
	Inputs{},
	Outputs{OutputSpec(ConcreteDataTypeMatcher{"TOF", "RAWDATA"})},
	AlgorithmSpec(adaptFromTask<ReaderProxyTask>()),
	Options{
	  {"filename", VariantType::String, "", {"Input file name"}},
	  {"dump-payload", VariantType::Bool, false, {"Dump payload before sending it"}},
	  {"CONET", VariantType::Bool, false, {"CONET mode"}}}
    }
  };
}
