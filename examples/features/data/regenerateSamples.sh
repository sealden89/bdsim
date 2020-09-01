BDSIM="bdsim"
REBDSIM="rebdsim"
REBDSIMCOMBINE="rebdsimCombine"
REBDSIMOPTICS="rebdsimOptics"

$BDSIM --file=originalmodels/sc.gmad --batch --ngenerate=10 --outfile=sample1 --seed=123
$BDSIM --file=originalmodels/sc.gmad --batch --ngenerate=10 --outfile=sample2 --seed=321
$REBDSIM analysisConfig.txt sample1.root ana1.root
$REBDSIM analysisConfig.txt sample2.root ana2.root
$REBDSIMCOMBINE combined-ana.root ana1.root ana2.root
$BDSIM --file=originalmodels/fodo.gmad --batch --ngenerate=100 --outfile=fodo --seed=456
$REBDSIMOPTICS fodo.root optics.root
