// requires rebdsim libraries to be already loaded
// assumes file in this directory called t1_event.root
{
  gROOT->LoadMacro("average_event_times.C");

  double rms;
  double mean;
  mean = AverageEventTime("output.root", &rms);
  std::cout << "Event duration: " << mean << " ± " << rms << std::endl;
}
