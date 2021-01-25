void gdmlDisplayRoot(std::string fileName) {
  TGeoManager::Import(fileName.data());
  gGeoManager->GetTopVolume()->Draw("ogl");
}
