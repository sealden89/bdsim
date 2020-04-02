BDS=/Users/nevay/physics/reps/bdsim-scoring-build/bdsim

$BDS --file=dose-check.gmad --outfile=d1 --batch --ngenerate=1000 --seed=123
rebdsimHistoMerge d1.root d1-histos.root
python dosecheck.py