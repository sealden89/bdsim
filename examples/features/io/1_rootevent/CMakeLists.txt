simple_testing(io-rootevent "--file=fodoRing.gmad --output=rootevent --circular" "")

# seed state
simple_testing(io-recreate "--file=sc_recreate.gmad" "")

# write the seed out per event
simple_testing(io-write-ascii-seed-state "--file=sc.gmad --writeSeedState" "")

# use ascii seed state
simple_testing(io-load-ascii-seed-state "--file=sc.gmad --seedStateFileName=../../data/output.seedstate.txt --ngenerate=1" "")

# write optional sampler data
simple_testing(io-sampler-extras          "--file=sc_samplerextras.gmad"          "")
simple_testing(io-sampler-extras-all      "--file=sc_samplerextrasall.gmad"       "")
simple_testing(io-sampler-extras-charge   "--file=sc_samplerextras_charge.gmad"   "")
simple_testing(io-sampler-extras-ion      "--file=sc_samplerextras_ion.gmad"      "")
simple_testing(io-sampler-extras-ke       "--file=sc_samplerextras_kineticenergy.gmad" "")
simple_testing(io-sampler-extras-mass     "--file=sc_samplerextras_mass.gmad"     "")
simple_testing(io-sampler-extras-option2  "--file=sc_samplerextras_option2.gmad"  "")
simple_testing(io-sampler-extras-rigidity "--file=sc_samplerextras_rigidity.gmad" "")

# with an ion beam
simple_testing(io-sampler-extras-ion-beam          "--file=sc_samplerextras_ion_beam.gmad"          "")
simple_testing(io-sampler-extras-all-ion-beam      "--file=sc_samplerextrasall_ion_beam.gmad"       "")
simple_testing(io-sampler-extras-charge-ion-beam   "--file=sc_samplerextras_charge_ion_beam.gmad"   "")
simple_testing(io-sampler-extras-ke-ion-beam       "--file=sc_samplerextras_kineticenergy_ion_beam.gmad" "")
simple_testing(io-sampler-extras-mass-ion-beam     "--file=sc_samplerextras_mass_ion_beam.gmad"     "")
simple_testing(io-sampler-extras-option2-ion-beam  "--file=sc_samplerextras_option2_ion_beam.gmad"  "")
simple_testing(io-sampler-extras-rigidity-ion-beam "--file=sc_samplerextras_rigidity_ion_beam.gmad" "")

# write optional eloss data
simple_testing(io-eloss-extras    "--file=sc_elossextras.gmad" "")
simple_testing(io-eloss-extras2   "--file=sc_elossextras2.gmad" "")
simple_testing(io-no-g4data       "--file=sc_nog4data.gmad --ngenerate=1" "")
simple_testing(io-no-model        "--file=sc_no_model.gmad --ngenerate=1" "")
simple_testing(io-no-eloss        "--file=sc_no_eloss.gmad --ngenerate=1" "")

# scoring map
simple_testing(io-scoring-map     "--file=sc_scoringmap.gmad" "")
