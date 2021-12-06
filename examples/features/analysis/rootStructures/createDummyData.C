{
  DataDummyClass* d = new DataDummyClass();
  TFile* f = d->CreateEmptyRebdsimFile("dummyTestFile.root", 2);
  d->CreateDirectories(f,"Event.");
}
