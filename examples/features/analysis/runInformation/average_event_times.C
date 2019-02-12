
double AverageEventTime(std::string fileName, double* rms = nullptr)
{
  // Open the file
  TFile* f = new TFile(fileName.c_str(), "READ");
  if (f->IsZombie())
    {std::cout << "Invalid file" << std::endl; return 0;}

  // Get the event 'tree' out of it
  TTree* t = dynamic_cast<TTree*>(f->Get("Event"));
  if (!t)
    {std::cout << "No Event tree in the file!" << std::endl; return 0;}

  // Create an even object
  Event* e = new Event();
  
  // Tell root to use this local event object to overlay data onto.
  e->SetBranchAddress(t);

  // Loop over all events and build a vector of event durations
  std::vector<double> eventTimes;
  for (int i = 0; i < t->GetEntries(); i++)
    {
      t->GetEntry(i);
      eventTimes.push_back((double)e->info->duration);
    }

  // Use a root statistic object to get the mean and rms.
  // Note the name doesn't have to be anything meaningful - it's totally useless.
  TStatistic stats("Mean", eventTimes.size(), eventTimes.data());

  if (rms)
    {*rms = stats.GetRMS();}
  double mean = stats.GetMean();

  // Clean up
  delete e;
  f->Close();
  delete f;

  return mean;
}
